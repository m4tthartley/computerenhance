//
//  Created by Matt Hartley on 22/08/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <core/core.h>
#include "shared.h"


typedef struct {
	enum {
		B_NULL = 0,
		B_PATTERN,
		B_D,
		B_W,
		B_MOD,
		B_REG,
		B_RM,
		B_DISP_LO_IF_MOD,
		B_DISP_HI_IF_MOD,
		B_DATA_LO,
		B_DATA_HI_IF_W,
		B_ADDR_LO,
		B_ADDR_HI,
	} type;
	uint8_t size;
	uint8_t pattern;
} bitchunk_t;

bitchunk_t decodeTable[][16] = {
	// MOV reg/mem to/from register
	{{B_PATTERN, 6, 0b100010}, {B_D}, {B_W}, {B_MOD}, {B_REG}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}},
	// MOV imm to reg/mem
	{{B_PATTERN, 7, 0b1100011}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b000}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}, {B_DATA_LO}, {B_DATA_HI_IF_W}},
	// MOV imm to reg
	{{B_PATTERN, 4, 0b1011}, {B_W}, {B_REG}, {B_DATA_LO}, {B_DATA_HI_IF_W}},
	// MOV mem to acc
	{{B_PATTERN, 7, 0b1010000}, {B_W}, {B_ADDR_LO}, {B_ADDR_HI}},
	// MOV acc to mem
	{{B_PATTERN, 7, 0b1010001}, {B_W}, {B_ADDR_LO}, {B_ADDR_HI}},
};

uint8_t bitChunkSizeTable[] = {
	/* B_NULL */ 0,
	/* B_PATTERN */ 0,
	/* B_D */ 1,
	/* B_W */ 1,
	/* B_MOD */ 2,
	/* B_REG */ 3,
	/* B_RM */ 3,
	/* B_DISP_LO_IF_MOD */ 8,
	/* B_DISP_HI_IF_MOD */ 8,
	/* B_DATA_LO */ 8,
	/* B_DATA_HI_IF_W */ 8,
	/* B_ADDR_LO */ 8,
	/* B_ADDR_HI */ 8,
};

