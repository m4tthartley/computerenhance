//
//  Created by Matt Hartley on 22/08/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <core/core.h>
#include "shared.h"


typedef enum {
	OPCODE_NULL = 0,

	OPCODE_MOV_RM_TOFROM_REG,
	OPCODE_MOV_IM_TO_RM,
	OPCODE_MOV_IM_TO_REG,
	OPCODE_MOV_MEM_TO_ACC,
	OPCODE_MOV_ACC_TO_MEM,
	OPCODE_MOV_RM_TO_SEG,
	OPCODE_MOV_SEG_TO_RM,
} opcode_t;

const char* opcodeNames[] = {
	"OPCODE_NULL             ",
	"OPCODE_MOV_RM_TOFROM_REG",
	"OPCODE_MOV_IM_TO_RM     ",
	"OPCODE_MOV_IM_TO_REG    ",
	"OPCODE_MOV_MEM_TO_ACC   ",
	"OPCODE_MOV_ACC_TO_MEM   ",
	"OPCODE_MOV_RM_TO_SEG    ",
	"OPCODE_MOV_SEG_TO_RM    ",
};

typedef struct {
	opcode_t opcode;
	uint8_t flags;
	uint8_t d;
	uint8_t w;
	uint8_t reg;
	uint8_t rm;
} opdef_t;

// enum {} datatype_t

opdef_t opdefs[] = {
	// MOV rm to/from reg
	[0b10001000] = { OPCODE_MOV_RM_TOFROM_REG, .d=0, .w=0 },
	[0b10001001] = { OPCODE_MOV_RM_TOFROM_REG, .d=0, .w=1 },
	[0b10001010] = { OPCODE_MOV_RM_TOFROM_REG, .d=1, .w=0 },
	[0b10001011] = { OPCODE_MOV_RM_TOFROM_REG, .d=1, .w=1 },

	// MOV immediate to rm
	[0b11000110] = { OPCODE_MOV_IM_TO_RM },
	[0b11000111] = { OPCODE_MOV_IM_TO_RM, .w=1 },

	// OPCODE_MOV_IM_TO_REG
	[0b10110000] = { OPCODE_MOV_IM_TO_REG, .w=0, .reg=0b000 },
	[0b10110001] = { OPCODE_MOV_IM_TO_REG, .w=0, .reg=0b001 },
	[0b10110010] = { OPCODE_MOV_IM_TO_REG, .w=0, .reg=0b010 },
	[0b10110011] = { OPCODE_MOV_IM_TO_REG, .w=0, .reg=0b011 },
	[0b10110100] = { OPCODE_MOV_IM_TO_REG, .w=0, .reg=0b100 },
	[0b10110101] = { OPCODE_MOV_IM_TO_REG, .w=0, .reg=0b101 },
	[0b10110110] = { OPCODE_MOV_IM_TO_REG, .w=0, .reg=0b110 },
	[0b10110111] = { OPCODE_MOV_IM_TO_REG, .w=0, .reg=0b111 },

	[0b10111000] = { OPCODE_MOV_IM_TO_REG, .w=1, .reg=0b000 },
	[0b10111001] = { OPCODE_MOV_IM_TO_REG, .w=1, .reg=0b001 },
	[0b10111010] = { OPCODE_MOV_IM_TO_REG, .w=1, .reg=0b010 },
	[0b10111011] = { OPCODE_MOV_IM_TO_REG, .w=1, .reg=0b011 },
	[0b10111100] = { OPCODE_MOV_IM_TO_REG, .w=1, .reg=0b100 },
	[0b10111101] = { OPCODE_MOV_IM_TO_REG, .w=1, .reg=0b101 },
	[0b10111110] = { OPCODE_MOV_IM_TO_REG, .w=1, .reg=0b110 },
	[0b10111111] = { OPCODE_MOV_IM_TO_REG, .w=1, .reg=0b111 },

	// OPCODE_MOV_MEM_TO_ACC
	[0b10100000] = { OPCODE_MOV_MEM_TO_ACC, .w=0 },
	[0b10100001] = { OPCODE_MOV_MEM_TO_ACC, .w=1 },

	// OPCODE_MOV_ACC_TO_MEM
	[0b10100010] = { OPCODE_MOV_ACC_TO_MEM, .w=0 },
	[0b10100011] = { OPCODE_MOV_ACC_TO_MEM, .w=1 },

	// OPCODE_MOV_RM_TO_SEG
	[0b10001110] = { OPCODE_MOV_RM_TO_SEG },

	// OPCODE_MOV_SEG_TO_RM
	[0b10001100] = { OPCODE_MOV_SEG_TO_RM },
};

