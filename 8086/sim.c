//
//  Created by Matt Hartley on 25/08/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include "core/core.h"
#include "core/sys.h"
#include "instruction_table.h"
#include "shared.h"
#include "sim.h"
#include "decode.h"
#include "disassembly.h"


// TODO: Segmented memory access


cpu_t cpu = {0};
uint8_t memory[0xFFFF << 4] = {0};

int GetTotalClocks();
int CalcMovClocks(rawinstruction_t inst);
int CalcAddClocks(rawinstruction_t inst);
void DisplayInstructionClocksEstimation(rawinstruction_t inst);

char HexNibbleStr(uint8_t value)
{
	return value < 10 ? '0'+value : 'A'+value-10;
}

char* HexStr16(uint16_t value)
{
	uint64_t result = 0;
	// char str[5] = {0};
	char* str = strcopy("0000");
	// sys_zero_memory(str, 5);

	// str[0] = ((reg.lo>>4)&0x0F) < 10 ? '0'+reg.lo : 'A'+reg.lo-10;
	// str[1] = (reg.lo&0x0F) < 10 ? '0'+reg.lo : 'A'+reg.lo-10;

	// str[0] = '0';
	// str[1] = 'x';
	int s = 0;
	str[s+0] = HexNibbleStr((value&0xF000)>>12);
	str[s+1] = HexNibbleStr((value&0x0F00)>>8);
	str[s+2] = HexNibbleStr((value&0x00F0)>>4);
	str[s+3] = HexNibbleStr((value&0x000F));

	return str;
	// sys_copy_memory(&result, str, 5);
	// return result;
}

char* HexByteStr(reg_t reg)
{
	uint64_t result = 0;
	// char str[5] = {0};
	char* str = strcopy("0000");
	// sys_zero_memory(str, 5);

	// str[0] = ((reg.lo>>4)&0x0F) < 10 ? '0'+reg.lo : 'A'+reg.lo-10;
	// str[1] = (reg.lo&0x0F) < 10 ? '0'+reg.lo : 'A'+reg.lo-10;

	// str[0] = '0';
	// str[1] = 'x';
	int s = 0;
	str[s+0] = HexNibbleStr(reg.hi>>4);
	str[s+1] = HexNibbleStr(reg.hi&0x0F);
	str[s+2] = HexNibbleStr(reg.lo>>4);
	str[s+3] = HexNibbleStr(reg.lo&0x0F);

	return str;
	// sys_copy_memory(&result, str, 5);
	// return result;
}

char* BinaryStr16(uint16_t value)
{
	char* str = strcopy("0000 0000 0000 0000");
	int c = 0;
	for (int b=0; b<16; ++b) {
		if (value & (1<<b)) {
			str[18 - (b + b/4)] = '1';
		} else {
			str[18 - (b + b/4)] = '0';
		}
	}

	return str;
}

uint16_t CalcEffectiveAddress(operand_t operand)
{
	assert(operand.type == OPERAND_EFF_ADDR);

	uint16_t effAddr;
	
	if (operand.flags & OPERAND_FLAG_DIRECT_ADDR) {
		effAddr = operand.address;
	} else {
		effAddr = cpu.registers[operand.reg].word;
		if (operand.flags & OPERAND_FLAG_OFFSET_REGISTER) {
			effAddr += cpu.registers[operand.regOff].word;
		}
		effAddr += operand.displacement;
	}

	return effAddr;
}

uint32_t CalcMemoryAddress(operand_t operand)
{
	// NOTE: 20bits are needed for 8086 memory,
	// I'm just using a u32

	assert(operand.type == OPERAND_EFF_ADDR);

	uint16_t effAddr = CalcEffectiveAddress(operand);

	uint32_t addr = (cpu.registers[operand.segreg].word << 4) + effAddr;
	// uint32_t addr = effAddr;
	return addr;
}

void* GetAddressFromOperand(operand_t* operand, bool_t wide)
{
	switch (operand->type) {
		case OPERAND_REG:
			if (!wide && operand->flags & OPERAND_FLAG_HIGH) {
				return &cpu.registers[operand->reg].hi;
			} else {
				return &cpu.registers[operand->reg];
			}

		case OPERAND_IMMEDIATE:
			return &operand->data;

		default:
			return NULL;
	}
}

