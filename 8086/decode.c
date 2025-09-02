//
//  Created by Matt Hartley on 22/08/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <core/core.h>
#include "instruction_table.h"
#include "shared.h"
#include "sim.h"
#include "decode.h"


extern uint8_t memory[];


// uint8_t bitChunkSizeTable[] = {
// 	// [B_NULL] = 0,
// 	[B_PATTERN] = 0,
// 	[B_D] = 1,
// 	[B_S] = 1,
// 	[B_W] = 1,
// 	[B_V] = 1,
// 	[B_Z] = 1,
// 	[B_MOD] = 2,
// 	[B_REG] = 3,
// 	[B_RM] = 3,
// 	[B_SR] = 2,
// 	// [B_DISP_LO] = 8,
// 	// [B_DISP_HI] = 8,
// 	[B_DISP_LO_IF_MOD] = 8,
// 	[B_DISP_HI_IF_MOD] = 8,
// 	[B_DATA_LO] = 8,
// 	[B_DATA_HI] = 8,
// 	[B_DATA_HI_IF_W] = 8,
// 	[B_DATA_HI_IF_SW_01] = 8,
// 	[B_ADDR_LO] = 8,
// 	[B_ADDR_HI] = 8,
// 	// [B_ADDR_LO_IF_MOD] = 8,
// 	// [B_ADDR_HI_IF_MOD] = 8,
// 	[B_INC8] = 8,
// 	[B_IP_LO] = 8,
// 	[B_IP_HI] = 8,
// 	[B_IP_INC_LO] = 8,
// 	[B_IP_INC_HI] = 8,
// };

// uint8_t fieldOffsetMap[] = {
// 	// [B_NULL] = 0,
// 	[B_PATTERN] = 0,
// 	[B_D] = offsetof(rawinstruction_t, d),
// 	[B_S] = offsetof(rawinstruction_t, s),
// 	[B_W] = offsetof(rawinstruction_t, w),
// 	[B_V] = offsetof(rawinstruction_t, v),
// 	[B_Z] = offsetof(rawinstruction_t, z),
// 	[B_MOD] = offsetof(rawinstruction_t, mod),
// 	[B_REG] = offsetof(rawinstruction_t, reg),
// 	[B_RM] = offsetof(rawinstruction_t, rm),
// 	[B_SR] = offsetof(rawinstruction_t, reg),
// 	// [B_DISP_LO] = offsetof(rawinstruction_t, disp),
// 	// [B_DISP_HI] = offsetof(rawinstruction_t, disp) + 1,
// 	[B_DISP_LO_IF_MOD] = offsetof(rawinstruction_t, disp),
// 	[B_DISP_HI_IF_MOD] = offsetof(rawinstruction_t, disp) + 1,
// 	[B_DATA_LO] = offsetof(rawinstruction_t, data),
// 	[B_DATA_HI] = offsetof(rawinstruction_t, data) + 1,
// 	[B_DATA_HI_IF_W] = offsetof(rawinstruction_t, data) + 1,
// 	[B_DATA_HI_IF_SW_01] = offsetof(rawinstruction_t, data) + 1,
// 	[B_ADDR_LO] = offsetof(rawinstruction_t, address),
// 	[B_ADDR_HI] = offsetof(rawinstruction_t, address) + 1,
// 	// [B_ADDR_LO_IF_MOD] = offsetof(rawinstruction_t, address),
// 	// [B_ADDR_HI_IF_MOD] = offsetof(rawinstruction_t, address) + 1,
// 	[B_INC8] = offsetof(rawinstruction_t, disp),
// 	[B_IP_LO] = offsetof(rawinstruction_t, address),
// 	[B_IP_HI] = offsetof(rawinstruction_t, address) + 1,
// 	[B_IP_INC_LO] = offsetof(rawinstruction_t, disp),
// 	[B_IP_INC_HI] = offsetof(rawinstruction_t, disp) + 1,
// };

bool_t segmentOverride;
uint8_t segment;

// uint8_t* instructionData;
// uint16_t ip;

