//
//  Created by Matt Hartley on 25/08/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

// #include "sim.h"
#include "instruction.h"

// #define I(mnemonic, ...) {OP_##mnemonic, __VA_ARGS__}
#define CONCAT(a, b) a ## b
#define CONCAT2(a, b) CONCAT(a, b)
#ifndef I
#	define I(mnemonic, ...)
// decodeformat_t CONCAT2(format, __LINE__) = {OP_##mnemonic, {__VA_ARGS__}};
#endif
#define A(mnemonic, ...) I(mnemonic, __VA_ARGS__)
#ifdef SKIP_ALT
#	undef A
#	define A(mnemonic, ...) 
#endif
// #define MOD_REG_RM_CHUNKS {B_MOD}, {B_REG}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}

#define Bits(bits) {BITS_PATTERN, sizeof(#bits)-1, 0b##bits}
#define D {BITS_D, 1}
#define W {BITS_W, 1}
#define MOD {BITS_MOD, 2}
#define REG {BITS_REG, 3}
#define RM {BITS_RM, 3}
#define SR {BITS_SR, 2}
#define DATA {BITS_DATA, 0}
#define DATA_IF_W {BITS_DATA_W_IF_W, 0}

#define ImplD {BITS_D, 0, 1}
#define ImplW {BITS_W, 0, 1}
#define ImplMod(value) {BITS_MOD, 0, value}
#define ImplReg(value) {BITS_REG, 0, value}
#define ImplRm(value) {BITS_RM, 0, value}

#define Addr {BITS_ADDR}, ImplMod(0b00), ImplRm(0b110)

// MOV reg/mem to/from register
// [OPCODE_MOV_RM_TOFROM_REG] = {{"mov", OPFORMAT_REG_RM}, {{B_PATTERN, 6, 0b100010}, {B_D}, {B_W}, {B_MOD}, {B_REG}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// I(MOV, {{B_PATTERN, 6, 0b100010}, {B_D}, {B_W}, {B_MOD}, {B_REG}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}),
I(MOV, Bits(100010), D, W, MOD, REG, RM)
// MOV imm to reg/mem
// [OPCODE_MOV_IM_TO_RM] = {{"mov", OPFORMAT_RM_IM}, {{B_PATTERN, 7, 0b1100011}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b000}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}, {B_DATA_LO}, {B_DATA_HI_IF_W}}},
A(MOV, Bits(1100011), W, MOD, Bits(000), RM, DATA, DATA_IF_W)
// // MOV imm to reg
A(MOV, Bits(1011), W, REG, DATA, DATA_IF_W, ImplD)
// // MOV mem to acc
A(MOV, Bits(1010000), W, Addr, ImplReg(0), ImplD)
// // MOV acc to mem
// [OPCODE_MOV_ACC_TO_MEM] = {{"mov", OPFORMAT_REG_RM, .d=1, .rm=0b110}, {{B_PATTERN, 7, 0b1010001}, {B_W}, {B_ADDR_LO}, {B_ADDR_HI}}},
A(MOV, Bits(1010001), W, Addr, ImplReg(0))
// // MOV reg/mem to segment reg
// [OPCODE_MOV_RM_TO_SEG] = {{"mov", OPFORMAT_SR_RM}, {{B_PATTERN, 8, 0b10001110}, {B_MOD}, {B_PATTERN, 1, 0}, {B_SR}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
A(MOV, Bits(10001110), MOD, Bits(0), SR, RM, ImplD, ImplW)
// // MOV segment reg to reg/mem
// [OPCODE_MOV_SEG_TO_RM] = {{"mov", OPFORMAT_RM_SR}, {{B_PATTERN, 8, 0b10001100}, {B_MOD}, {B_PATTERN, 1, 0}, {B_SR}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
A(MOV, Bits(10001100), MOD, Bits(0), SR, RM, ImplW)