uint8_t fieldOffsetMap[] = {
	/* B_NULL */ 0,
	/* B_PATTERN */ 0,
	/* B_D */ offsetof(rawinstruction_t, d),
	/* B_W */ offsetof(rawinstruction_t, w),
	/* B_MOD */ offsetof(rawinstruction_t, mod),
	/* B_REG */ offsetof(rawinstruction_t, reg),
	/* B_RM */ offsetof(rawinstruction_t, rm),
	/* B_DISP_LO_IF_MOD */ offsetof(rawinstruction_t, disp),
	/* B_DISP_HI_IF_MOD */ offsetof(rawinstruction_t, disp) + 1,
	/* B_DATA_LO */ offsetof(rawinstruction_t, data),
	/* B_DATA_HI_IF_W */ offsetof(rawinstruction_t, data) + 1,
	/* B_ADDR_LO */ offsetof(rawinstruction_t, disp),
	/* B_ADDR_HI */ offsetof(rawinstruction_t, disp) + 1,
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

void DecodeRawInstruction(rawinstruction_t inst)
{
	switch (inst.op) {
		case OPCODE_MOV_RM_TOFROM_REG: {
			
			uint8_t dest = inst.d ? inst.reg : inst.rm;
			uint8_t src = inst.d ? inst.rm : inst.reg;
			
			print("mov ");
			PrintMovMemoryOperand(inst.d ? 0b11 : inst.mod, dest, inst.disp, inst.w);
			print(", ");
			PrintMovMemoryOperand(inst.d ? inst.mod : 0b11, src, inst.disp, inst.w);
			// print(" \n");
		} break;

		case OPCODE_MOV_IM_TO_RM: {
			print("mov ");
			PrintMovMemoryOperand(inst.mod, inst.rm, inst.disp, inst.w);
			print(", ");
			if (inst.mod == 0b11) {
				print("%i", inst.data);
			} else {
				PrintImmediate(inst.data, inst.w);
			}
			// print(" \n");
		} break;

		case OPCODE_MOV_IM_TO_REG: {
			print("mov ");
			print("%s", regNames[inst.w*8 + inst.reg]);
			print(", ");
			print("%i", inst.data);
			// PrintImmediate(data, def.w);
			// print(" \n");
		} break;

		case OPCODE_MOV_MEM_TO_ACC: {
			print("mov ");
			print("ax");
			print(", ");
			PrintMovMemoryOperand(0, 0b110, inst.disp, inst.w);
		} break;

		case OPCODE_MOV_ACC_TO_MEM: {
			print("mov ");
			PrintMovMemoryOperand(0, 0b110, inst.disp, inst.w);
			print(", ");
			print("ax");
		} break;

		default:
			print_err("unknown op \n");
			exit(1);
	}

	print("   ; %s \n", opcodeNames[inst.op]);
}

void DecodeInstruction(uint8_t** ip)
{
	for (int idx=0; idx<array_size(decodeTable); ++idx) {
		rawinstruction_t inst = {0};
		uint32_t bitCursor = 0;
		for (int pat=0; pat<array_size(decodeTable[idx]); ++pat) {
			bitchunk_t chunk = decodeTable[idx][pat];
			if (chunk.type != B_PATTERN) {
				chunk.size = bitChunkSizeTable[chunk.type];
			}
			if (chunk.type == B_NULL) {
				// print("found match \n");
				inst.op = idx;
				assert(!(bitCursor & 7));
				*ip += (bitCursor >> 3);
				DecodeRawInstruction(inst);
				return;
			} // found match?

			uint16_t* bits = (uint16_t*)(*ip + (bitCursor/8));
			uint32_t offset = bitCursor % 8;

			uint16_t mask = (1 << chunk.size) - 1;
			uint16_t value = (*bits >> ((8-chunk.size)-offset)) & mask;

			if (chunk.type == B_DISP_LO_IF_MOD && inst.dispSize < 1) {
				continue;
			}
			if (chunk.type == B_DISP_HI_IF_MOD && inst.dispSize < 2) {
				continue;
			}
			if (chunk.type == B_DATA_HI_IF_W && !inst.w) {
				continue;
			}

			if (chunk.type == B_PATTERN) {
				// print("bitCursor %u, offset %i, chunk.size %i, value %u, pattern %u \n", bitCursor, offset, chunk.size, value, chunk.pattern);
				if (value != chunk.pattern) {
					break;
				}
				bitCursor += chunk.size;
			} else {
				// switch (chunk.type) {
				// 	case B_DISP_LO_IF_MOD: {
				// 		if (inst.mod == 0b01 || inst.mod == 0b10 || (inst.mod == 0b00 && inst.rm == 0b110)) {
							
				// 		}
				// 	} break;

				// 	default:
				// }
				if (chunk.type == B_DISP_LO_IF_MOD && inst.dispSize == 1) { // TODO: dispSize function
					// ((int8_t*)&inst)[fieldOffsetMap[chunk.type]] = *(int8_t*)&value;
					inst.disp = (int8_t)value;
				} 
				// else if (chunk.type == B_DISP_HI_IF_MOD) {
				// 	inst.disp |= (int8_t)value << 8;
				// } 
				else {
					((uint8_t*)&inst)[fieldOffsetMap[chunk.type]] = value;
				}
				bitCursor += chunk.size;
			}

			if (inst.mod == 0b01) {
				inst.dispSize = 1;
			}
			if (inst.mod == 0b10) {
				inst.dispSize = 2;
			}
			if (inst.mod == 0b00 && inst.rm == 0b110) {
				inst.directAddress = TRUE;
				inst.dispSize = 2;
			}
		}
		
		// print("not a match \n");
	}

	print_err("no match found \n");
	*ip += 1;
}

void Decode(data_t file)
{
	print("; Disassembly of 8086 binary \n\nbits 16\n\n");

	uint8_t* ip = file.data;
	while (ip < file.data+file.size) {
		DecodeInstruction(&ip);
	}
}