// uint32_t DispSizeForMod(rawinstruction_t inst)
// {
// 	if (inst.mod == 0b01) {
// 		return 1;
// 	}
// 	if (inst.mod == 0b10) {
// 		return 2;
// 	}
// 	if (inst.mod == 0b00 && inst.rm == 0b110) {
// 		return 2;
// 	}

// 	return 0;
// }

// void PrintInstructionRM(rawinstruction_t inst, bool_t sizeSpecifier)
// {
// 	if (inst.mod == 0b11) {
// 		print("%s", regNames[inst.w*8 + inst.rm]);
// 	} else {
// 		if (sizeSpecifier && !inst.skipRmSizeSpecifier) {
// 			if (inst.w) {
// 				print("word ");
// 			} else {
// 				print("byte ");
// 			}
// 		}

// 		efaddr_t addr = effectiveAddressTable[inst.rm];
// 		// if (!mod && rm==0b110) {
// 		// 	addr.flags &= ~EFADDR_BASE;
// 		// }

// 		if (segmentOverride) {
// 			print("%s:", segRegNames[segment]);
// 			segmentOverride = FALSE;
// 		}

// 		print("[");
		
// 		if (inst.mod == 0b00 && inst.rm == 0b110) {
// 			print("%u", inst.address);
// 		} else {
// 			if (addr.flags & EFADDR_BASE) {
// 				print("%s", regNames[8 + addr.baseReg]);
// 			}
// 			if (addr.flags & EFADDR_OFF) {
// 				print(" + %s", regNames[8 + addr.offReg]);
// 			}

// 			if (inst.disp && (addr.flags & EFADDR_DISP16 || inst.mod == 0b01 || inst.mod == 0b10)) {
// 				char* sign = inst.disp < 0 ? "-" : "+";
// 				if (addr.flags & EFADDR_BASE) {
// 					print(" %s ", sign);
// 				}
// 				print("%i", abs(inst.disp));
// 			}
// 		}

// 		print("]");
// 	}

// }

// void PrintImmediate(uint16_t data, bool_t w)
// {
// 	if (w) {
// 		print("word %u", data);
// 	} else {
// 		print("byte %u", data);
// 	}
// }

// void OutputRawInstruction(rawinstruction_t inst)
// {
// 	// switch (inst.op) {
// 	// 	case OPCODE_MOV_RM_TOFROM_REG: {
			
// 	// 		uint8_t dest = inst.d ? inst.reg : inst.rm;
// 	// 		uint8_t src = inst.d ? inst.rm : inst.reg;
			
// 	// 		// print("mov ");
// 	// 		print("%s ", inst.type);
// 	// 		PrintMovMemoryOperand(inst.d ? 0b11 : inst.mod, dest, inst.disp, inst.w);
// 	// 		print(", ");
// 	// 		PrintMovMemoryOperand(inst.d ? inst.mod : 0b11, src, inst.disp, inst.w);
// 	// 		// print(" \n");
// 	// 	} break;

// 	// 	case OPCODE_MOV_IM_TO_RM: {
// 	// 		print("mov ");
// 	// 		PrintMovMemoryOperand(inst.mod, inst.rm, inst.disp, inst.w);
// 	// 		print(", ");
// 	// 		if (inst.mod == 0b11) {
// 	// 			print("%i", inst.data);
// 	// 		} else {
// 	// 			PrintImmediate(inst.data, inst.w);
// 	// 		}
// 	// 		// print(" \n");
// 	// 	} break;

// 	// 	case OPCODE_MOV_IM_TO_REG: {
// 	// 		print("mov ");
// 	// 		print("%s", regNames[inst.w*8 + inst.reg]);
// 	// 		print(", ");
// 	// 		print("%i", inst.data);
// 	// 		// PrintImmediate(data, def.w);
// 	// 		// print(" \n");
// 	// 	} break;

// 	// 	case OPCODE_MOV_MEM_TO_ACC: {
// 	// 		print("mov ");
// 	// 		print("ax");
// 	// 		print(", ");
// 	// 		PrintMovMemoryOperand(0, 0b110, inst.disp, inst.w);
// 	// 	} break;

