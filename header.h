#ifndef _HEADER_H
#define _HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu6502.h"

#ifndef _BYTE
typedef unsigned char byte;
#define _BYTE
#endif

#ifndef _WORD
typedef unsigned short word;
#endif

#ifdef MAIN
#define LINK
#else
#define LINK extern
#endif

LINK CPU6502  cpu;
LINK byte       ram[65536];
LINK char       runDebugger;

extern void c6502_irq(CPU6502* cpu);
extern void c6502_nmi(CPU6502* cpu);
extern void c6502_reset(CPU6502* cpu);

extern void Debugger();
extern word Disassem(word addr, char* dest);
extern int loadFile(char* filename);

#endif
