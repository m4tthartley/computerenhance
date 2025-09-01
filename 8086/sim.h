//
//  Created by Matt Hartley on 25/08/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//


#ifndef SIM_H
#define SIM_H

#include <core/core.h>
#include "instruction_table.h"


// typedef enum {
// 	// OPCODE_NULL = 0,

// 	OPCODE_MOV_RM_TOFROM_REG,
// 	OPCODE_MOV_IM_TO_RM,
// 	OPCODE_MOV_IM_TO_REG,
// 	OPCODE_MOV_MEM_TO_ACC,
// 	OPCODE_MOV_ACC_TO_MEM,
// 	OPCODE_MOV_RM_TO_SEG,
// 	OPCODE_MOV_SEG_TO_RM,

// 	OPCODE_PUSH_RM,
// 	OPCODE_PUSH_REG,
// 	OPCODE_PUSH_SR,

// 	OPCODE_POP_RM,
// 	OPCODE_POP_REG,
// 	OPCODE_POP_SR,

// 	OPCODE_XCHG_REG_RM,
// 	OPCODE_XCHG_ACC_REG,

// 	OPCODE_IN_FIX,
// 	OPCODE_IN_VAR,

// 	OPCODE_OUT_FIX,
// 	OPCODE_OUT_VAR,

// 	OPCODE_XLAT,

// 	OPCODE_LEA,
// 	OPCODE_LDS,
// 	OPCODE_LES,
// 	OPCODE_LAHF,
// 	OPCODE_SAHF,
// 	OPCODE_PUSHF,
// 	OPCODE_POPF,

// 	OPCODE_ADD_REG_RM,
// 	OPCODE_ADD_RM_IM,
// 	OPCODE_ADD_ACC_IM,

// 	OPCODE_ADC_REG_RM,
// 	OPCODE_ADC_RM_IM,
// 	OPCODE_ADC_ACC_IM,

// 	OPCODE_INC_RM,
// 	OPCODE_INC_REG,
// 	OPCODE_INC_AAA,
// 	OPCODE_INC_DAA,

// 	OPCODE_SUB_REG_RM,
// 	OPCODE_SUB_RM_IM,
// 	OPCODE_SUB_ACC_IM,

// 	OPCODE_SBB_REG_RM,
// 	OPCODE_SBB_RM_IM,
// 	OPCODE_SBB_ACC_IM,

// 	OPCODE_DEC_RM,
// 	OPCODE_DEC_REG,
// 	OPCODE_NEG,

// 	OPCODE_CMP_REG_RM,
// 	OPCODE_CMP_RM_IM,
// 	OPCODE_CMP_ACC_IM,

// 	OPCODE_AAS,
// 	OPCODE_DAS,

// 	OPCODE_MUL,
// 	OPCODE_IMUL,
// 	OPCODE_AAM,

// 	OPCODE_DIV,
// 	OPCODE_IDIV,
// 	OPCODE_AAD,

// 	OPCODE_CBW,
// 	OPCODE_CWD,

// 	OPCODE_NOT,
// 	OPCODE_SHL,
// 	OPCODE_SHR,
// 	OPCODE_SAR,
// 	OPCODE_ROL,
// 	OPCODE_ROR,
// 	OPCODE_RCL,
// 	OPCODE_RCR,

// 	OPCODE_AND_REG_RM,
// 	OPCODE_AND_RM_IM,
// 	OPCODE_AND_ACC_IM,

// 	OPCODE_TEST_REG_RM,
// 	OPCODE_TEST_RM_IM,
// 	OPCODE_TEST_ACC_IM,
	
// 	OPCODE_OR_REG_RM,
// 	OPCODE_OR_RM_IM,
// 	OPCODE_OR_ACC_IM,

// 	OPCODE_XOR_REG_RM,
// 	OPCODE_XOR_RM_IM,
// 	OPCODE_XOR_ACC_IM,

// 	// OPCODE_REP,
// 	OPCODE_REP_MOVSB,
// 	OPCODE_REP_MOVSW,
// 	OPCODE_REP_CMPSB,
// 	OPCODE_REP_CMPSW,
// 	OPCODE_REP_SCASB,
// 	OPCODE_REP_SCASW,
// 	OPCODE_REP_LODSB,
// 	OPCODE_REP_LODSW,
// 	OPCODE_REP_STOSB,
// 	OPCODE_REP_STOSW,

// 	OPCODE_CALL_DIRECT_SEG,
// 	OPCODE_CALL_INDIRECT_SEG,
// 	OPCODE_CALL_DIRECT_INTERSEG,
// 	OPCODE_CALL_INDIRECT_INTERSEG,