// 	// 	case OPCODE_MOV_ACC_TO_MEM: {
// 	// 		print("mov ");
// 	// 		PrintMovMemoryOperand(0, 0b110, inst.disp, inst.w);
// 	// 		print(", ");
// 	// 		print("ax");
// 	// 	} break;

// 	// 	default:
// 	// 		print_err("unknown op \n");
// 	// 		exit(1);
// 	// }

// 	bool_t newline = TRUE;

// 	switch (inst.format) {
// 		case OPFORMAT_NONE: {
// 			print("%s ", inst.type);
// 		} break;

// 		case OPFORMAT_REG_RM: {
			
// 			uint8_t dest = inst.d ? inst.rm : inst.reg;
// 			uint8_t src = inst.d ? inst.reg : inst.rm;
			
// 			// print("mov ");
// 			print("%s ", inst.type);
// 			if (inst.d) {
// 				PrintInstructionRM(inst, FALSE);
// 			} else {
// 				print("%s", regNames[inst.w*8 + inst.reg]);
// 			}
// 			print(", ");
// 			if (inst.d) {
// 				print("%s", regNames[inst.w*8 + inst.reg]);
// 			} else {
// 				PrintInstructionRM(inst, FALSE);
// 			}
// 			// print(" \n");
// 		} break;

// 		case OPFORMAT_RM_IM: {
// 			// print("mov ");
// 			print("%s ", inst.type);
// 			PrintInstructionRM(inst, FALSE);
// 			print(", ");
// 			if (inst.mod == 0b11) {
// 				print("%i", inst.data);
// 			} else {
// 				PrintImmediate(inst.data, inst.w);
// 			}
// 			// print(" \n");
// 		} break;

// 		case OPFORMAT_REG_IM: {
// 			// print("mov ");
// 			print("%s ", inst.type);
// 			print("%s", regNames[inst.w*8 + inst.reg]);
// 			print(", ");
// 			print("%i", inst.data);
// 			// PrintImmediate(data, def.w);
// 			// print(" \n");
// 		} break;

// 		case OPFORMAT_IM_REG: {
// 			// print("mov ");
// 			print("%s ", inst.type);
// 			print("%i", inst.data);
// 			print(", ");
// 			print("%s", regNames[inst.w*8 + inst.reg]);
// 			// PrintImmediate(data, def.w);
// 			// print(" \n");
// 		} break;

// 		case OPFORMAT_SR_RM: {
// 			print("%s ", inst.type);
// 			print("%s", segRegNames[inst.reg]);
// 			print(", ");
// 			PrintInstructionRM(inst, FALSE);
// 		} break;

// 		case OPFORMAT_RM_SR: {
// 			print("%s ", inst.type);
// 			PrintInstructionRM(inst, FALSE);
// 			print(", ");
// 			print("%s", segRegNames[inst.reg]);
// 		} break;

// 		case OPFORMAT_IM: {
// 			print("%s ", inst.type);
// 			print("%i", inst.data);
// 		} break;

// 		case OPFORMAT_REG_REG16: {
// 			print("%s ", inst.type);
// 			print("%s", regNames[inst.w*8 + inst.reg]);
// 			print(", ");
// 			print("%s", regNames[8 + inst.rm]);
// 		} break;

// 		case OPFORMAT_REG16_REG: {
// 			print("%s ", inst.type);
// 			print("%s", regNames[8 + inst.rm]);
// 			print(", ");
// 			print("%s", regNames[inst.w*8 + inst.reg]);
// 		} break;

// 		case OPFORMAT_INC: {
// 			print("%s ", inst.type);
// 			print("$+2%c%i", inst.disp < 0 ? '-' : '+', abs(inst.disp));
// 		} break;

// 		case OPFORMAT_IP_INC: {
// 			// print("%u\n", inst.disp & 0xFFFF);
// 			print("%s ", inst.type);
// 			print("%i", inst.disp + ip);
// 		} break;

// 		case OPFORMAT_RM: {
// 			print("%s ", inst.type);
// 			PrintInstructionRM(inst, TRUE);
// 		} break;

