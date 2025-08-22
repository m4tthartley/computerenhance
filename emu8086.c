//
//  Created by Matt Hartley on 21/08/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

/*
	Computer Enhance Part 1: Reading ASM
	This part of the course creates an
	8086 decoder and emulator in order to
	teach assembly language.
*/

#include <stdio.h>
#include <stdlib.h>

#define CORE_IMPL
#include <core/sys.h>
#include <core/core.h>


enum {
	OP_MOV = 0b10001000,
} opcode_t;

char* regNames[] = {
	"al",
	"cl",
	"dl",
	"bl",
	"ah",
	"ch",
	"dh",
	"bh",

	"ax",
	"cx",
	"dx",
	"bx",
	"sp",
	"bp",
	"si",
	"di",
};
// char* regNamesWide[] = {
	
// };


// void DecodeInstruction(uint8_t** data)
// {

// }

typedef struct {
	uint8_t* data;
	size_t size;
} data_t;

data_t LoadFile(char* filename)
{
	file_t bin = sys_open(filename);
	if (!bin) {
		print_err("Failed to open %s \n", filename);
	}

	data_t result;
	stat_t info = sys_stat(bin);
	printf("file size: %lu \n\n", info.size);
	result.data = malloc(info.size);
	result.size = info.size;
	sys_read(bin, 0, result.data, info.size);
	sys_close(bin);

	return result;
}

int main()
{
	// data_t file = LoadFile("./data/listing_0037_single_register_mov");
	data_t file = LoadFile("./data/listing_0038_many_register_mov");

	uint8_t* ip = file.data;
	while (ip < file.data+file.size) {
		if ((*ip & 0b11111100) == OP_MOV) {
			bool d = *ip & 0b10;
			bool w = *ip & 0b01;
			++ip;

			uint8_t mod = (*ip & 0b11000000) >> 6;
			uint8_t reg = (*ip & 0b00111000) >> 3;
			uint8_t rm = *ip & 0b00000111;
			uint8_t dest = d ? reg : rm;
			uint8_t src = d ? rm : reg;
			++ip;

			// print("mov d=%u, w=%u \n", d, w);
			print("mov %s, %s \n", regNames[w*8 + dest], regNames[w*8 + src]);

			continue;
		}

		sys_print_err("error \n");
	}


	return 0;
}
