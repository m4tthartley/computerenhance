
#include "sim.h"

// int effAddrClocks

/*
	NOTE:
		This is all awful,
		I just hacked it together for learning purposes
*/

int totalClocks = 0;

int GetTotalClocks()
{
	return totalClocks;
}

int CalcEffAddrClocks(operand_t operand)
{
	if (operand.flags & OPERAND_FLAG_DIRECT_ADDR) {
		return 6;
	}

	int result = 0;

	result += 5; // for base register

	if (operand.flags & OPERAND_FLAG_OFFSET_REGISTER) {
		if ((operand.reg == bp && operand.regOff == di) ||
			(operand.reg == bx && operand.regOff == si)) {
			result += 2;
		} else {
			result += 3;
		}
	}

	if (operand.displacement != 0) {
		result += 4;
	}

	return result;
}

int CalcMovClocks(rawinstruction_t inst)
{
	int clocks = 0;

	int table[][5] = {
		{OPERAND_REG, OPERAND_REG, 2},
		{OPERAND_REG, OPERAND_EFF_ADDR, 8, TRUE, 1},
		{OPERAND_EFF_ADDR, OPERAND_REG, 9, TRUE, 0},

		{OPERAND_REG, OPERAND_IMMEDIATE, 4},
		{OPERAND_EFF_ADDR, OPERAND_IMMEDIATE, 10, TRUE, 0},
	};

	for (int idx=0; idx<array_size(table); ++idx) {
		if (inst.operand0.type == table[idx][0] && inst.operand1.type == table[idx][1]) {
			clocks += table[idx][2];
			if (table[idx][3]) {
				clocks += CalcEffAddrClocks(inst.operands[table[idx][4]]);
			}
			break;
		}
	}

	if (inst.operand0.type == OPERAND_EFF_ADDR) {
		if (CalcMemoryAddress(inst.operand0) & 1) {
			clocks += 4;
		}
	}
	if (inst.operand1.type == OPERAND_EFF_ADDR) {
		if (CalcMemoryAddress(inst.operand1) & 1) {
			clocks += 4;
		}
	}

	totalClocks += clocks;
	return clocks;
}

int CalcAddClocks(rawinstruction_t inst)
{
	int clocks = 0;

	int table[][6] = {
		{OPERAND_REG, OPERAND_REG, 3},
		{OPERAND_REG, OPERAND_EFF_ADDR, 9, TRUE, 1, 1},
		{OPERAND_EFF_ADDR, OPERAND_REG, 16, TRUE, 0, 2},
		{OPERAND_REG, OPERAND_IMMEDIATE, 4},
		{OPERAND_EFF_ADDR, OPERAND_IMMEDIATE, 17, TRUE, 0, 2},
	};

	for (int idx=0; idx<array_size(table); ++idx) {
		if (inst.operand0.type == table[idx][0] && inst.operand1.type == table[idx][1]) {
			clocks += table[idx][2];
			if (table[idx][3]) {
				clocks += CalcEffAddrClocks(inst.operands[table[idx][4]]);
			}
			if (table[idx][5]) {
				// clocks += table[idx][5] * 4;
				if (inst.operand0.type == OPERAND_EFF_ADDR) {
					if (CalcMemoryAddress(inst.operand0) & 1) {
						clocks += 4 * table[idx][5];
					}
				}
				if (inst.operand1.type == OPERAND_EFF_ADDR) {
					if (CalcMemoryAddress(inst.operand1) & 1) {
						clocks += 4 * table[idx][5];
					}
				}
			}
			break;
		}
	}

	totalClocks += clocks;
	return clocks;
}

void DisplayInstructionClocksEstimation(rawinstruction_t inst)
{
	if (inst.op == OP_MOV) {
		print(" ; clocks est: %i", CalcMovClocks(inst));
	}
	else if (inst.op == OP_ADD) {
		print(" ; clocks est: %i", CalcAddClocks(inst));
	}
	else {
		print(" ; clocks est: ???");
	}
}