// 	OPCODE_JMP_DIRECT_SEG,
// 	OPCODE_JMP_DIRECT_SEG_SHORT,
// 	OPCODE_JMP_INDIRECT_SEG,
// 	OPCODE_JMP_DIRECT_INTERSEG,
// 	OPCODE_JMP_INDIRECT_INTERSEG,

// 	OPCODE_RET_SEG,
// 	OPCODE_RET_SEG_IM,
// 	OPCODE_RET_INTERSEG,
// 	OPCODE_RET_INTERSEG_IM,

// 	OPCODE_JE,
// 	OPCODE_JL,
// 	OPCODE_JLE,
// 	OPCODE_JB,
// 	OPCODE_JBE,
// 	OPCODE_JP,
// 	OPCODE_JO,
// 	OPCODE_JS,
// 	OPCODE_JNE,
// 	OPCODE_JNL,
// 	OPCODE_JNLE,
// 	OPCODE_JNB,
// 	OPCODE_JNBE,
// 	OPCODE_JNP,
// 	OPCODE_JNO,
// 	OPCODE_JNS,
// 	OPCODE_LOOP,
// 	OPCODE_LOOPZ,
// 	OPCODE_LOOPNZ,
// 	OPCODE_JCXZ,

// 	OPCODE_INT,
// 	OPCODE_INT_TYPE3,
// 	OPCODE_INTO,
// 	OPCODE_IRET,

// 	OPCODE_CLC,
// 	OPCODE_CMC,
// 	OPCODE_STC,
// 	OPCODE_CLD,
// 	OPCODE_STD,
// 	OPCODE_CLI,
// 	OPCODE_STI,
// 	OPCODE_HLT,
// 	OPCODE_WAIT,
// 	OPCODE_ESC,
// 	OPCODE_LOCK,
// 	OPCODE_SEGMENT,
// } opcode_t;

// const char* opcodeNames[] = {
// 	// "OPCODE_NULL             ",
// 	"OPCODE_MOV_RM_TOFROM_REG",
// 	"OPCODE_MOV_IM_TO_RM     ",
// 	"OPCODE_MOV_IM_TO_REG    ",
// 	"OPCODE_MOV_MEM_TO_ACC   ",
// 	"OPCODE_MOV_ACC_TO_MEM   ",
// 	"OPCODE_MOV_RM_TO_SEG    ",
// 	"OPCODE_MOV_SEG_TO_RM    ",

// 	"OPCODE_ADD_REG_RM       ",
// 	"OPCODE_ADD_RM_IM        ",
// 	"OPCODE_ADD_ACC_IM       ",

// 	"OPCODE_SUB_REG_RM       ",
// 	"OPCODE_SUB_RM_IM        ",
// 	"OPCODE_SUB_ACC_IM       ",

// 	"OPCODE_CMP_REG_RM       ",
// 	"OPCODE_CMP_RM_IM        ",
// 	"OPCODE_CMP_ACC_IM       ",

// 	"OPCODE_JE               ",
// 	"OPCODE_JL               ",
// 	"OPCODE_JLE              ",
// 	"OPCODE_JB               ",
// 	"OPCODE_JBE              ",
// 	"OPCODE_JP               ",
// 	"OPCODE_JO               ",
// 	"OPCODE_JS               ",
// 	"OPCODE_JNE              ",
// 	"OPCODE_JNL              ",
// 	"OPCODE_JNLE             ",
// 	"OPCODE_JNB              ",
// 	"OPCODE_JNBE             ",
// 	"OPCODE_JNP              ",
// 	"OPCODE_JNO              ",
// 	"OPCODE_JNS              ",
// 	"OPCODE_LOOP             ",
// 	"OPCODE_LOOPZ            ",
// 	"OPCODE_LOOPNZ           ",
// 	"OPCODE_JCXZ             ",
// };

// enum {
// 	OPFORMAT_NONE,
// 	OPFORMAT_REG_RM,
// 	OPFORMAT_RM_IM,
// 	// OPFORMAT_IM_RM,
// 	OPFORMAT_REG_IM,
// 	OPFORMAT_IM_REG,

// 	OPFORMAT_SR_RM,
// 	OPFORMAT_RM_SR,

// 	OPFORMAT_IM,
// 	OPFORMAT_REG_REG16,
// 	OPFORMAT_REG16_REG,
// 	OPFORMAT_INC,
// 	OPFORMAT_IP_INC,
// 	OPFORMAT_RM,
// 	OPFORMAT_REG,
// 	OPFORMAT_SR,

// 	OPFORMAT_SHIFT, // RM, (1 | cl)

