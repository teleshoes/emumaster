#include "m6502.h"

const quint8 M6502::cyclesTable[256] = {
/* 0x00 */7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
/* 0x10 */2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 0x20 */6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
/* 0x30 */2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 0x40 */6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
/* 0x50 */2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 0x60 */6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
/* 0x70 */2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 0x80 */2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
/* 0x90 */2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
/* 0xA0 */2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
/* 0xB0 */2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
/* 0xC0 */2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
/* 0xD0 */2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 0xE0 */2, 6, 3, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
/* 0xF0 */2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, };

const quint8 M6502::sizeTable[256] = {
/* 0x00 */1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0x10 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
/* 0x20 */3, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0x30 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
/* 0x40 */1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0x50 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
/* 0x60 */1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0x70 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
/* 0x80 */2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0x90 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
/* 0xA0 */2, 2, 2, 2, 2, 2, 2, 3, 1, 2, 1, 2, 3, 3, 3, 4,
/* 0xB0 */2, 2, 1, 2, 2, 2, 2, 4, 1, 3, 1, 3, 3, 3, 3, 4,
/* 0xC0 */2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0xD0 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
/* 0xE0 */2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0xF0 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3 };

const quint8 M6502::addressingModeTable[256] = {
/* 0x00 */Impli, IndiX, Impli, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0x08 */Impli, Immed, Accum, Immed, Absol, Absol, Absol, Absol,
/* 0x10 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPX, ZerPX,
/* 0x18 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoX, AbsoX,
/* 0x20 */Absol, IndiX, Impli, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0x28 */Impli, Immed, Accum, Immed, Absol, Absol, Absol, Absol,
/* 0x30 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPX, ZerPX,
/* 0x38 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoX, AbsoX,
/* 0x40 */Impli, IndiX, Impli, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0x48 */Impli, Immed, Accum, Immed, Absol, Absol, Absol, Absol,
/* 0x50 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPX, ZerPX,
/* 0x58 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoX, AbsoX,
/* 0x60 */Impli, IndiX, Impli, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0x68 */Impli, Immed, Accum, Immed, Indir, Absol, Absol, Absol,
/* 0x70 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPX, ZerPX,
/* 0x78 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoX, AbsoX,
/* 0x80 */Immed, IndiX, Immed, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0x88 */Impli, Immed, Impli, Immed, Absol, Absol, Absol, Absol,
/* 0x90 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPY, ZerPY,
/* 0x98 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoY, AbsoY,
/* 0xA0 */Immed, IndiX, Immed, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0xA8 */Impli, Immed, Impli, Immed, Absol, Absol, Absol, Absol,
/* 0xB0 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPY, ZerPY,
/* 0xB8 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoY, AbsoY,
/* 0xC0 */Immed, IndiX, Immed, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0xC8 */Impli, Immed, Impli, Immed, Absol, Absol, Absol, Absol,
/* 0xD0 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPX, ZerPX,
/* 0xD8 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoX, AbsoX,
/* 0xE0 */Immed, IndiX, Immed, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0xE8 */Impli, Immed, Impli, Immed, Absol, Absol, Absol, Absol,
/* 0xF0 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPX, ZerPX,
/* 0xF8 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoX, AbsoX };

const char *M6502::nameTable[256] = {
/* 0x00 */"BRK", "ORA", "KIL", "SLO", "DOP", "ORA", "ASL", "SLO",
/* 0x08 */"PHP", "ORA", "ASL", "AAC", "TOP", "ORA", "ASL", "SLO",
/* 0x10 */"BPL", "ORA", "KIL", "SLO", "DOP", "ORA", "ASL", "SLO",
/* 0x18 */"CLC", "ORA", "NOP", "SLO", "TOP", "ORA", "ASL", "SLO",
/* 0x20 */"JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA",
/* 0x28 */"PLP", "AND", "ROL", "AAC", "BIT", "AND", "ROL", "RLA",
/* 0x30 */"BMI", "AND", "KIL", "RLA", "DOP", "AND", "ROL", "RLA",
/* 0x38 */"SEC", "AND", "NOP", "RLA", "TOP", "AND", "ROL", "RLA",
/* 0x40 */"RTI", "EOR", "KIL", "SRE", "DOP", "EOR", "LSR", "SRE",
/* 0x48 */"PHA", "EOR", "LSR", "ASR", "JMP", "EOR", "LSR", "SRE",
/* 0x50 */"BVC", "EOR", "KIL", "SRE", "DOP", "EOR", "LSR", "SRE",
/* 0x58 */"CLI", "EOR", "NOP", "SRE", "TOP", "EOR", "LSR", "SRE",
/* 0x60 */"RTS", "ADC", "KIL", "RRA", "DOP", "ADC", "ROR", "RRA",
/* 0x68 */"PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
/* 0x70 */"BVS", "ADC", "KIL", "RRA", "DOP", "ADC", "ROR", "RRA",
/* 0x78 */"SEI", "ADC", "NOP", "RRA", "TOP", "ADC", "ROR", "RRA",
/* 0x80 */"DOP", "STA", "DOP", "AAX", "STY", "STA", "STX", "AAX",
/* 0x88 */"DEY", "DOP", "TXA", "XAA", "STY", "STA", "STX", "AAX",
/* 0x90 */"BCC", "STA", "KIL", "AXA", "STY", "STA", "STX", "AAX",
/* 0x98 */"TYA", "STA", "TXS", "XAS", "SYA", "STA", "SXA", "AXA",
/* 0xA0 */"LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX",
/* 0xA8 */"TAY", "LDA", "TAX", "ATX", "LDY", "LDA", "LDX", "LAX",
/* 0xB0 */"BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX",
/* 0xB8 */"CLV", "LDA", "TSX", "LAR", "LDY", "LDA", "LDX", "LAX",
/* 0xC0 */"CPY", "CMP", "DOP", "DCP", "CPY", "CMP", "DEC", "DCP",
/* 0xC8 */"INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
/* 0xD0 */"BNE", "CMP", "KIL", "DCP", "DOP", "CMP", "DEC", "DCP",
/* 0xD8 */"CLD", "CMP", "NOP", "DCP", "TOP", "CMP", "DEC", "DCP",
/* 0xE0 */"CPX", "SBC", "DOP", "ISC", "CPX", "SBC", "INC", "ISC",
/* 0xE8 */"INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISC",
/* 0xF0 */"BEQ", "SBC", "KIL", "ISC", "DOP", "SBC", "INC", "ISC",
/* 0xF8 */"SED", "SBC", "NOP", "ISC", "TOP", "SBC", "INC", "ISC" };