// PUSH reg/mem
// [OPCODE_PUSH_RM] = {{"push", OPFORMAT_RM, .w=1}, {{B_PATTERN, 8, 0b11111111}, {B_MOD}, {B_PATTERN, 3, 0b110}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// // PUSH reg
// [OPCODE_PUSH_REG] = {{"push", OPFORMAT_REG, .w=1}, {{B_PATTERN, 5, 0b01010}, {B_REG}}},
// // PUSH seg reg
// [OPCODE_PUSH_SR] = {{"push", OPFORMAT_SR, .w=1}, {{B_PATTERN, 3, 0b000}, {B_SR}, {B_PATTERN, 3, 0b110}}},

// // POP reg/mem
// [OPCODE_POP_RM] = {{"pop", OPFORMAT_RM, .w=1}, {{B_PATTERN, 8, 0b10001111}, {B_MOD}, {B_PATTERN, 3, 0b000}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// // POP reg
// [OPCODE_POP_REG] = {{"pop", OPFORMAT_REG, .w=1}, {{B_PATTERN, 5, 0b01011}, {B_REG}}},
// // POP seg reg
// [OPCODE_POP_SR] = {{"pop", OPFORMAT_SR, .w=1}, {{B_PATTERN, 3, 0b000}, {B_SR}, {B_PATTERN, 3, 0b111}}},

// // XCHG reg, reg/mem
// [OPCODE_XCHG_REG_RM] = {{"xchg", OPFORMAT_REG_RM, .d=1}, {{B_PATTERN, 7, 0b1000011}, {B_W}, {B_MOD}, {B_REG}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// // XCHG acc, reg
// [OPCODE_XCHG_ACC_REG] = {{"xchg", OPFORMAT_REG_RM, .w=1, .d=1, .mod=0b11}, {{B_PATTERN, 5, 0b10010}, {B_REG}}},

// // IN fixed port
// [OPCODE_IN_FIX] = {{"in", OPFORMAT_REG_IM}, {{B_PATTERN, 7, 0b1110010}, {B_W}, {B_DATA_LO}}},
// // IN variable port
// [OPCODE_IN_VAR] = {{"in", OPFORMAT_REG_REG16, .mod=0b11, .rm=0b010}, {{B_PATTERN, 7, 0b1110110}, {B_W}}},

// // OUT fixed port
// [OPCODE_OUT_FIX] = {{"out", OPFORMAT_IM_REG}, {{B_PATTERN, 7, 0b1110011}, {B_W}, {B_DATA_LO}}},
// // OUT variable port
// [OPCODE_OUT_VAR] = {{"out", OPFORMAT_REG16_REG, .mod=0b11, .rm=0b010}, {{B_PATTERN, 7, 0b1110111}, {B_W}}},

// // XLAT
// [OPCODE_XLAT] = {{"xlat", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11010111}}},

// // LOADS/STORES
// [OPCODE_LEA] = {{"lea", OPFORMAT_REG_RM, .w=1}, {{B_PATTERN, 8, 0b10001101}, MOD_REG_RM_CHUNKS}},
// [OPCODE_LDS] = {{"lds", OPFORMAT_REG_RM, .w=1}, {{B_PATTERN, 8, 0b11000101}, MOD_REG_RM_CHUNKS}},
// [OPCODE_LES] = {{"les", OPFORMAT_REG_RM, .w=1}, {{B_PATTERN, 8, 0b11000100}, MOD_REG_RM_CHUNKS}},
// [OPCODE_LAHF] = {{"lahf", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b10011111}}},
// [OPCODE_SAHF] = {{"sahf", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b10011110}}},
// [OPCODE_PUSHF] = {{"pushf", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b10011100}}},
// [OPCODE_POPF] = {{"popf", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b10011101}}},