// 		case OPFORMAT_REG: {
// 			print("%s ", inst.type);
// 			print("%s", regNames[inst.w*8 + inst.reg]);
// 		} break;

// 		case OPFORMAT_SR: {
// 			print("%s ", inst.type);
// 			print("%s", segRegNames[inst.reg]);
// 		} break;

// 		case OPFORMAT_SHIFT: {
// 			print("%s ", inst.type);
// 			PrintInstructionRM(inst, TRUE);
// 			print(", ");
// 			if (inst.v) {
// 				print("cl");
// 			} else {
// 				print("1");
// 			}
// 		} break;

// 		case OPFORMAT_PREFIX: {
// 			newline = FALSE;
// 			print("%s ", inst.type);
// 		} break;

// 		case OPFORMAT_SEGMENT: {
// 			newline = FALSE;
// 			segment = inst.reg;
// 			segmentOverride = TRUE;
// 		} break;

// 		case OPFORMAT_IP_CS: {
// 			print("%s ", inst.type);
// 			print("%u", inst.data);
// 			print(":");
// 			print("%u", inst.address);
// 		} break;

// 		default:
// 			print_err("unknown op \n");
// 			exit(1);
// 	}

// 	// print("   ; %s \n", opcodeNames[inst.op]);
// 	if (newline) {
// 		print("\n");
// 	}
// }

operand_t CreateRegOperand(uint8_t reg, bool_t wide)
{
	operand_t operand = {0};
	operand.type = OPERAND_REG;

	if (wide) {
		operand.reg = reg;
		operand.flags |= OPERAND_FLAG_WIDE;
	} else {
		operand.reg = reg % 4;
		if (reg > 3) {
			operand.flags |= OPERAND_FLAG_HIGH;
		}
	}

	return operand;
}

