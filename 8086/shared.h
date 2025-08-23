//
//  Created by Matt Hartley on 22/08/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#ifndef SHARED_H
#define SHARED_H

#include <core/core.h>


typedef struct {
	uint8_t* data;
	size_t size;
} data_t;

#define al 0b000
#define cl 0b001
#define dl 0b010
#define bl 0b011
#define ah 0b100
#define ch 0b101
#define dh 0b110
#define bh 0b111
#define ax 0b000
#define cx 0b001
#define dx 0b010
#define bx 0b011
#define sp 0b100
#define bp 0b101
#define si 0b110
#define di 0b111

const char* regNames[] = {
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

typedef enum {
	// OPCODE_NULL = 0,

	OPCODE_MOV_RM_TOFROM_REG,
	OPCODE_MOV_IM_TO_RM,
	OPCODE_MOV_IM_TO_REG,
	OPCODE_MOV_MEM_TO_ACC,
	OPCODE_MOV_ACC_TO_MEM,
	OPCODE_MOV_RM_TO_SEG,
	OPCODE_MOV_SEG_TO_RM,
} opcode_t;

const char* opcodeNames[] = {
	// "OPCODE_NULL             ",
	"OPCODE_MOV_RM_TOFROM_REG",
	"OPCODE_MOV_IM_TO_RM     ",
	"OPCODE_MOV_IM_TO_REG    ",
	"OPCODE_MOV_MEM_TO_ACC   ",
	"OPCODE_MOV_ACC_TO_MEM   ",
	"OPCODE_MOV_RM_TO_SEG    ",
	"OPCODE_MOV_SEG_TO_RM    ",
};

#define REG_ENABLED_MASK 0b10000000
enum {
	EFADDR_BASE = (1<<0),
	EFADDR_OFF = (1<<1),
	EFADDR_DISP8 = (1<<2),
	EFADDR_DISP16 = (1<<2),
};
typedef struct {
	uint8_t baseReg;
	uint8_t offReg;
	uint8_t flags;
	uint16_t disp;
} efaddr_t;

efaddr_t effectiveAddressTable[] = {
	{bx, si, EFADDR_BASE|EFADDR_OFF},
	{bx, di, EFADDR_BASE|EFADDR_OFF},
	{bp, si, EFADDR_BASE|EFADDR_OFF},
	{bp, di, EFADDR_BASE|EFADDR_OFF},
	{si,  0, EFADDR_BASE},
	{di,  0, EFADDR_BASE},
	{bp,  0, EFADDR_BASE|EFADDR_DISP16},
	{bx,  0, EFADDR_BASE},
};

typedef enum {
	OP_NULL = 0,
	OP_MOV = 1,
} op_t;

typedef struct {
	opcode_t op;
	uint8_t d;
	uint8_t w;
	uint8_t mod;
	uint8_t reg;
	uint8_t rm;
	int16_t disp;
	uint16_t data;
	uint8_t dispSize;
	bool_t directAddress;
} rawinstruction_t;

typedef struct {
	op_t op;
} instruction_t;


#endif