// 	OPFORMAT_PREFIX,
// 	OPFORMAT_SEGMENT,
// 	OPFORMAT_IP_CS,
// } opformat_t;

// #define REG_ENABLED_MASK 0b10000000
// enum {
// 	EFADDR_BASE = (1<<0),
// 	EFADDR_OFF = (1<<1),
// 	EFADDR_DISP8 = (1<<2),
// 	EFADDR_DISP16 = (1<<2),
// };
// typedef struct {
// 	uint8_t baseReg;
// 	uint8_t offReg;
// 	uint8_t flags;
// 	uint16_t disp;
// } efaddr_t;

// efaddr_t effectiveAddressTable[] = {
// 	{bx, si, EFADDR_BASE|EFADDR_OFF},
// 	{bx, di, EFADDR_BASE|EFADDR_OFF},
// 	{bp, si, EFADDR_BASE|EFADDR_OFF},
// 	{bp, di, EFADDR_BASE|EFADDR_OFF},
// 	{si,  0, EFADDR_BASE},
// 	{di,  0, EFADDR_BASE},
// 	{bp,  0, EFADDR_BASE|EFADDR_DISP16},
// 	{bx,  0, EFADDR_BASE},
// };

typedef enum {
	OPERAND_NONE,
	OPERAND_REG,
	OPERAND_SEGREG,
	OPERAND_EFF_ADDR,
	// OPERAND_DIRECT_ADDR,
	OPERAND_IMMEDIATE,
	OPERAND_IP,
	OPERAND_INCREMENT,
} operandtype_t;
enum {
	OPERAND_FLAG_OFFSET_REGISTER = (1<<0),
	OPERAND_FLAG_DIRECT_ADDR 	 = (1<<1),
	OPERAND_FLAG_WIDE			 = (1<<2),
	OPERAND_FLAG_HIGH			 = (1<<3),
	OPERAND_FLAG_SIGNED			 = (1<<4),
	OPERAND_FLAG_SIZE_SPECIFIER  = (1<<5),
} operandflags_t;
typedef struct {
	operandtype_t type;
	// for effective addresses reg will encode both registers in separate nibbles
	// the reg only needs 3 bits, so maybe the 4th bit could be if it's enabled
	uint8_t reg;
	uint8_t regOff;
	uint8_t segreg;
	union {
		uint16_t data;
		uint16_t address;
		int16_t displacement;
	};
	uint8_t flags;
} operand_t;

// typedef struct {
// 	op_t op;
// } instructionencoding_t;

typedef struct {
	char* type;
	uint8_t format;
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
	uint16_t address;
	// bool_t directAddress;
	bool_t skipRmSizeSpecifier;
	// ^ old stuff
	
	op_t op;
	uint8_t size;
	bool_t wide;

	union {
		operand_t operands[2];
		struct {
			operand_t operand0;
			operand_t operand1;
		};
	};
} rawinstruction_t;

// typedef struct {
// 	op_t op;
// } instruction_t;


// #define al 0b000
// #define cl 0b001
// #define dl 0b010
// #define bl 0b011
// #define ah 0b100
// #define ch 0b101
// #define dh 0b110
// #define bh 0b111

enum {
	ax = 0b000,
	cx = 0b001,
	dx = 0b010,
	bx = 0b011,
	sp = 0b100,
	bp = 0b101,
	si = 0b110,
	di = 0b111,

	al = 0b000,
	cl = 0b001,
	dl = 0b010,
	bl = 0b011,
	ah = 0b100,
	ch = 0b101,
	dh = 0b110,
	bh = 0b111,

	es = 0b1000,
	cs = 0b1001,
	ss = 0b1010,
	ds = 0b1011,
};

// #define es 0b00
// #define cs 0b01
// #define ss 0b10
// #define ds 0b11

typedef struct {
	union {
		uint16_t word;
		struct {
			uint8_t lo;
			uint8_t hi;
		};
	};
} reg_t;

typedef struct {
	// uint8_t* instructionData;
	// uint16_t ip;

	union {
		struct {
			reg_t ax;
			reg_t cx;
			reg_t dx;
			reg_t bx;

			reg_t sp;
			reg_t bp;
			reg_t si;
			reg_t di;

			reg_t es;
			reg_t cs;
			reg_t ss;
			reg_t ds;

			uint16_t ip;
			uint16_t lastIp;
			uint16_t flags;

			// union {
			// 	uint16_t ax;
			// 	struct {
			// 		uint8_t al;
			// 		uint8_t ah;
			// 	};
			// };
		};
		reg_t registers[12];
	};
} cpu_t;


#endif
