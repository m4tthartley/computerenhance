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

char HexNibbleStr(uint8_t value)
{
	return value < 10 ? '0'+value : 'A'+value-10;
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

uint32_t GetMemoryAddress(operand_t operand)
{
	// NOTE: 20bits are needed for 8086 memory,
	// I'm just using a u32

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

uint16_t GetOperandValue(rawinstruction_t inst, int index)
{
	switch (inst.operands[index].type) {
		case OPERAND_REG:
			if (inst.wide) {
				return cpu.registers[inst.operands[index].reg].word;
			} else {
				if (inst.operands[index].flags & OPERAND_FLAG_HIGH) {
					return cpu.registers[inst.operands[index].reg].hi;
				} else {
					return cpu.registers[inst.operands[index].reg].lo;
				}
			}

		case OPERAND_EFF_ADDR: {
			uint32_t addr = GetMemoryAddress(inst.operands[index]);
			uint16_t* value = (uint16_t*)(memory + addr);
			return *value & (0xFF | (0xFF << (inst.wide*8)));
			// if (inst.wide) {

			// } else {

			// }
		} break;

		case OPERAND_IMMEDIATE:
			return inst.operands[index].data;

		default:
			// assert(FALSE);
			// print_err(" GetOperandValue for operand type not implemented ");
			return 0;
	}
}

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
			uint32_t addr = GetMemoryAddress(inst.operand0);
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

void UpdateCpuFlags(op_t op, uint16_t oldValue, uint16_t value, bool_t wide)
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

void SimInstruction(rawinstruction_t inst)
{
	uint16_t dest = GetOperandValue(inst, 0);
	uint16_t src = GetOperandValue(inst, 1);
	uint16_t oldDest = dest;

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

			if (inst.wide) {
				SetCarryFlag(src > 0xFFFF-dest);
			} else {
				SetCarryFlag(src > 0xFF-dest);
			}

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
			SetAuxCarryFlag((dest ^ src ^ (dest+src)) & 0x10);

			dest += src;
			StoreInDestination(inst, dest);

			// overflow flag
			if (inst.wide) {
				uint16_t sameSign = (oldDest&0x8000) == (src&0x8000);
				uint16_t of =  sameSign && (oldDest&(0x8000)) != (dest&0x8000);
				SetCpuFlag(OF, of);
			} else {
				uint16_t sameSign = (oldDest&0x80) == (src&0x80);
				uint16_t of =  sameSign && (oldDest&0x80) != (dest&0x80);
				SetCpuFlag(OF, of);
			}
		} break;

		case OP_SUB:
		case OP_CMP: {
			SetCpuFlag(CF, src > dest);
			SetCpuFlag(AF, (dest ^ src ^ (dest-src)) & 0x10);

			dest -= src;
			if (inst.op == OP_SUB) {
				StoreInDestination(inst, dest);
			}

			// overflow flag
			if (inst.wide) {
				uint16_t diffSign = (oldDest&0x8000) != (src&0x8000);
				uint16_t of =  diffSign && (oldDest&(0x8000)) != (dest&0x8000);
				SetCpuFlag(OF, of);
			} else {
				uint16_t diffSign = (oldDest&0x80) != (src&0x80);
				uint16_t of =  diffSign && (oldDest&0x80) != (dest&0x80);
				SetCpuFlag(OF, of);
			}
		} break;

		// case OP_CMP: {
		// 	SetCpuFlag(CF, src > dest);
		// 	SetCpuFlag(AF, (dest ^ src ^ (dest-src)) & 0x10);
		// 	dest -= src;
		// } break;


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

		default:
			print_err(" Unimplemented operation ");
	}

	UpdateCpuFlags(inst.op, oldDest, dest, inst.wide);
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
			print("\n");
		}

		cpu.lastIp = cpu.ip;
	}

	print("\n;  REGISTERS \n");
	for (int i=0; i<4; ++i) {
		print(";  %s %s    %s %s    %s %s \n", regNames[i], HexByteStr(cpu.registers[i]), regNames[i+4], HexByteStr(cpu.registers[i+4]), regNames[i+8], HexByteStr(cpu.registers[i+8]));
	}
	print("\n;  %s %s    %s %s \n", "ip", HexByteStr(*(reg_t*)&cpu.ip), "fg", HexByteStr(*(reg_t*)&cpu.flags));

	print("\n;  FLAGS \n");
	print(";  ");
	for (int i=0; i<9; ++i) {
		print("%s  ", cpuFlagNames[i]);
	}
	print("\n;  ");
	for (int i=0; i<9; ++i) {
		print("%u   ", (cpu.flags & (1<<i)) >> i);
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