// #define ARITHMETIC_REG_RM(code, mnemonic, bitcode) /* reg/mem, reg */ [OPCODE_##code##_REG_RM] = {{mnemonic, OPFORMAT_REG_RM}, {{B_PATTERN, 6, 0b00##bitcode##0}, {B_D}, {B_W}, {B_MOD}, {B_REG}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}}
// #define ARITHMETIC_RM_IM(code, mnemonic, bitcode) /* reg/mem, imm */ [OPCODE_##code##_RM_IM] = {{mnemonic, OPFORMAT_RM_IM}, {{B_PATTERN, 6, 0b100000}, {B_S}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b##bitcode}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}, {B_DATA_LO}, {B_DATA_HI_IF_SW_01}}}
// #define ARITHMETIC_ACC_IM(code, mnemonic, bitcode) /* acc, imm */ [OPCODE_##code##_ACC_IM] = {{mnemonic, OPFORMAT_REG_IM}, {{B_PATTERN, 7, 0b00##bitcode##10}, {B_W}, {B_DATA_LO}, {B_DATA_HI_IF_W}}}
// #define STANDARD_ARITHMETIC_FORMAT_WITH_SIGN(code, mnemonic, bitcode) \
// ARITHMETIC_REG_RM(code, mnemonic, bitcode), \
// ARITHMETIC_RM_IM(code, mnemonic, bitcode), \
// ARITHMETIC_ACC_IM(code, mnemonic, bitcode)

// // ADD reg/mem, reg
// [OPCODE_ADD_REG_RM] = {{"add", OPFORMAT_REG_RM}, {{B_PATTERN, 6, 0b000000}, {B_D}, {B_W}, {B_MOD}, {B_REG}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// // ADD reg/mem, imm
// [OPCODE_ADD_RM_IM] = {{"add", OPFORMAT_RM_IM}, {{B_PATTERN, 6, 0b100000}, {B_S}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b000}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}, {B_DATA_LO}, {B_DATA_HI_IF_SW_01}}},
// // ADD acc, imm
// [OPCODE_ADD_ACC_IM] = {{"add", OPFORMAT_REG_IM}, {{B_PATTERN, 7, 0b0000010}, {B_W}, {B_DATA_LO}, {B_DATA_HI_IF_W}}},

// // ADC formats
// STANDARD_ARITHMETIC_FORMAT_WITH_SIGN(ADC, "adc", 010),

// // INC reg/mem
// [OPCODE_INC_RM] = {{"inc", OPFORMAT_RM}, {{B_PATTERN, 7, 0b1111111}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b000}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// // INC reg
// [OPCODE_INC_REG] = {{"inc", OPFORMAT_REG, .w=1}, {{B_PATTERN, 5, 0b01000}, {B_REG}}},
// // INC ascii adjust
// [OPCODE_INC_AAA] = {{"aaa", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b00110111}}},
// // INC decimal adjust
// [OPCODE_INC_DAA] = {{"daa", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b00100111}}},

// // SUB reg/mem, reg
// [OPCODE_SUB_REG_RM] = {{"sub", OPFORMAT_REG_RM}, {{B_PATTERN, 6, 0b001010}, {B_D}, {B_W}, {B_MOD}, {B_REG}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// // SUB reg/mem, imm
// [OPCODE_SUB_RM_IM] = {{"sub", OPFORMAT_RM_IM}, {{B_PATTERN, 6, 0b100000}, {B_S}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b101}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}, {B_DATA_LO}, {B_DATA_HI_IF_SW_01}}},
// // SUB acc, imm
// [OPCODE_SUB_ACC_IM] = {{"sub", OPFORMAT_REG_IM}, {{B_PATTERN, 7, 0b0010110}, {B_W}, {B_DATA_LO}, {B_DATA_HI_IF_W}}},

// // SBB formats
// STANDARD_ARITHMETIC_FORMAT_WITH_SIGN(SBB, "sbb", 011),

