//
//  Created by Matt Hartley on 26/08/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#ifndef INSTRUCTION_TABLE_H
#define INSTRUCTION_TABLE_H

#include <core/core.h>


typedef enum {
	OP_NULL = 0,
	// OP_MOV = 1,
#	define I(mnemonic, ...) OP_##mnemonic,
#	define SKIP_ALT
#	include "instruction_table.inc.h"
#	undef SKIP_ALT
#	undef I
} op_t;

char* opMnemonicStrings[] = {
	(char*)"NULL",
#	define I(mnemonic, ...) #mnemonic,
#	define SKIP_ALT
#	include "instruction_table.inc.h"
#	undef SKIP_ALT
#	undef I
};

typedef enum {
	BITS_END = 0,
	BITS_TYPE,
	BITS_FORMAT,
	BITS_PATTERN,
	BITS_D,
	BITS_S,
	BITS_W,
	BITS_V,
	BITS_Z,
	BITS_MOD,
	BITS_REG,
	BITS_RM,
	BITS_SR,

	// BITS_DISP_LO_IF_MOD, // this one gets sign extended
	// BITS_DISP_HI_IF_MOD,
	BITS_DISP,
	BITS_DISP_IS_WIDE,

	BITS_DATA,
	BITS_DATA_HI,
	BITS_DATA_W_IF_W,
	// BITS_DATA_HI_IF_SW_01,
	BITS_ADDR,
	BITS_ADDR_LO,
	BITS_ADDR_HI,
	BITS_INC,
	BITS_INC8,
	BITS_IP,
	BITS_CS,
	BITS_IP_LO,
	BITS_IP_HI,
	BITS_IP_INC_LO,
	BITS_IP_INC_HI,

	BITS_ADDR_SEG,

	BITS_DOUBLE_WIDE,

	BITS_COUNT,
} bitchunktype_t;

typedef struct {
	bitchunktype_t type;
	uint8_t size;
	uint8_t value;
	// void* d;
} bitchunk_t;

enum {
	FLAG_CARRY		= (1<<0),

	FLAG_PARITY		= (1<<2),

	FLAG_AUX_CARRY	= (1<<4),

	FLAG_ZERO		= (1<<6),
	FLAG_SIGN		= (1<<7),
	FLAG_TRAP		= (1<<8),
	FLAG_INT_ENABLE	= (1<<9),
	FLAG_DIRECTION	= (1<<10),
	FLAG_OVERFLOW	= (1<<11),
	
	FLAG_COUNT		= 12,
} cpuflag_t;

char* cpuFlagNames[] = {
	(char*)"CF",
	(char*)"  ",
	(char*)"PF",
	(char*)"  ",
	(char*)"AF",
	(char*)"  ",
	(char*)"ZF",
	(char*)"SF",
	(char*)"TF",
	(char*)"IF",
	(char*)"DF",
	(char*)"OF",
};

enum {
	CF	= 0,
	PF	= 2,
	AF	= 4,
	ZF	= 6,
	SF	= 7,
	TF	= 8,
	IF	= 9,
	DF	= 10,
	OF	= 11,
};

enum {
	// FLAGLOGIC_NONE = 0,
	FLAGLOGIC_0 = 0,
	FLAGLOGIC_1 = 1,
	FLAGLOGIC_X = 2,
};

typedef struct {
	bool_t enabled;
	// uint8_t flag;
	uint8_t logic;
} flaglogic_t;

// typedef struct {
// 	flaglogic_t flags[FLAG_COUNT];
// } instructionflags_t;

#define DECODE_FORMAT_CHUNKS 16
typedef struct {
	op_t op;
	bitchunk_t chunks[DECODE_FORMAT_CHUNKS];
} decodeformat_t;

decodeformat_t decodeTable[] = {
	#define I(mnemonic, ...) {OP_##mnemonic, {__VA_ARGS__}},
	#include "instruction_table.inc.h"
	#undef I
};

flaglogic_t instructionFlags[][FLAG_COUNT] = {
#	define Flags(mnemonic, ...) [OP_##mnemonic] = { __VA_ARGS__ },
#	define SKIP_ALT
#	include "instruction_table.inc.h"
#	undef SKIP_ALT
#	undef Flags
};

#define HALT_OPCODE 0b11110100


#endif
