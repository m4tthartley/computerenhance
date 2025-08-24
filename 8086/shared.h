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

#define es 0b00
#define cs 0b01
#define ss 0b10
#define ds 0b11

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

const char* segRegNames[] = {
	"es",
	"cs",
	"ss",
	"ds",
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

	OPCODE_PUSH_RM,
	OPCODE_PUSH_REG,
	OPCODE_PUSH_SR,

	OPCODE_POP_RM,
	OPCODE_POP_REG,
	OPCODE_POP_SR,

	OPCODE_XCHG_REG_RM,
	OPCODE_XCHG_ACC_REG,

	OPCODE_IN_FIX,
	OPCODE_IN_VAR,

	OPCODE_OUT_FIX,
	OPCODE_OUT_VAR,

	OPCODE_XLAT,

	OPCODE_LEA,
	OPCODE_LDS,
	OPCODE_LES,
	OPCODE_LAHF,
	OPCODE_SAHF,
	OPCODE_PUSHF,
	OPCODE_POPF,

	OPCODE_ADD_REG_RM,
	OPCODE_ADD_RM_IM,
	OPCODE_ADD_ACC_IM,

	OPCODE_ADC_REG_RM,
	OPCODE_ADC_RM_IM,
	OPCODE_ADC_ACC_IM,

	OPCODE_INC_RM,
	OPCODE_INC_REG,
	OPCODE_INC_AAA,
	OPCODE_INC_DAA,

	OPCODE_SUB_REG_RM,
	OPCODE_SUB_RM_IM,
	OPCODE_SUB_ACC_IM,

	OPCODE_SBB_REG_RM,
	OPCODE_SBB_RM_IM,
	OPCODE_SBB_ACC_IM,

	OPCODE_DEC_RM,
	OPCODE_DEC_REG,
	OPCODE_NEG,

	OPCODE_CMP_REG_RM,
	OPCODE_CMP_RM_IM,
	OPCODE_CMP_ACC_IM,

	OPCODE_AAS,
	OPCODE_DAS,

	OPCODE_MUL,
	OPCODE_IMUL,
	OPCODE_AAM,

	OPCODE_DIV,
	OPCODE_IDIV,
	OPCODE_AAD,

	OPCODE_CBW,
	OPCODE_CWD,

	OPCODE_NOT,
	OPCODE_SHL,
	OPCODE_SHR,
	OPCODE_SAR,
	OPCODE_ROL,
	OPCODE_ROR,
	OPCODE_RCL,
	OPCODE_RCR,

	OPCODE_AND_REG_RM,
	OPCODE_AND_RM_IM,
	OPCODE_AND_ACC_IM,

	OPCODE_TEST_REG_RM,
	OPCODE_TEST_RM_IM,
	OPCODE_TEST_ACC_IM,
	
	OPCODE_OR_REG_RM,
	OPCODE_OR_RM_IM,
	OPCODE_OR_ACC_IM,

	OPCODE_XOR_REG_RM,
	OPCODE_XOR_RM_IM,
	OPCODE_XOR_ACC_IM,

	OPCODE_JE,
	OPCODE_JL,
	OPCODE_JLE,
	OPCODE_JB,
	OPCODE_JBE,
	OPCODE_JP,
	OPCODE_JO,
	OPCODE_JS,
	OPCODE_JNE,
	OPCODE_JNL,
	OPCODE_JNLE,
	OPCODE_JNB,
	OPCODE_JNBE,
	OPCODE_JNP,
	OPCODE_JNO,
	OPCODE_JNS,
	OPCODE_LOOP,
	OPCODE_LOOPZ,
	OPCODE_LOOPNZ,
	OPCODE_JCXZ,
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

	"OPCODE_ADD_REG_RM       ",
	"OPCODE_ADD_RM_IM        ",
	"OPCODE_ADD_ACC_IM       ",

	"OPCODE_SUB_REG_RM       ",
	"OPCODE_SUB_RM_IM        ",
	"OPCODE_SUB_ACC_IM       ",

	"OPCODE_CMP_REG_RM       ",
	"OPCODE_CMP_RM_IM        ",
	"OPCODE_CMP_ACC_IM       ",

	"OPCODE_JE               ",
	"OPCODE_JL               ",
	"OPCODE_JLE              ",
	"OPCODE_JB               ",
	"OPCODE_JBE              ",
	"OPCODE_JP               ",
	"OPCODE_JO               ",
	"OPCODE_JS               ",
	"OPCODE_JNE              ",
	"OPCODE_JNL              ",
	"OPCODE_JNLE             ",
	"OPCODE_JNB              ",
	"OPCODE_JNBE             ",
	"OPCODE_JNP              ",
	"OPCODE_JNO              ",
	"OPCODE_JNS              ",
	"OPCODE_LOOP             ",
	"OPCODE_LOOPZ            ",
	"OPCODE_LOOPNZ           ",
	"OPCODE_JCXZ             ",
};

enum {
	OPFORMAT_NONE,
	OPFORMAT_REG_RM,
	OPFORMAT_RM_IM,
	// OPFORMAT_IM_RM,
	OPFORMAT_REG_IM,
	OPFORMAT_IM_REG,
	OPFORMAT_REG_REG16,
	OPFORMAT_REG16_REG,
	OPFORMAT_INC,
	OPFORMAT_RM,
	OPFORMAT_REG,
	OPFORMAT_SR,

	OPFORMAT_SHIFT, // RM, (1 | cl)
} opformat_t;

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
	char* type;
	uint8_t format;
	opcode_t op;
	uint8_t d;
	uint8_t s;
	uint8_t w;
	uint8_t v;
	uint8_t z;
	uint8_t mod;
	uint8_t reg; // this will store reg and segreg
	uint8_t rm;
	// uint8_t segreg;
	int16_t disp;
	uint16_t data;
	uint8_t dispSize;
	// bool_t directAddress;
} rawinstruction_t;

typedef struct {
	op_t op;
} instruction_t;


#endif