// // DEC reg/mem
// [OPCODE_DEC_RM] = {{"dec", OPFORMAT_RM}, {{B_PATTERN, 7, 0b1111111}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b001}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// // DEC reg
// [OPCODE_DEC_REG] = {{"dec", OPFORMAT_REG, .w=1}, {{B_PATTERN, 5, 0b01001}, {B_REG}}},
// // NEG
// [OPCODE_NEG] = {{"neg", OPFORMAT_RM}, {{B_PATTERN, 7, 0b1111011}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b011}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},

// // CMP reg/mem, reg
// [OPCODE_CMP_REG_RM] = {{"cmp", OPFORMAT_REG_RM}, {{B_PATTERN, 6, 0b001110}, {B_D}, {B_W}, {B_MOD}, {B_REG}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// // CMP reg/mem, imm
// [OPCODE_CMP_RM_IM] = {{"cmp", OPFORMAT_RM_IM}, {{B_PATTERN, 6, 0b100000}, {B_S}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b111}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}, {B_DATA_LO}, {B_DATA_HI_IF_SW_01}}},
// // CMP acc, imm
// [OPCODE_CMP_ACC_IM] = {{"cmp", OPFORMAT_REG_IM}, {{B_PATTERN, 7, 0b0011110}, {B_W}, {B_DATA_LO}, {B_DATA_HI_IF_W}}},

// [OPCODE_AAS] = {{"aas", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b00111111}}},
// [OPCODE_DAS] = {{"das", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b00101111}}},

// // MUL
// [OPCODE_MUL] = {{"mul", OPFORMAT_RM}, {{B_PATTERN, 7, 0b1111011}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b100}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_IMUL] = {{"imul", OPFORMAT_RM}, {{B_PATTERN, 7, 0b1111011}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b101}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_AAM] = {{"aam", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11010100}, {B_PATTERN, 8, 0b00001010}}},

// // DIV
// [OPCODE_DIV] = {{"div", OPFORMAT_RM}, {{B_PATTERN, 7, 0b1111011}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b110}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_IDIV] = {{"idiv", OPFORMAT_RM}, {{B_PATTERN, 7, 0b1111011}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b111}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_AAD] = {{"aad", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11010101}, {B_PATTERN, 8, 0b00001010}}},

// // Convert
// [OPCODE_CBW] = {{"cbw", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b10011000}}},
// [OPCODE_CWD] = {{"cwd", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b10011001}}},

// // LOGIC
// [OPCODE_NOT] = {{"not", OPFORMAT_RM}, {{B_PATTERN, 7, 0b1111011}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b010}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_SHL] = {{"shl", OPFORMAT_SHIFT}, {{B_PATTERN, 6, 0b110100}, {B_V}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b100}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_SHR] = {{"shr", OPFORMAT_SHIFT}, {{B_PATTERN, 6, 0b110100}, {B_V}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b101}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_SAR] = {{"sar", OPFORMAT_SHIFT}, {{B_PATTERN, 6, 0b110100}, {B_V}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b111}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_ROL] = {{"rol", OPFORMAT_SHIFT}, {{B_PATTERN, 6, 0b110100}, {B_V}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b000}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_ROR] = {{"ror", OPFORMAT_SHIFT}, {{B_PATTERN, 6, 0b110100}, {B_V}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b001}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_RCL] = {{"rcl", OPFORMAT_SHIFT}, {{B_PATTERN, 6, 0b110100}, {B_V}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b010}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_RCR] = {{"rcr", OPFORMAT_SHIFT}, {{B_PATTERN, 6, 0b110100}, {B_V}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b011}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},

// // AND
// [OPCODE_AND_REG_RM] = {{"and", OPFORMAT_REG_RM}, {{B_PATTERN, 6, 0b001000}, {B_D}, {B_W}, {B_MOD}, {B_REG}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_AND_RM_IM] = {{"and", OPFORMAT_RM_IM}, {{B_PATTERN, 7, 0b1000000}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b100}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}, {B_DATA_LO}, {B_DATA_HI_IF_W}}},
// [OPCODE_AND_ACC_IM] = {{"and", OPFORMAT_REG_IM}, {{B_PATTERN, 7, 0b0010010}, {B_W}, {B_DATA_LO}, {B_DATA_HI_IF_W}}},