uint32_t OperandBitSize(operand_t operand)
{
	uint32_t sizes[] = { 8, 16 };
	bool_t wide = operand.flags & OPERAND_FLAG_WIDE;
	return sizes[wide];
}

uint32_t OperandBitMask(operand_t operand)
{
	uint32_t sizes[] = { 0xFF, 0xFFFF };
	bool_t wide = operand.flags & OPERAND_FLAG_WIDE;
	return sizes[wide];
}

uint32_t OperandSignBit(operand_t operand)
{
	uint32_t sizes[] = { 0x80, 0x8000 };
	bool_t wide = operand.flags & OPERAND_FLAG_WIDE;
	return sizes[wide];
}

uint32_t SignExtend(uint32_t value, int bitCount)
{
	// value is a u32 containing the original value of a different size
	
	uint32_t signBit = 1 << (bitCount-1);
	int32_t signedValue = (value ^ signBit) - signBit;
	return signedValue;
}

uint32_t GetOperandValue(operand_t operand, bool_t wide)
{
	uint32_t bitSize = OperandBitSize(operand);
	uint32_t bitMask = OperandBitMask(operand);

	switch (operand.type) {
		case OPERAND_REG:
			if (operand.flags & OPERAND_FLAG_SIGNED) {
				int32_t result;
				if (wide) {
					result = (int16_t)cpu.registers[operand.reg].word;
				} else {
					if (operand.flags & OPERAND_FLAG_HIGH) {
						result = (int8_t)cpu.registers[operand.reg].hi;
					} else {
						result = (int8_t)cpu.registers[operand.reg].lo;
					}
				}
				return result;
			} else {
				if (wide) {
					return cpu.registers[operand.reg].word;
				} else {
					if (operand.flags & OPERAND_FLAG_HIGH) {
						return cpu.registers[operand.reg].hi;
					} else {
						return cpu.registers[operand.reg].lo;
					}
				}
			}	

		case OPERAND_EFF_ADDR: {
			uint32_t addr = CalcMemoryAddress(operand);

			if (operand.flags & OPERAND_FLAG_SIGNED) {
				uint32_t value = *(uint16_t*)(memory + addr);
				return SignExtend(value, bitSize);
			} else {
				// uint32_t addr = GetMemoryAddress(operand);
				uint16_t* value = (uint16_t*)(memory + addr);
				// return *value & (0xFF | (0xFF << (wide*8)));
				return *value & bitMask;
			}
			// if (inst.wide) {

			// } else {

			// }
		} break;

		case OPERAND_IMMEDIATE:
			return operand.data;

		case OPERAND_INCREMENT: {
			int32_t result = operand.displacement;
			return result;
		}

		default:
			// assert(FALSE);
			// print_err(" GetOperandValue for operand type not implemented ");
			return 0;
	}
}

// uint16_t GetInstructionOperandValue(rawinstruction_t inst, int index)
// {
// 	switch (inst.operands[index].type) {
// 		case OPERAND_REG:
// 			if (inst.wide) {
// 				return cpu.registers[inst.operands[index].reg].word;
// 			} else {
// 				if (inst.operands[index].flags & OPERAND_FLAG_HIGH) {
// 					return cpu.registers[inst.operands[index].reg].hi;
// 				} else {
// 					return cpu.registers[inst.operands[index].reg].lo;
// 				}
// 			}

// 		case OPERAND_EFF_ADDR: {
// 			uint32_t addr = GetMemoryAddress(inst.operands[index]);
// 			uint16_t* value = (uint16_t*)(memory + addr);
// 			return *value & (0xFF | (0xFF << (inst.wide*8)));
// 			// if (inst.wide) {

// 			// } else {

// 			// }
// 		} break;

// 		case OPERAND_IMMEDIATE:
// 			return inst.operands[index].data;

// 		default:
// 			// assert(FALSE);
// 			// print_err(" GetOperandValue for operand type not implemented ");
// 			return 0;
// 	}
// }

