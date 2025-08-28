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

void SimInstruction(rawinstruction_t inst)
{

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

	cpu.ax.word = 3;
	cpu.bx.word = 1024;

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

	print("\n; end of file\n\n");
}