void PrintMovMemoryOperand(uint8_t mod, uint8_t rm, int16_t disp, bool_t w)
{
	if (mod == 0b11) {
		print("%s", regNames[w*8 + rm]);
	} else {
		efaddr_t addr = effectiveAddressTable[rm];
		if (!mod && rm==0b110) {
			addr.flags &= ~EFADDR_BASE;
		}

		print("[");
		if (addr.flags & EFADDR_BASE) {
			print("%s", regNames[8 + addr.baseReg]);
		}
		if (addr.flags & EFADDR_OFF) {
			print(" + %s", regNames[8 + addr.offReg]);
		}
		if (disp && (addr.flags & EFADDR_DISP16 || mod == 0b01 || mod == 0b10)) {
			char* sign = disp < 0 ? "-" : "+";
			if (addr.flags & EFADDR_BASE) {
				print(" %s ", sign);
			}
			print("%i", abs(disp));
		}
		print("]");
	}

}

void PrintImmediate(uint16_t data, bool_t w)
{
	if (w) {
		print("word %u", data);
	} else {
		print("byte %u", data);
	}
}

void DecodeStream(data_t file)
{
	print("; Disassembly of 8086 binary \n\nbits 16\n\n");

	uint8_t* ip = file.data;
	while (ip < file.data+file.size) {
		opdef_t def = opdefs[*ip];
		// print("%s   ", opcodeNames[def.opcode]);

		// (*ip & 0b11111100) == 0b10001000
		switch (def.opcode) {
			case OPCODE_MOV_RM_TOFROM_REG: {
				bool d = *ip & 0b10;
				bool w = *ip & 0b01;
				++ip;

				uint8_t mod = (*ip & 0b11000000) >> 6;
				uint8_t reg = (*ip & 0b00111000) >> 3;
				uint8_t rm = *ip & 0b00000111;
				uint8_t dest = d ? reg : rm;
				uint8_t src = d ? rm : reg;
				++ip;

				// print("MOD%i,D%u,SRC%u, ", mod, d, src);

				int16_t disp = 0;
				if (mod == 0b10 || !mod && rm==0b110) {
					disp = *(int16_t*)ip;
					ip += 2;
				} else if (mod == 0b01) {
					// print("%i \n", *(int8_t*)ip);
					disp = *(int8_t*)ip;
					++ip;
				}

				// print("mov d=%u, w=%u \n", d, w);
				// print("mov %s, %s \n", regNames[w*8 + dest], regNames[w*8 + src]);
				print("mov ");
				PrintMovMemoryOperand(d ? 0b11 : mod, dest, disp, w);
				print(", ");
				PrintMovMemoryOperand(d ? mod : 0b11, src, disp, w);
				print(" \n");

				// continue;
				// ++ip;
			} break;

			case OPCODE_MOV_IM_TO_RM: {
				++ip;
				uint8_t mod = (*ip & 0b11000000) >> 6;
				uint8_t rm = *ip & 0b00000111;
				++ip;

				int16_t disp = 0;
				if (mod == 0b10 || !mod && rm==0b110) {
					disp = *(int16_t*)ip;
					ip += 2;
				} else if (mod == 0b01) {
					print("%i \n", *(int8_t*)ip);
					disp = *(int8_t*)ip;
					++ip;
				}

				// print("; w %u \n", def.w);

				uint16_t data = *ip;
				++ip;
				if (def.w) {
					data |= *ip << 8;
					++ip;
				}

				print("mov ");
				PrintMovMemoryOperand(mod, rm, disp, def.w);
				print(", ");
				if (mod == 0b11) {
					print("%i", data);
				} else {
					PrintImmediate(data, def.w);
				}
				print(" \n");
			} break;

			case OPCODE_MOV_IM_TO_REG: {
				uint16_t data = *(++ip);
				if (def.w) {
					data |= *(++ip) << 8;
				}

				print("mov ");
				print("%s", regNames[def.w*8 + def.reg]);
				print(", ");
				print("%i", data);
				// PrintImmediate(data, def.w);
				print(" \n");

				++ip;
			} break;

			case OPCODE_MOV_MEM_TO_ACC: {
				++ip;
				uint16_t disp = *(int16_t*)ip;
				ip += 2;

				print("mov ");
				print("ax");
				print(", ");
				PrintMovMemoryOperand(0, 0b110, disp, def.w);
				print(" \n");
			} break;

			case OPCODE_MOV_ACC_TO_MEM: {
				++ip;
				uint16_t disp = *(int16_t*)ip;
				ip += 2;

				print("mov ");
				PrintMovMemoryOperand(0, 0b110, disp, def.w);
				print(", ");
				print("ax");
				print(" \n");
			} break;

			default:
				sys_print_err("error \n");
				exit(1);
		}
	}

	print("\n");
}