void StoreInDestination(rawinstruction_t inst, uint16_t value)
{
	operand_t op = inst.operand0;

	switch (op.type) {
		case OPERAND_REG:
			if (inst.wide) {
				cpu.registers[op.reg].word = value;
				// cpu.flags = (value & 0x8000) ? cpu.flags | FLAG_SIGN : cpu.flags & ~FLAG_SIGN;
			} else {
				if (op.flags & OPERAND_FLAG_HIGH) {
					cpu.registers[op.reg].hi = value;
					// cpu.flags |= (value & 0x80) ? (1<<FLAG_SIGN) : 0;
				} else {
					cpu.registers[op.reg].lo = value;
					// cpu.flags |= (value & 0x80) ? (1<<FLAG_SIGN) : 0;
				}
			}
			break;

		case OPERAND_EFF_ADDR: {
			uint32_t addr = CalcMemoryAddress(inst.operand0);
			memory[addr] = value & 0x00FF;
			if (inst.wide) {
				memory[addr+1] = value >> 8;
			}
		} break;
			
		case OPERAND_IMMEDIATE:
			// return inst.operands[index].data;
			assert(FALSE);

		default:
			assert(FALSE);
	}
}

void* GetOperandAddress(rawinstruction_t inst, int index)
{
	return GetAddressFromOperand(&inst.operands[index], inst.wide);
}

void SetCpuFlag(uint8_t flag, bool_t value)
{
	assert(value==0 || value==1);

	if (value){
		cpu.flags |= 1<<flag;
	} else {
		cpu.flags &= ~(1<<flag);
	}
}

uint8_t InstBitWidth(rawinstruction_t inst)
{
	uint8_t a[] = { 8, 16 };
	return a[inst.wide];
}

uint8_t BitWidth(bool_t wide)
{
	uint8_t a[] = { 8, 16 };
	return a[wide];
}

void UpdateCpuFlags(op_t op, uint32_t oldValue, uint32_t value, bool_t wide)
{
	// uint16_t dest = GetOperandValue(inst, 0);
	if (op >= array_size(instructionFlags)) {
		return;
	}

	for (int flag=0; flag<FLAG_COUNT; ++flag) {
		flaglogic_t logic = instructionFlags[op][flag];
		if (logic.enabled && logic.logic != FLAGLOGIC_X) {
			SetCpuFlag(flag, logic.logic);
		}
	}

	flaglogic_t ZFLogic = instructionFlags[op][ZF];
	flaglogic_t SFLogic = instructionFlags[op][SF];
	flaglogic_t* f = instructionFlags[op];
	bool_t cf = f[CF].enabled && (f[CF].logic == FLAGLOGIC_X);
	bool_t pf = f[PF].enabled && (f[PF].logic == FLAGLOGIC_X);
	bool_t of = f[OF].enabled && (f[OF].logic == FLAGLOGIC_X);
	flaglogic_t asd = f[PF];

	// if (cf) {
	// 	uint8_t w = BitWidth(wide);
	// 	SetCpuFlag(CF, value & (1<<BitWidth(wide)));
	// }

	if (pf) {
		// print("\n; %s", BinaryStr16(value));
		int count = 0;
		for (int i=0; i<8; ++i) {
			if (value & (1<<i)) ++count;
		}
		// if (wide) {
		// 	for (int i=8; i<16; ++i) {
		// 		if (value & (1<<i)) ++count;
		// 	}
		// }
		SetCpuFlag(PF, !(count & 1));
	}

	if (f[ZF].enabled && f[ZF].logic == FLAGLOGIC_X) {
		SetCpuFlag(ZF, value == 0);
	}

	if (SFLogic.enabled) {
		if (SFLogic.logic == FLAGLOGIC_X) {
			// bool_t value;
			// if (inst.wide) {

			// } else {
			// 	if (inst.operand0.flags & OPERAND_FLAG_HIGH) {

			// 	} else {

			// 	}
			// }
			// cpu.flags = (value & 0x8000) ? cpu.flags | FLAG_SIGN : cpu.flags & ~FLAG_SIGN;
			SetCpuFlag(SF, value & 0x8000);
			// print(" ; (sign flag %i), %i", value & 0x8000, value);
		}
	}

	// if (of) {
	// 	if (wide) {
	// 		uint16_t asd = (oldValue&0x8000) ^ (value&0x8000);
	// 		SetCpuFlag(OF, (oldValue&0x8000) ^ (value&0x8000));
	// 	} else {
	// 		SetCpuFlag(OF, (oldValue&0x80) ^ (value&0x80));
	// 	}
	// }
}

void SetCarryFlag(bool_t value)
{
	SetCpuFlag(CF, value);
}

void SetAuxCarryFlag(bool_t value)
{
	SetCpuFlag(AF, value);
}

void ConditionalJump(bool_t condition, int16_t displacement)
{
	if (condition) {
		// print("; jumping   ");
		cpu.ip += displacement;
	}
}

