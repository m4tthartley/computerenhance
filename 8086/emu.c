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

#include "decode.c"


typedef enum {
	OP_NULL = 0,
	OP_MOV = 1,
} optype_t;

typedef struct {
	optype_t type;
} op_t;

// char* regNamesWide[] = {
	
// };


// void DecodeInstruction(uint8_t** data)
// {

// }

data_t LoadFile(char* filename)
{
	file_t bin = sys_open(filename);
	if (!bin) {
		print_err("Failed to open %s \n", filename);
	}

	data_t result;
	stat_t info = sys_stat(bin);
	// printf("file size: %lu \n\n", info.size);
	result.data = malloc(info.size);
	result.size = info.size;
	sys_read(bin, 0, result.data, info.size);
	sys_close(bin);

	return result;
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		print_err("Filename argument required \n");
		exit(1);
	}

	char* filename = argv[1];

	// data_t file = LoadFile("./data/listing_0037_single_register_mov");
	// data_t file = LoadFile("./data/listing_0038_many_register_mov");
	data_t file = LoadFile(filename);

	DecodeStream(file);

	return 0;
}
