//
//  Created by Matt Hartley on 25/08/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include "core/core.h"
#include "core/sys.h"
#include "shared.h"
#include "sim.h"
#include "decode.h"
#include "disassembly.h"


cpu_t cpu;

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
			if (!inst.wide && inst.operands[index].flags & OPERAND_FLAG_HIGH) {
				return cpu.registers[inst.operands[index].reg].hi;
			} else {
				return cpu.registers[inst.operands[index].reg].word;
			}

		case OPERAND_IMMEDIATE:
			return inst.operands[index].data;

		default:
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
			} else {
				if (op.flags & OPERAND_FLAG_HIGH) {
					cpu.registers[op.reg].hi = value;
				} else {
					cpu.registers[op.reg].lo = value;
				}
			}
			break;
			
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

void SimInstruction(rawinstruction_t inst)
{
	uint16_t dest = GetOperandValue(inst, 0);
	uint16_t src = GetOperandValue(inst, 1);

	switch (inst.op) {
		case OP_MOV: {
			// if (inst.operand0.flags & OPERAND_FLAG_WIDE)
			if (inst.wide) {
				uint16_t* dest = GetAddressFromOperand(&inst.operand0, inst.wide);
				uint16_t* src = GetAddressFromOperand(&inst.operand1, inst.wide);
				// uint16_t src = GetValue16FromOperand(inst.operand1);
				assert(dest && src);
				*dest = *src;
			} else {
				uint8_t* dest = GetAddressFromOperand(&inst.operand0, inst.wide);
				// uint8_t src = GetValue8FromOperand(inst.operand1);
				uint8_t* src = GetAddressFromOperand(&inst.operand1, inst.wide);
				assert(dest && src);
				*dest = *src;
			}
		} break;

		case OP_ADD: {
			uint8_t* dest = GetAddressFromOperand(&inst.operand0, inst.wide);
			uint8_t* src = GetAddressFromOperand(&inst.operand1, inst.wide);
			uint16_t result = dest[0];
			if (inst.wide) {
				result |= dest[1] << 8;
			}
			uint16_t addition = src[0];
			if (inst.wide) {
				addition |= src[1] << 8;
			}
			result += addition;
			dest[0] = result & 0xFF;
			if (inst.wide) {
				// dest[1] = (result & 0xFF00) >> 8;
				dest[1] = result >> 8;
			}
		} break;

		case OP_SUB: {
			uint16_t result = dest - src;
			StoreInDestination(inst, result);
		} break;

		default:
			print_err("Unimplemented operation \n");
	}
}

char HexNibbleStr(uint8_t value)
{
	return value < 10 ? '0'+value : 'A'+value-10;
}

char* HexByteStr(reg_t reg)
{
	uint64_t result = 0;
	// char str[5] = {0};
	char* str = strcopy("0x0000");
	sys_zero_memory(str, 5);

	// str[0] = ((reg.lo>>4)&0x0F) < 10 ? '0'+reg.lo : 'A'+reg.lo-10;
	// str[1] = (reg.lo&0x0F) < 10 ? '0'+reg.lo : 'A'+reg.lo-10;

	str[0] = '0';
	str[1] = 'x';
	str[2] = HexNibbleStr(reg.hi>>4);
	str[3] = HexNibbleStr(reg.hi&0x0F);
	str[4] = HexNibbleStr(reg.lo>>4);
	str[5] = HexNibbleStr(reg.lo&0x0F);

	return str;
	// sys_copy_memory(&result, str, 5);
	// return result;
}

void Simulate(data_t file, bool_t printDisassembly)
{
	cpu.instructionData = file.data;

	print("; Disassembly of 8086 binary \n\nbits 16\n\n");

	// cpu.ax.word = 3;
	// cpu.bx.word = 1024;
	// cpu.flags |= FLAG_PARITY;
	// cpu.flags |= FLAG_ZERO;

	while (cpu.ip < file.size) {
		rawinstruction_t inst = DecodeInstruction(&cpu);

		if (printDisassembly) {
			DisplayInstruction(inst);
		}

		SimInstruction(inst);
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
}