void PushStack16(uint16_t value)
{
	cpu.sp.word -= 2;
	uint16_t* mem = (uint16_t*)(memory + (cpu.ss.word<<4) + cpu.sp.word);
	*mem = value;
}

uint16_t PopStack16()
{
	uint16_t* mem = (uint16_t*)(memory + (cpu.ss.word<<4) + cpu.sp.word);
	cpu.sp.word += 2;
	return *mem;
}

void SimInstruction(rawinstruction_t inst)
{
	// uint32_t dest = GetInstructionOperandValue(inst, 0);
	// uint32_t src = GetInstructionOperandValue(inst, 1);
	uint32_t dest = GetOperandValue(inst.operand0, inst.wide);
	uint32_t src = GetOperandValue(inst.operand1, inst.wide);
	uint32_t result = 0;
	// uint16_t oldDest = dest;

	bool_t cf = cpu.flags & FLAG_CARRY;
	bool_t pf = cpu.flags & FLAG_PARITY;
	bool_t af = cpu.flags & FLAG_AUX_CARRY;
	bool_t zf = cpu.flags & FLAG_ZERO;
	bool_t sf = cpu.flags & FLAG_SIGN;
	bool_t of = cpu.flags & FLAG_OVERFLOW;

	int16_t inc = inst.operand1.displacement;

	switch (inst.op) {
		case OP_MOV: {
			// if (inst.operand0.flags & OPERAND_FLAG_WIDE)
			// if (inst.wide) {
			// 	uint16_t* dest = GetAddressFromOperand(&inst.operand0, inst.wide);
			// 	uint16_t* src = GetAddressFromOperand(&inst.operand1, inst.wide);
			// 	// uint16_t src = GetValue16FromOperand(inst.operand1);
			// 	assert(dest && src);
			// 	*dest = *src;
			// } else {
			// 	uint8_t* dest = GetAddressFromOperand(&inst.operand0, inst.wide);
			// 	// uint8_t src = GetValue8FromOperand(inst.operand1);
			// 	uint8_t* src = GetAddressFromOperand(&inst.operand1, inst.wide);
			// 	assert(dest && src);
			// 	*dest = *src;
			// }

			StoreInDestination(inst, src);
		} break;

		case OP_PUSH: {
			cpu.sp.word -= 2;
			uint32_t addr = (cpu.ss.word<<4) + cpu.sp.word;
			uint16_t* mem = (uint16_t*)(memory + (cpu.ss.word<<4) + cpu.sp.word);
			*mem = src;
		} break;
		case OP_POP: {
			uint32_t addr = (cpu.ss.word<<4) + cpu.sp.word;
			uint16_t* mem = (uint16_t*)(memory + (cpu.ss.word<<4) + cpu.sp.word);
			// *mem = src;
			StoreInDestination(inst, *mem);
			cpu.sp.word += 2;
		} break;

		case OP_LEA: {
			uint16_t effAddr = CalcEffectiveAddress(inst.operand1);
			StoreInDestination(inst, effAddr);
		} break;
		case OP_LDS: {
			uint32_t addr = *(uint32_t*)(memory + CalcMemoryAddress(inst.operand1));
			StoreInDestination(inst, addr & 0xFFFF);
			cpu.ds.word = addr >> 16;
		} break;
		case OP_LES: {
			uint32_t addr = *(uint32_t*)(memory + CalcMemoryAddress(inst.operand1));
			StoreInDestination(inst, addr & 0xFFFF);
			cpu.es.word = addr >> 16;
		} break;

		case OP_LAHF: {
			uint8_t flags = 0;
			flags |= cpu.flags & FLAG_CARRY;
			flags |= cpu.flags & FLAG_PARITY;
			flags |= cpu.flags & FLAG_AUX_CARRY;
			flags |= cpu.flags & FLAG_ZERO;
			flags |= cpu.flags & FLAG_SIGN;
			cpu.ax.hi = flags;
		} break;
		case OP_SAHF: {
			uint8_t flags = cpu.ax.hi;
			uint32_t mask = ~(FLAG_CARRY|FLAG_PARITY|FLAG_AUX_CARRY|FLAG_ZERO|FLAG_SIGN);
			cpu.flags &= mask;
			cpu.flags |= flags & FLAG_CARRY;
			cpu.flags |= flags & FLAG_PARITY;
			cpu.flags |= flags & FLAG_AUX_CARRY;
			cpu.flags |= flags & FLAG_ZERO;
			cpu.flags |= flags & FLAG_SIGN;
		} break;
		case OP_PUSHF: {
			cpu.sp.word -= 2;
			uint32_t addr = (cpu.ss.word<<4) + cpu.sp.word;
			uint16_t* mem = (uint16_t*)(memory + addr);
			*mem = cpu.flags;
		} break;
		case OP_POPF: {
			uint32_t addr = (cpu.ss.word<<4) + cpu.sp.word;
			uint16_t* mem = (uint16_t*)(memory + addr);
			cpu.flags = *mem;
			cpu.sp.word += 2;
		} break;
		

		case OP_ADD: {
			// uint8_t* dest = GetAddressFromOperand(&inst.operand0, inst.wide);
			// uint8_t* src = GetAddressFromOperand(&inst.operand1, inst.wide);
			// uint16_t result = dest[0];
			// if (inst.wide) {
			// 	result |= dest[1] << 8;
			// }
			// uint16_t addition = src[0];
			// if (inst.wide) {
			// 	addition |= src[1] << 8;
			// }
			// result += addition;
			// dest[0] = result & 0xFF;
			// if (inst.wide) {
			// 	dest[1] = result >> 8;
			// }

			// if (inst.wide) {
			// 	SetCarryFlag(src > 0xFFFF-dest);
			// } else {
			// 	SetCarryFlag(src > 0xFF-dest);
			// }

			// if (dest < 0x10) {
			// 	SetAuxCarryFlag(src > 0x10-dest);
			// }
			// uint16_t test = dest ^ src;
			// print("\n; %s", BinaryStr16(dest));
			// print("\n; %s", BinaryStr16(src));
			// print("\n; %s", BinaryStr16(dest+src));
			// print("\n; %s", BinaryStr16(dest ^ src));
			// print("\n; %s", BinaryStr16(dest ^ src ^ (dest+src)));
			// print("\n; %s", BinaryStr16((dest ^ src ^ (dest+src)) & 0x10));

			
			result = dest + src;
			StoreInDestination(inst, result);
			
			// flags
			SetCpuFlag(CF, result & (1<<BitWidth(inst.wide)));
			SetCpuFlag(AF, (dest ^ src ^ result) & 0x10);
			if (inst.wide) {
				uint16_t sameSign = (dest&0x8000) == (src&0x8000);
				uint16_t of =  sameSign && (dest&(0x8000)) != (result&0x8000);
				SetCpuFlag(OF, of);
			} else {
				uint16_t sameSign = (dest&0x80) == (src&0x80);
				uint16_t of =  sameSign && (dest&0x80) != (result&0x80);
				SetCpuFlag(OF, of);
			}
		} break;

		case OP_SUB:
		case OP_CMP: {
			result = dest - src;
			if (inst.op == OP_SUB) {
				StoreInDestination(inst, result);
			}

			// flags
			SetCpuFlag(CF, dest < src+cf);
			SetCpuFlag(AF, (dest ^ src ^ result) & 0x10);
			if (inst.wide) {
				uint16_t diffSign = (dest&0x8000) != (src&0x8000);
				uint16_t of =  diffSign && (dest&(0x8000)) != (result&0x8000);
				SetCpuFlag(OF, of);
			} else {
				uint16_t diffSign = (dest&0x80) != (src&0x80);
				uint16_t of =  diffSign && (dest&0x80) != (result&0x80);
				SetCpuFlag(OF, of);
			}
		} break;

		case OP_INC: {
			src = 1;
			result = dest + src;
			StoreInDestination(inst, result);

			SetCpuFlag(AF, (dest ^ src ^ result) & 0x10);
			uint32_t signBit = OperandSignBit(inst.operand0);
			SetCpuFlag(OF, (dest&signBit) ^ (result&signBit));
		} break;

		case OP_DEC: {
			src = 1;
			result = dest - src;
			StoreInDestination(inst, result);

			SetCpuFlag(AF, (dest ^ src ^ result) & 0x10);
			uint32_t signBit = OperandSignBit(inst.operand0);
			SetCpuFlag(OF, (dest&signBit) ^ (result&signBit));
		} break;

		// case OP_CMP: {
		// 	SetCpuFlag(CF, src > dest);
		// 	SetCpuFlag(AF, (dest ^ src ^ (dest-src)) & 0x10);
		// 	dest -= src;
		// } break;

		case OP_MUL: {
			// uint32_t dest = GetOperandValue((operand_t){ .type=OPERAND_REG }, inst.wide);
			uint32_t dest = cpu.ax.word & (0xFFFF >> (!inst.wide)*8);
			result = dest * src;
			if (inst.wide) {
				cpu.ax.word = result & 0xFFFF;
				cpu.dx.word = result >> 16;
			} else {
				cpu.ax.word = result & 0xFFFF;
			}

			uint8_t CFOF = result & (inst.wide ? 0xFFFF0000 : 0xFF00);
			SetCpuFlag(CF, CFOF);
			SetCpuFlag(OF, CFOF);
		} break;
		case OP_IMUL: {
			// int32_t src = (int16_t)GetOperandValue(inst.operand1, inst.wide);
			int32_t dest = (int16_t)(cpu.ax.word & (0xFFFF >> (!inst.wide)*8));
			int32_t iresult = dest * (int32_t)src;
			result = iresult;
			if (inst.wide) {
				cpu.ax.word = (int16_t)iresult;
				cpu.dx.word = iresult >> 16;
			} else {
				cpu.ax.word = iresult & 0xFFFF;
			}

			uint8_t CFOF = result >> (BitWidth(inst.wide)) != (inst.wide ? 0xFFFF : 0xFF);
			SetCpuFlag(CF, CFOF);
			SetCpuFlag(OF, CFOF);
		} break;

		case OP_DIV: {
			if (!src) {
				print_err("Divide by zero exception \n");
			}

			if (inst.wide) {
				uint32_t dxax = cpu.ax.word | (cpu.dx.word << 16);
				uint32_t quotient = dxax / src;
				uint32_t remainder = dxax % src;
				cpu.ax.word = quotient;
				cpu.dx.word = remainder;
			} else {
				// uint32_t dxax = cpu.ax.word | (cpu.dx.word << 16);
				uint32_t quotient = cpu.ax.word / src;
				uint32_t remainder = cpu.ax.word % src;
				cpu.ax.lo = quotient;
				cpu.dx.hi = remainder;
			}
		} break;
		case OP_IDIV: {
			if (!src) {
				print_err("Divide by zero exception \n");
			}

			if (inst.wide) {
				int32_t dxax = cpu.ax.word | (cpu.dx.word << 16);
				int32_t divisor = src;
				int32_t quotient = dxax / divisor;
				int32_t remainder = dxax % divisor;
				cpu.ax.word = quotient;
				cpu.dx.word = remainder;
			} else {
				int32_t dividend = cpu.ax.word;
				int32_t divisor = src;
				int32_t quotient = dividend / divisor;
				int32_t remainder = dividend % divisor;
				cpu.ax.lo = quotient;
				cpu.dx.hi = remainder;
			}
		} break;

		case OP_NOT: {
			result = ~dest;
			StoreInDestination(inst, result);
		} break;
		case OP_SHL: {
			uint32_t result = dest << src;
			StoreInDestination(inst, result);

			uint32_t signBit = OperandSignBit(inst.operand0);
			SetCpuFlag(CF, result & (signBit<<1));
			if (src == 1) {
				SetCpuFlag(OF, (dest&signBit)^(result&signBit));
			}
		} break;
		case OP_SHR: {
			uint32_t result = dest >> src;
			StoreInDestination(inst, result);

			uint32_t signBit = OperandSignBit(inst.operand0);
			SetCpuFlag(CF, result & (signBit<<1));
			if (src == 1) {
				SetCpuFlag(OF, (dest&signBit)^(result&signBit));
			}
		} break;
		case OP_SAR: {
			int32_t result = (int32_t)dest >> (int32_t)src;
			StoreInDestination(inst, result);

			uint32_t signBit = OperandSignBit(inst.operand0);
			SetCpuFlag(CF, result & (signBit<<1));
			if (src == 1) {
				SetCpuFlag(OF, (dest&signBit)^(result&signBit));
			}
		} break;

		case OP_AND: {
			result = dest & src;
			StoreInDestination(inst, result);
		} break;
		case OP_TEST: {
			result = dest & src;
		} break;
		case OP_OR: {
			result = dest | src;
			StoreInDestination(inst, result);
		} break;
		case OP_XOR: {
			result = dest ^ src;
			StoreInDestination(inst, result);
		} break;


		case OP_CALL: {
			uint16_t ip = inst.operand1.data;
			uint16_t cs = inst.operand1.data1;
			if (inst.operand1.type == OPERAND_EFF_ADDR) {
				uint16_t* mem = (uint16_t*)(memory + CalcMemoryAddress(inst.operand1));
				ip = mem[0];
				cs = mem[1];
			}

			if (inst.operand0.flags & OPERAND_FLAG_FAR_ADDR) {
				cpu.sp.word -= 4;
				uint16_t* mem = (uint16_t*)(memory + (cpu.ss.word<<4) + cpu.sp.word);
				mem[0] = cpu.ip;
				mem[1] = cpu.cs.word;
				cpu.ip = ip;
				cpu.cs.word = cs;
			} else {
				cpu.sp.word -= 2;
				uint16_t* mem = (uint16_t*)(memory + (cpu.ss.word<<4) + cpu.sp.word);
				mem[0] = cpu.ip;
				int32_t disp = (int16_t)ip;
				cpu.ip += disp;
			}
		} break;
		case OP_JMP: {
			uint16_t ip = inst.operand1.data;
			uint16_t cs = inst.operand1.data1;
			if (inst.operand1.type == OPERAND_EFF_ADDR) {
				uint16_t* mem = (uint16_t*)(memory + CalcMemoryAddress(inst.operand1));
				ip = mem[0];
				cs = mem[1];
			}

			if (inst.operand0.flags & OPERAND_FLAG_FAR_ADDR) {
				cpu.ip = ip;
				cpu.cs.word = cs;
			} else {
				int32_t disp = (int16_t)ip;
				cpu.ip += disp;
			}
		} break;
		case OP_RET: {
			cpu.ip = PopStack16();
			uint32_t extraPop = src;
			cpu.sp.word += extraPop;
		} break;
		case OP_RETF: {
			cpu.ip = PopStack16();
			cpu.cs.word = PopStack16();
			uint32_t extraPop = src;
			cpu.sp.word += extraPop;
		} break;


		case OP_JZ:
			ConditionalJump(cpu.flags & FLAG_ZERO, inc);
			break;
		// JL
		// JLE
		case OP_JB:
			ConditionalJump(cpu.flags & FLAG_CARRY, inc);
			break;
		// JBE
		case OP_JP:
			ConditionalJump(cpu.flags & FLAG_PARITY, inc);
			break;
		// JO
		// JS

		case OP_JNZ:
			ConditionalJump(~cpu.flags & FLAG_ZERO, inc);
			break;

		// JNL
		// JNLE
		// JNB
		// JNBE
		// JNP
		// JNO
		// JNS
		case OP_LOOP:
			--cpu.cx.word;
			ConditionalJump(cpu.cx.word, inc);
			break;
		case OP_LOOPZ:
			--cpu.cx.word;
			ConditionalJump(cpu.cx.word && (cpu.flags & FLAG_ZERO), inc);
			break;
		case OP_LOOPNZ:
			--cpu.cx.word;
			ConditionalJump(cpu.cx.word && (~cpu.flags & FLAG_ZERO), inc);
			break;
		// JCXZ


		case OP_NOP:
			print("---");
			break;

		default:
			print_err(" Unimplemented operation ");
	}

	UpdateCpuFlags(inst.op, dest, result, inst.wide);
}