// // TEST
// // NOTE: this 6 bit pattern on this one was wrong in the manual, and I'm now unsure if it will clash with xchg
// [OPCODE_TEST_REG_RM] = {{"test", OPFORMAT_REG_RM}, {{B_PATTERN, 6, 0b100001}, {B_D}, {B_W}, {B_MOD}, {B_REG}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_TEST_RM_IM] = {{"test", OPFORMAT_RM_IM}, {{B_PATTERN, 7, 0b1111011}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b000}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}, {B_DATA_LO}, {B_DATA_HI_IF_W}}},
// [OPCODE_TEST_ACC_IM] = {{"test", OPFORMAT_REG_IM}, {{B_PATTERN, 7, 0b1010100}, {B_W}, {B_DATA_LO}, {B_DATA_HI_IF_W}}},

// // OR
// [OPCODE_OR_REG_RM] = {{"or", OPFORMAT_REG_RM}, {{B_PATTERN, 6, 0b000010}, {B_D}, {B_W}, {B_MOD}, {B_REG}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_OR_RM_IM] = {{"or", OPFORMAT_RM_IM}, {{B_PATTERN, 7, 0b1000000}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b001}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}, {B_DATA_LO}, {B_DATA_HI_IF_W}}},
// [OPCODE_OR_ACC_IM] = {{"or", OPFORMAT_REG_IM}, {{B_PATTERN, 7, 0b0000110}, {B_W}, {B_DATA_LO}, {B_DATA_HI_IF_W}}},

// // XOR
// [OPCODE_XOR_REG_RM] = {{"xor", OPFORMAT_REG_RM}, {{B_PATTERN, 6, 0b001100}, {B_D}, {B_W}, {B_MOD}, {B_REG}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_XOR_RM_IM] = {{"xor", OPFORMAT_RM_IM}, {{B_PATTERN, 7, 0b1000000}, {B_W}, {B_MOD}, {B_PATTERN, 3, 0b110}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}, {B_DATA_LO}, {B_DATA_HI_IF_W}}},
// [OPCODE_XOR_ACC_IM] = {{"xor", OPFORMAT_REG_IM}, {{B_PATTERN, 7, 0b0011010}, {B_W}, {B_DATA_LO}, {B_DATA_HI_IF_W}}},

// // REP
// #define REP_BITS 0b1111001
// [OPCODE_REP_MOVSB] = {{"rep movsb", OPFORMAT_NONE}, {{B_PATTERN, 7, REP_BITS}, {B_Z}, {B_PATTERN, 8, 0b10100100}}},
// [OPCODE_REP_MOVSW] = {{"rep movsw", OPFORMAT_NONE}, {{B_PATTERN, 7, REP_BITS}, {B_Z}, {B_PATTERN, 8, 0b10100101}}},
// [OPCODE_REP_CMPSB] = {{"rep cmpsb", OPFORMAT_NONE}, {{B_PATTERN, 7, REP_BITS}, {B_Z}, {B_PATTERN, 8, 0b10100110}}},
// [OPCODE_REP_CMPSW] = {{"rep cmpsw", OPFORMAT_NONE}, {{B_PATTERN, 7, REP_BITS}, {B_Z}, {B_PATTERN, 8, 0b10100111}}},
// [OPCODE_REP_SCASB] = {{"rep scasb", OPFORMAT_NONE}, {{B_PATTERN, 7, REP_BITS}, {B_Z}, {B_PATTERN, 8, 0b10101110}}},
// [OPCODE_REP_SCASW] = {{"rep scasw", OPFORMAT_NONE}, {{B_PATTERN, 7, REP_BITS}, {B_Z}, {B_PATTERN, 8, 0b10101111}}},
// [OPCODE_REP_LODSB] = {{"rep lodsb", OPFORMAT_NONE}, {{B_PATTERN, 7, REP_BITS}, {B_Z}, {B_PATTERN, 8, 0b10101100}}},
// [OPCODE_REP_LODSW] = {{"rep lodsw", OPFORMAT_NONE}, {{B_PATTERN, 7, REP_BITS}, {B_Z}, {B_PATTERN, 8, 0b10101101}}},
// [OPCODE_REP_STOSB] = {{"rep stosb", OPFORMAT_NONE}, {{B_PATTERN, 7, REP_BITS}, {B_Z}, {B_PATTERN, 8, 0b10101010}}},
// [OPCODE_REP_STOSW] = {{"rep stosw", OPFORMAT_NONE}, {{B_PATTERN, 7, REP_BITS}, {B_Z}, {B_PATTERN, 8, 0b10101011}}},

