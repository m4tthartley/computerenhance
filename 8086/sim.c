//
//  Created by Matt Hartley on 25/08/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#include "shared.h"
#include "sim.h"
#include "decode.h"
#include "disassembly.h"


cpu_t cpu;

void Simulate(data_t file, bool_t printDisassembly)
{
	cpu.instructionData = file.data;

	print("; Disassembly of 8086 binary \n\nbits 16\n\n");

	while (cpu.ip < file.size) {
		rawinstruction_t inst = DecodeInstruction(&cpu);
		if (printDisassembly) {
			DisplayInstruction(inst);
		}
	}

	print("\n; end of file\n\n");
}