void DisplayRegisterChanges(cpu_t previous, cpu_t current)
{
	print("   ; ");
	
	for (int reg=0; reg<12; ++reg) {
		if (previous.registers[reg].word != current.registers[reg].word) {
			print("%s(%s->%s), ", regNames[reg], HexByteStr(previous.registers[reg]), HexByteStr(current.registers[reg]));
		}
	}

	if (previous.flags != current.flags) {
		char oldFlags[32] = {0};
		char newFlags[32] = {0};
		for (int flag=0; flag<9; ++flag) {
			uint16_t bitmask = 1<<flag;
			// if ((previous.flags & bitmask) != (current.flags & bitmask)) {
			// 	print("flag[%s](%i->%i)", cpuFlagNames[flag], (previous.flags & bitmask) >> flag, (current.flags & bitmask) >> flag);
			// }
			if (previous.flags & bitmask) {
				strbappend(oldFlags, strformat("%c", cpuFlagNames[flag][0]), 32);
			}
			if (current.flags & bitmask) {
				strbappend(newFlags, strformat("%c", cpuFlagNames[flag][0]), 32);
			}
		}

		print("flags(%s->%s), ", oldFlags, newFlags);
	}

	print("ip(%u->%u)", cpu.lastIp, cpu.ip);
}

data_t fileData;
void LoadExecutable(data_t file)
{
	fileData = file;
	sys_copy_memory(memory, file.data, file.size);
	memory[file.size] = HALT_OPCODE;
	cpu.ip = 0;
}

