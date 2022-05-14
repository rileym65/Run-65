#ifndef CPU6502_H
#define CPU6502_H

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_V 0x40
#define FLAG_N 0x80

#ifndef _BYTE
typedef unsigned char byte;
#define _BYTE
#endif

#ifndef _WORD
typedef unsigned short word;
#endif

typedef struct {
  byte  a;
  byte  y;
  byte  x;
  byte  s;
  byte  p;
  word  pc;
  byte  clocks;
  byte  idle;
  byte  (*readMem)(word addr);
  void  (*writeMem)(word addr, byte value);
  byte  irq;
  byte  nmi;
  } CPU6502;

#ifdef CCPU
  void (*op[257])(CPU6502 *cpu);
#endif

extern void c6502_cycle();
extern void c6502_setup();
extern void c6502_irq(CPU6502* cpu);
extern void c6502_nmi(CPU6502* cpu);
extern void c6502_reset(CPU6502* cpu);


#endif