// // CALL
// [OPCODE_CALL_DIRECT_SEG] = {{"call", OPFORMAT_IP_INC, .skipRmSizeSpecifier=1}, {{B_PATTERN, 8, 0b11101000}, {B_IP_INC_LO}, {B_IP_INC_HI}}},
// [OPCODE_CALL_INDIRECT_SEG] = {{"call", OPFORMAT_RM, .skipRmSizeSpecifier=1, .w=1}, {{B_PATTERN, 8, 0b11111111}, {B_MOD}, {B_PATTERN, 3, 0b010}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_CALL_DIRECT_INTERSEG] = {{"call", OPFORMAT_IP_CS, .skipRmSizeSpecifier=1}, {{B_PATTERN, 8, 0b10011010}, {B_IP_LO}, {B_IP_HI}, {B_DATA_LO}, {B_DATA_HI}}},
// [OPCODE_CALL_INDIRECT_INTERSEG] = {{"call far", OPFORMAT_RM, .skipRmSizeSpecifier=1, .w=1}, {{B_PATTERN, 8, 0b11111111}, {B_MOD}, {B_PATTERN, 3, 0b011}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},

// // JMP unconditional
// [OPCODE_JMP_DIRECT_SEG] = {{"jmp", OPFORMAT_IP_INC}, {{B_PATTERN, 8, 0b11101001}, {B_IP_INC_LO}, {B_IP_INC_HI}}},
// [OPCODE_JMP_DIRECT_SEG_SHORT] = {{"jmp", OPFORMAT_IP_INC}, {{B_PATTERN, 8, 0b11101011}, {B_INC8}}},
// [OPCODE_JMP_INDIRECT_SEG] = {{"jmp", OPFORMAT_RM, .w=1}, {{B_PATTERN, 8, 0b11111111}, {B_MOD}, {B_PATTERN, 3, 0b100}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},
// [OPCODE_JMP_DIRECT_INTERSEG] = {{"jmp", OPFORMAT_IP_CS}, {{B_PATTERN, 8, 0b11101010}, {B_IP_LO}, {B_IP_HI}, {B_DATA_LO}, {B_DATA_HI}}},
// [OPCODE_JMP_INDIRECT_INTERSEG] = {{"jmp far", OPFORMAT_RM, .w=1}, {{B_PATTERN, 8, 0b11111111}, {B_MOD}, {B_PATTERN, 3, 0b101}, {B_RM}, {B_DISP_LO_IF_MOD}, {B_DISP_HI_IF_MOD}}},

// // RET
// [OPCODE_RET_SEG] = {{"ret", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11000011}}},
// [OPCODE_RET_SEG_IM] = {{"ret", OPFORMAT_IM}, {{B_PATTERN, 8, 0b11000010}, {B_DATA_LO}, {B_DATA_HI}}},
// [OPCODE_RET_INTERSEG] = {{"retf", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11001011}}},
// [OPCODE_RET_INTERSEG_IM] = {{"retf", OPFORMAT_IM}, {{B_PATTERN, 8, 0b11001010}, {B_DATA_LO}, {B_DATA_HI}}},