void Simulate(bool_t printDisassembly)
{
	print("; Disassembly of 8086 binary \n\nbits 16\n\n");

	for (;;) {
		rawinstruction_t inst = DecodeInstruction(&cpu);

		if (printDisassembly) {
			DisplayInstruction(inst);
		}

		if (inst.op == OP_HLT) {
			// NOTE: Program has reached the end of the instruction stream
			print("; instruction stream ended \n");
			break;
		}

		cpu_t previousCpuState = cpu;
		cpu.ip += inst.size;

		SimInstruction(inst);

		if (printDisassembly) {
			DisplayRegisterChanges(previousCpuState, cpu);
			DisplayInstructionClocksEstimation(inst);
			print("\n");
		}

		cpu.lastIp = cpu.ip;
	}

	print("\n;  CLOCKS \n");
	print("Total clocks estimate: %i \n", GetTotalClocks());

	print("\n;  REGISTERS \n");
	for (int i=0; i<4; ++i) {
		print(
			";  %s %s (%i)    %s %s (%i)    %s %s (%i) \n",
			regNames[i], HexByteStr(cpu.registers[i]), (int16_t)cpu.registers[i].word,
			regNames[i+4], HexByteStr(cpu.registers[i+4]), (int16_t)cpu.registers[i+4].word,
			regNames[i+8], HexByteStr(cpu.registers[i+8]), (int16_t)cpu.registers[i+8].word
		);
	}
	print("\n;  %s %s    %s %s \n", "ip", HexByteStr(*(reg_t*)&cpu.ip), "fg", HexByteStr(*(reg_t*)&cpu.flags));

	print("\n;  FLAGS \n");
	print(";  ");
	for (int i=0; i<12; ++i) {
		print("%s  ", cpuFlagNames[i]);
	}
	print("\n;  ");
	for (int i=0; i<12; ++i) {
		print("%u   ", (cpu.flags & (1<<i)) >> i);
	}

	print("\n\n;  TOP OF STACK \n");
	for (int i=1; i<9; ++i) {
		uint16_t value = *(int16_t*)(memory + (cpu.ss.word<<4) + 0x10000 - (i*2));
		print("stack -%i, %s \n", i*2, HexStr16(value));
	}

	print("\n\n; end of file\n\n");

	// file_t memoryOutputFile = sys_create("build/8086memory.data");
	// if (memoryOutputFile) {
	// 	sys_write(memoryOutputFile, 0, memory, sizeof(memory));
	// 	sys_close(memoryOutputFile);
	// }

	typedef struct __attribute((packed)) {
		char header[2];
		uint32_t size;
		uint16_t reserved1;
		uint16_t reserved2;
		uint32_t offset;
		
		uint32_t headerSize;
		int32_t bitmapWidth;
		int32_t bitmapHeight;
		uint16_t colorPlanes;
		uint16_t colorDepth;
		uint32_t compression;
		uint32_t imageSize;
		int32_t hres;
		int32_t vres;
		uint32_t paletteSize;
		uint32_t importantColors;
	} bmpheader_t;

	bmpheader_t* bmp = sys_alloc_memory(sizeof(bmpheader_t) + 64*64*4);
	*bmp = (bmpheader_t){
		.header = "BM",
		.size = 64*64*4,
		.offset = sizeof(bmpheader_t),

		.headerSize = 40,
		.bitmapHeight = 64,
		.bitmapWidth = 64,
		.colorPlanes = 1,
		.colorDepth = 32,
	};
	sys_copy_memory(bmp+1, memory+0x10000, bmp->size);

	file_t framebufferOutput = sys_create("build/8086framebuffer.bmp");
	if (framebufferOutput) {
		sys_write(framebufferOutput, 0, bmp, sizeof(bmpheader_t)+bmp->size);
		sys_close(framebufferOutput);
	}
}