rawinstruction_t TryDecodeInstructionFormat(cpu_t* cpu, decodeformat_t format)
{
	rawinstruction_t inst = {0};
	uint8_t bits[BITS_COUNT] = {0};
	bool_t hasBits[BITS_COUNT] = {0};

	bool_t match = TRUE;
	uint32_t bitCursor = 0;
	int chunkIndex = 0;
	while (format.chunks[chunkIndex].type != BITS_END) {
		assert(chunkIndex < DECODE_FORMAT_CHUNKS);
		bitchunk_t chunk = format.chunks[chunkIndex++];

		// if (chunk.type != B_PATTERN) {
		// 	chunk.size = bitChunkSizeTable[chunk.type];
		// }

		bool_t directAddress = hasBits[BITS_MOD] && bits[BITS_MOD] == 0b00 && bits[BITS_RM] == 0b110;
		hasBits[chunk.type] = TRUE;
		
		if (chunk.size) {
			uint16_t* data = (uint16_t*)(memory + (cpu->ip + bitCursor/8));
			uint32_t offset = bitCursor % 8;
			uint16_t mask = (1 << chunk.size) - 1;
			uint16_t value = (*data >> ((8-chunk.size)-offset)) & mask;

			bits[chunk.type] = value;

			if (chunk.type == BITS_PATTERN) {
				if (value != chunk.value) {
					match = FALSE;
					break;
				}
			}
			bitCursor += chunk.size;
		} else {
			bits[chunk.type] = chunk.value;
		}

		// if (chunk.type == B_DISP_LO_IF_MOD && DispSizeForMod(inst) < 1) {
		// 	continue;
		// }
		// if (chunk.type == B_DISP_HI_IF_MOD && DispSizeForMod(inst) < 2) {
		// 	continue;
		// }
		// if (chunk.type == B_DATA_HI_IF_W) {
		// 	if (!inst.w) {
		// 		continue;
		// 	}
		// }
		// if (chunk.type == B_DATA_HI_IF_SW_01) {
		// 	if (inst.s || !inst.w) {
		// 		continue;
		// 	}
		// }

		// if (chunk.type == B_ADDR_LO_IF_MOD && !directAddress) {
		// 	continue;
		// }
		// if (chunk.type == B_ADDR_HI_IF_MOD && !directAddress) {
		// 	continue;
		// }

		// if (chunk.type == BITS_PATTERN) {
		// 	if (value != chunk.pattern) {
		// 		match = FALSE;
		// 		break;
		// 	}
		// 	bitCursor += chunk.size;
		// } else {
			// switch (chunk.type) {
			// 	case B_DISP_LO_IF_MOD: {
			// 		if (inst.mod == 0b01 || inst.mod == 0b10 || (inst.mod == 0b00 && inst.rm == 0b110)) {
						
			// 		}
			// 	} break;

			// 	default:
			// }

			// if (directAddress) {
			// 	if (chunk.type == B_DISP_LO_IF_MOD) chunk.type = B_ADDR_LO;
			// 	if (chunk.type == B_DISP_HI_IF_MOD) chunk.type = B_ADDR_HI;
			// }

			// bool_t signExtend = (chunk.type == B_DISP_LO_IF_MOD && DispSizeForMod(inst) == 1) || chunk.type == B_INC8;

			// if (signExtend) {
			// 	inst.disp = (int8_t)value;
			// } 
			// else if (chunk.type == B_D) {
			// 	inst.d = !value;
			// }
			// else {
			// 	((uint8_t*)&inst)[fieldOffsetMap[chunk.type]] = value;
			// }

		// 	bitCursor += chunk.size;
		// }

		// if (inst.mod == 0b01) {
		// 	inst.dispSize = 1;
		// }
		// if (inst.mod == 0b10) {
		// 	inst.dispSize = 2;
		// }
		// if (inst.mod == 0b00 && inst.rm == 0b110) {
		// 	inst.directAddress = TRUE;
		// 	inst.dispSize = 2;
		// }
	}

	if (match) {
		assert(!(bitCursor & 7));
		inst.op = format.op;
		inst.size = (bitCursor >> 3);
		
		operand_t regOperand = {0};
		operand_t modOperand = {0};

		uint8_t reg = bits[BITS_REG];
		uint8_t mod = bits[BITS_MOD];
		uint8_t rm = bits[BITS_RM];
		// uint8_t dispSize = 0;
		// bool_t disp = FALSE;
		// bool_t dispIsWide = FALSE;
		bool_t wide = bits[BITS_W];
		bool_t dataIsWide = hasBits[BITS_DATA_W_IF_W] && !bits[BITS_S] && bits[BITS_W];
		bool_t directAddress = mod == 0 && rm == 0b110;

		if (hasBits[BITS_REG]) {
			regOperand = CreateRegOperand(reg, wide);
		}

		if (hasBits[BITS_SR]) {
			regOperand = CreateRegOperand(8 + bits[BITS_SR], wide);
		}

		if (hasBits[BITS_MOD]) {
			if (mod == 0b11) {
				// register mode
				modOperand = CreateRegOperand(rm, wide);
			} else {
				// effective address mode
				modOperand.type = OPERAND_EFF_ADDR;

				// uint8_t effAddrTable[] = {
				// 	0b10001000 | bx | (si<<4),
				// 	0b10001000 | bx | (di<<4),
				// 	0b10001000 | bp | (si<<4),
				// 	0b10001000 | bp | (di<<4),
				// 	0b00001000 | si,
				// 	0b00001000 | di,
				// 	0b00001000 | bp,
				// 	0b00001000 | bx,
				// 	// {bx, si},
				// 	// {bx, di},
				// 	// {bp, si},
				// 	// {bp, di},
				// 	// {si, 0},
				// 	// {di, 0},
				// 	// {bp, 0},
				// 	// {bx, 0},
				// };

				uint8_t effAddrBaseRegisters[] = {
					bx, bx, bp, bp, si, di, bp, bx,
				};
				uint8_t effAddrOffsetRegisters[] = {
					si, di, si, di,
				};
				uint8_t segmentRegisterBase[] = {
					ds, ds, ss, ss, ds, ds, ss, ds,
				};

				if (directAddress) {
					// direct address
					hasBits[BITS_DISP] = TRUE;
					hasBits[BITS_DISP_IS_WIDE] = TRUE;
					modOperand.flags |= OPERAND_FLAG_DIRECT_ADDR;
				} else {
					// modOperand.reg = effAddrTable[rm];

					modOperand.reg = effAddrBaseRegisters[rm];
					if (rm < 4) {
						modOperand.regOff = effAddrOffsetRegisters[rm];
						modOperand.flags |= OPERAND_FLAG_OFFSET_REGISTER;
					}
				}

				if (mod) {
					hasBits[BITS_DISP] = TRUE;
				}

				// if (mod == 0b01) {
				// 	dispSize = 1;
				// }
				if (mod == 0b10) {
					hasBits[BITS_DISP_IS_WIDE] = TRUE;
				}

				if (hasBits[BITS_ADDR_SEG]) {
					modOperand.segreg = bits[BITS_ADDR_SEG];
				} else {
					if (directAddress) {
						modOperand.segreg = ds;
					} else {
						modOperand.segreg = segmentRegisterBase[rm];
					}
				}
			}
		}

		operand_t* operand0 = bits[BITS_D] ? &regOperand : &modOperand;
		operand_t* operand1 = bits[BITS_D] ? &modOperand : &regOperand;

		if (hasBits[BITS_DISP]) {
			if (hasBits[BITS_DISP_IS_WIDE]) {
				modOperand.displacement = *(int16_t*)(memory + (cpu->ip + inst.size));
			} else {
				modOperand.displacement = *(int8_t*)(memory + (cpu->ip + inst.size));
			}

			inst.size += 1 + hasBits[BITS_DISP_IS_WIDE];
		}

		if (hasBits[BITS_DATA]) {
			// if (regOperand.type) {
				operand1->type = OPERAND_IMMEDIATE;
				if (bits[BITS_S]) {
					operand1->flags |= OPERAND_FLAG_SIGNED;
				}

				if (dataIsWide) {
					operand1->flags |= OPERAND_FLAG_WIDE;
					operand1->data = *(uint16_t*)(memory + (cpu->ip + inst.size));
				} else {
					if (bits[BITS_S]) {
						operand1->data = *(int8_t*)(memory + (cpu->ip + inst.size));
					} else {
						operand1->data = *(uint8_t*)(memory + (cpu->ip + inst.size));
					}
				}
			// } else {
			// 	regOperand.type = OPERAND_IMMEDIATE;

			// 	if (dataIsWide) {
			// 		regOperand.data = *(uint16_t*)(instructionData + (ip + inst.size));
			// 	} else {
			// 		regOperand.data = *(uint8_t*)(instructionData + (ip + inst.size));
			// 	}
			// }

			if (hasBits[BITS_INC]) {
				operand1->type = OPERAND_INCREMENT;
			}

			inst.size += 1 + dataIsWide;
		}

		if (operand1->type == OPERAND_IMMEDIATE && operand0->type != OPERAND_REG) {
			operand1->flags |= OPERAND_FLAG_SIZE_SPECIFIER;
		}

		// if (bits[BITS_D]) {
		// 	inst.operand0 = regOperand;
		// 	inst.operand1 = modOperand;
		// } else {
		// 	inst.operand1 = regOperand;
		// 	inst.operand0 = modOperand;
		// }
		inst.operand0 = *operand0;
		inst.operand1 = *operand1;
		inst.wide = wide;

		if (bits[BITS_S]) {
			inst.operand0.flags |= OPERAND_FLAG_SIGNED;
			inst.operand1.flags |= OPERAND_FLAG_SIGNED;
		}
		if (wide) {
			inst.operand0.flags |= OPERAND_FLAG_WIDE;
			inst.operand1.flags |= OPERAND_FLAG_WIDE;
		}
	}

	return inst;
}

rawinstruction_t DecodeInstruction(cpu_t* cpu)
{
	for (int idx=0; idx<array_size(decodeTable); ++idx) {
		if (!decodeTable[idx].op) {
			continue;
		}

		rawinstruction_t inst = TryDecodeInstructionFormat(cpu, decodeTable[idx]);
		if (inst.op) {
			return inst;
		}
		// return inst;
	}

	// print_err("no match found \n");
	// exit(1);
	return (rawinstruction_t){0};
}

// void Decode(data_t file)
// {
	
// }