// // JUMPS conditional
// [OPCODE_JE] = {{"je", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01110100}, {B_INC8}}},
// [OPCODE_JL] = {{"jl", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01111100}, {B_INC8}}},
// [OPCODE_JLE] = {{"jle", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01111110}, {B_INC8}}},
// [OPCODE_JB] = {{"jb", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01110010}, {B_INC8}}},
// [OPCODE_JBE] = {{"jbe", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01110110}, {B_INC8}}},
// [OPCODE_JP] = {{"jp", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01111010}, {B_INC8}}},
// [OPCODE_JO] = {{"jo", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01110000}, {B_INC8}}},
// [OPCODE_JS] = {{"js", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01111000}, {B_INC8}}},
// [OPCODE_JNE] = {{"jnz", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01110101}, {B_INC8}}},
// [OPCODE_JNL] = {{"jnl", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01111101}, {B_INC8}}},
// [OPCODE_JNLE] = {{"jnle", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01111111}, {B_INC8}}},
// [OPCODE_JNB] = {{"jnb", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01110011}, {B_INC8}}},
// [OPCODE_JNBE] = {{"jnbe", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01110111}, {B_INC8}}},
// [OPCODE_JNP] = {{"jnp", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01111011}, {B_INC8}}},
// [OPCODE_JNO] = {{"jno", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01110001}, {B_INC8}}},
// [OPCODE_JNS] = {{"jns", OPFORMAT_INC}, {{B_PATTERN, 8, 0b01111001}, {B_INC8}}},
// [OPCODE_LOOP] = {{"loop", OPFORMAT_INC}, {{B_PATTERN, 8, 0b11100010}, {B_INC8}}},
// [OPCODE_LOOPZ] = {{"loopz", OPFORMAT_INC}, {{B_PATTERN, 8, 0b11100001}, {B_INC8}}},
// [OPCODE_LOOPNZ] = {{"loopnz", OPFORMAT_INC}, {{B_PATTERN, 8, 0b11100000}, {B_INC8}}},
// [OPCODE_JCXZ] = {{"jcxz", OPFORMAT_INC}, {{B_PATTERN, 8, 0b11100011}, {B_INC8}}},

// // INTERRUPTS
// [OPCODE_INT] = {{"int", OPFORMAT_IM}, {{B_PATTERN, 8, 0b11001101}, {B_DATA_LO}}},
// [OPCODE_INT_TYPE3] = {{"int3", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11001100}}},
// [OPCODE_INTO] = {{"into", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11001110}}},
// [OPCODE_IRET] = {{"iret", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11001111}}},

// // PROCESSOR CONTROL
// [OPCODE_CLC] = {{"clc", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11111000}}},
// [OPCODE_CMC] = {{"cmc", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11110101}}},
// [OPCODE_STC] = {{"stc", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11111001}}},
// [OPCODE_CLD] = {{"cld", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11111100}}},
// [OPCODE_STD] = {{"std", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11111101}}},
// [OPCODE_CLI] = {{"cli", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11111010}}},
// [OPCODE_STI] = {{"sti", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11111011}}},
// [OPCODE_HLT] = {{"hlt", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b11110100}}},
// [OPCODE_WAIT] = {{"wait", OPFORMAT_NONE}, {{B_PATTERN, 8, 0b10011011}}},

// // OPCODE_ESC,

// [OPCODE_LOCK] = {{"lock", OPFORMAT_PREFIX}, {{B_PATTERN, 8, 0b11110000}}},

// [OPCODE_SEGMENT] = {{"segment", OPFORMAT_SEGMENT}, {{B_PATTERN, 3, 0b001}, {B_SR}, {B_PATTERN, 3, 0b110}}},


#undef I
#undef A
