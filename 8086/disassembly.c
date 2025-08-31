//
//  Created by Matt Hartley on 25/08/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include <core/core.h>
#include "sim.h"
#include "decode.h"
#include "disassembly.h"


void DisplayOperand(rawinstruction_t inst, operand_t operand)
{
	switch (operand.type) {
		case OPERAND_REG: {
			char* reg = strcopy(regNames[operand.reg]);
			if (operand.reg < 4) {
				if (!(operand.flags & OPERAND_FLAG_WIDE)) {
					reg[1] = operand.flags & OPERAND_FLAG_HIGH ? 'h' : 'l';
				}
			}

			print("%s", reg);
		} break;

		case OPERAND_EFF_ADDR: {
			print("[");
			if (operand.flags & OPERAND_FLAG_DIRECT_ADDR) {
				print("%u", operand.address);
			} else {
				print("%s", regNames[operand.reg]);
				if (operand.flags & OPERAND_FLAG_OFFSET_REGISTER) {
					print(" + %s", regNames[operand.regOff]);
				}
				if (operand.displacement) {
					print(" %c %i", operand.displacement<0 ? '-' : '+', abs(operand.displacement));
				}
			}
			print("]");
		} break;

		case OPERAND_IMMEDIATE: {
			if (operand.flags & OPERAND_FLAG_SIZE_SPECIFIER) {
				if (operand.flags & OPERAND_FLAG_WIDE) {
					print("word ");
				} else {
					print("byte ");
				}
			}
			if (operand.flags & OPERAND_FLAG_SIGNED) {
				print("%i", (int16_t)operand.data);
			} else {
				print("%u", operand.data);
			}
		} break;

		case OPERAND_INCREMENT: {
			print("$");
			if (operand.displacement > -1) {
				print("+");
			}
			print("%i", operand.displacement+2);
		} break;

		default:
			print("unknown ");
	}
}

void DisplayInstruction(rawinstruction_t inst)
{
	char* mnemonic = strcopy(opMnemonicStrings[inst.op]);
	// strlower(mnemonic);
	print("%s ", mnemonic);

	if (inst.operand0.type) {
		DisplayOperand(inst, inst.operand0);
	}
	if (inst.operand1.type) {
		if (inst.operand0.type) {
			print(", ");
		}
		DisplayOperand(inst, inst.operand1);
	}

	// print("\n");
}
