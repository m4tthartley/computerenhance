//
//  Created by Matt Hartley on 27/08/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#ifndef DISASSEMBLY_H
#define DISASSEMBLY_H

#include "sim.h"


char* regNames[] = {
	// "al",
	// "cl",
	// "dl",
	// "bl",
	// "ah",
	// "ch",
	// "dh",
	// "bh",

	(char*)"ax",
	(char*)"cx",
	(char*)"dx",
	(char*)"bx",
	(char*)"sp",
	(char*)"bp",
	(char*)"si",
	(char*)"di",
	(char*)"es",
	(char*)"cs",
	(char*)"ss",
	(char*)"ds",
};

const char* segRegNames[] = {
	"es",
	"cs",
	"ss",
	"ds",
};

void DisplayInstruction(rawinstruction_t inst);


#endif
