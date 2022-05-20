#define CCPU

#include "header.h"
#include "cpu6502.h"

byte rdmem(CPU6502* cpu, word address) {
  if (cpu->readMem != NULL) return cpu->readMem(address);
  return 0xff;
  }

void wrmem(CPU6502* cpu, word address, byte value) {
  if (cpu->writeMem != NULL) cpu->writeMem(address, value);
  }

byte fetch(CPU6502* cpu) {
  return rdmem(cpu, cpu->pc++);
  }

word IndirectAddress(CPU6502* cpu) {
  word addr;
  word address;
  addr = fetch(cpu);
  address = rdmem(cpu, addr);
  addr = (addr + 1) & 0xff;
  address |= (rdmem(cpu, addr) << 8);
  return address;
  }

word IndexedIndirectXAddress(CPU6502* cpu) {
  word addr;
  word address;
  addr = (fetch(cpu) + cpu->x) & 0xff;
  address = rdmem(cpu, addr);
  addr = (addr + 1) & 0xff;
  address |= (rdmem(cpu, addr) << 8);
  return address;
  }

word IndexedIndirectYAddress(CPU6502* cpu, byte inc) {
  word addr;
  word address;
  addr = fetch(cpu);
  address = rdmem(cpu, addr);
  if ((addr & 0xff) == 0xff) cpu->clocks += inc;
  address |= (rdmem(cpu, (addr+1) & 0xff) << 8);
  address += cpu->y;
  return address;
  }

word getEA(CPU6502* cpu, byte inst, byte inc) {
  word address;
  inst = (inst >> 2) & 0x07;
  if (inst == 0) return IndexedIndirectXAddress(cpu);
  if (inst == 1) return fetch(cpu);
  if (inst == 2) return fetch(cpu);
  if (inst == 3) {
    address = fetch(cpu);
    address |= (fetch(cpu) << 8);
    return address;
    }
  if (inst == 4) return IndexedIndirectYAddress(cpu, inc);
  if (inst == 5) {
    return (fetch(cpu) + cpu->x) & 0xff;
    }
  if (inst == 6) {
    address = fetch(cpu);
    address |= (fetch(cpu) << 8);
    if ((address & 0xff00) != ((address + cpu->y) & 0xff00)) cpu->clocks += inc;
    return address + cpu->y;
    }
  if (inst == 7) {
    address = fetch(cpu);
    address |= (fetch(cpu) << 8);
    if ((address & 0xff00) != ((address + cpu->x) & 0xff00)) cpu->clocks += inc;
    return address + cpu->x;
    }
  return 0;
  }

void c6502_adc(CPU6502* cpu, byte arg) {
  word a,b,c;
  if (cpu->p & FLAG_D) {
    a = (((cpu->a & 0xf0) >> 4) * 10) + (cpu->a & 0x0f);
    b = (((arg & 0xf0) >> 4) * 10) + (arg & 0x0f);
    c = a + b;
    if (cpu->p & FLAG_C) c++;
    if (c >= 100) {
      cpu->p |= FLAG_C;
      c -= 100;
      }
    else {
      cpu->p &= (~FLAG_C);
      }
    cpu->a = (c / 10) << 4;
    cpu->a |= (c % 10);
    if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
    cpu->p &= (~FLAG_N);
    cpu->p &= (~FLAG_V);
    }
  else {
    a = cpu->a;
    b = arg;
    c = a + b;
    if ((cpu->p & 1) != 0) c++;
    cpu->a = c & 0xff;
    if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
    if (cpu->a >= 0x80) cpu->p |= FLAG_N; else cpu->p &= (~FLAG_N);
    if (c >= 256) cpu->p |= FLAG_C; else cpu->p &= (~FLAG_C);
    if ((a & 0x80) == (b & 0x80)) {
      if ((a & 0x80) != (cpu->a & 0x80))
        cpu->p |= FLAG_V;
      else
        cpu->p &= (~FLAG_V);
      }
    else
      cpu->p &= (~FLAG_V);
    }
  }


void c6502_and(CPU6502* cpu, byte inst) {
  word address;
  address = getEA(cpu, inst, 1);
  if (inst == 0x29) cpu->a &= address;
    else cpu->a &= rdmem(cpu, address);
  if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->a & 0x80);
  }

void c6502_asl(CPU6502* cpu, byte inst) {
  byte v;
  word address;
  if (inst == 0x0a) v = cpu->a;
  else {
    address = getEA(cpu, inst, 1);
    v = rdmem(cpu, address);
    }
  if (v >= 0x80) cpu->p |= FLAG_C;
    else cpu->p &= (~FLAG_C);
  v <<= 1;
  if (inst == 0x0a) cpu->a = v;
    else wrmem(cpu, address, v);
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (v & 0x80);
  }

void c6502_bit(CPU6502* cpu, byte inst) {
  byte v;
  word address;
  address = getEA(cpu, inst, 1);
  v = rdmem(cpu, address);
  v &= cpu->a;
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x3f) | (v & 0xc0);
  }

void c6502_cp(CPU6502* cpu, byte arg1, byte arg2) {
  if (arg1 == arg2) {
    cpu->p |= FLAG_Z;
    cpu->p |= FLAG_C;
    cpu->p &= (~FLAG_N);
    return;
    }
  if (arg1 > arg2) {
    cpu->p &= (~FLAG_Z);
    cpu->p |= FLAG_C;
    arg1 -= arg2;
    cpu->p = (cpu->p & 0x7f) | (arg1 & 0x80);
    return;
    }
  cpu->p &= (~FLAG_Z);
  cpu->p &= (~FLAG_C);
  arg1 -= arg2;
  cpu->p = (cpu->p & 0x7f) | (arg1 & 0x80);
  }

void c6502_cmp(CPU6502* cpu, byte inst) {
  byte v;
  word address;
  address = getEA(cpu, inst, 1);
  if (inst == 0xc9) v = address;
    else v = rdmem(cpu, address);
  c6502_cp(cpu, cpu->a, v);
  }

void c6502_cpx(CPU6502* cpu, byte inst) {
  byte v;
  word address;
  if (inst == 0xe0) v = fetch(cpu);
  else {
    address = getEA(cpu, inst, 1);
    v = rdmem(cpu, address);
    }
  c6502_cp(cpu, cpu->x, v);
  }

void c6502_cpy(CPU6502* cpu, byte inst) {
  byte v;
  word address;
  if (inst == 0xc0) v = fetch(cpu);
  else {
    address = getEA(cpu, inst, 1);
    v = rdmem(cpu, address);
    }
  c6502_cp(cpu, cpu->y, v);
  }

void c6502_dec(CPU6502* cpu, byte inst) {
  byte c;
  byte v;
  word address;
  c = cpu->clocks;
  address = getEA(cpu, inst, 1);
  cpu->clocks = c;
  v = rdmem(cpu, address) - 1;
  wrmem(cpu, address, v);
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (v & 0x80);
  }

void c6502_eor(CPU6502* cpu, byte inst) {
  word address;
  address = getEA(cpu, inst, 1);
  if (inst == 0x49) cpu->a ^= address;
    else cpu->a ^= rdmem(cpu, address);
  if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->a & 0x80);
  }

void c6502_inc(CPU6502* cpu, byte inst) {
  byte c;
  byte v;
  word address;
  c = cpu->clocks;
  address = getEA(cpu, inst, 1);
  cpu->clocks = c;
  v = rdmem(cpu, address) + 1;
  wrmem(cpu, address, v);
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (v & 0x80);
  }

void c6502_lda(CPU6502* cpu, byte inst) {
  word address;
  address = getEA(cpu, inst, 1);
  if (inst == 0xa9) cpu->a = address;
    else cpu->a = rdmem(cpu, address);
  if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->a & 0x80);
  }

void c6502_ldx(CPU6502* cpu, byte inst) {
  word address;
  if (inst == 0xa2) address = getEA(cpu, 0x08, 1);
  else if (inst == 0xb6) address = (fetch(cpu) + cpu->y) & 0xff;
  else if (inst == 0xbe) address = getEA(cpu, 0x18, 1);
  else {
    address = getEA(cpu, inst, 1);
    }
  if (inst == 0xa2) cpu->x = address;
    else cpu->x = rdmem(cpu, address);
  if (cpu->x == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->x & 0x80);
  }

void c6502_ldy(CPU6502* cpu, byte inst) {
  word address;
  if (inst == 0xa0) address = getEA(cpu, 0x08, 1);
  else address = getEA(cpu, inst, 1);
  if (inst == 0xa0) cpu->y = address;
    else cpu->y = rdmem(cpu, address);
  if (cpu->y == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->y & 0x80);
  }

void c6502_lsr(CPU6502* cpu, byte inst) {
  byte v;
  word address;
  if (inst == 0x4a) v = cpu->a;
  else {
    address = getEA(cpu, inst, 1);
    v = rdmem(cpu, address);
    }
  if ((v & 1) == 1) cpu->p |= FLAG_C;
    else cpu->p &= (~FLAG_C);
  v >>= 1;
  if (inst == 0x4a) cpu->a = v;
    else wrmem(cpu, address, v);
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (v & 0x80);
  }

void c6502_ora(CPU6502* cpu, byte inst) {
  word address;
  address = getEA(cpu, inst, 1);
  if (inst == 0x09) cpu->a |= address;
    else cpu->a |= rdmem(cpu, address);
  if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->a & 0x80);
  }

void c6502_rol(CPU6502* cpu, byte inst) {
  byte c;
  byte v;
  word address;
  if (inst == 0x2a) v = cpu->a;
  else {
    address = getEA(cpu, inst, 1);
    v = rdmem(cpu, address);
    }
  c = cpu->p & FLAG_C;
  if (v >= 0x80) cpu->p |= FLAG_C;
    else cpu->p &= (~FLAG_C);
  v <<= 1;
  if (c != 0) v |= 1;
  if (inst == 0x2a) cpu->a = v;
    else wrmem(cpu, address, v);
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (v & 0x80);
  }

void c6502_ror(CPU6502* cpu, byte inst) {
  byte c;
  byte v;
  word address;
  if (inst == 0x6a) v = cpu->a;
  else {
    address = getEA(cpu, inst, 1);
    v = rdmem(cpu, address);
    }
  c = cpu->p & FLAG_C;
  if ((v & 1) == 1) cpu->p |= FLAG_C;
    else cpu->p &= (~FLAG_C);
  v >>= 1;
  if (c != 0) v |= 0x80;
  if (inst == 0x6a) cpu->a = v;
    else wrmem(cpu, address, v);
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (v & 0x80);
  }

void c6502_sbc(CPU6502* cpu, byte arg) {
  int a,b,c;
  if (cpu->p & FLAG_D) {
    a = (((cpu->a & 0xf0) >> 4) * 10) + (cpu->a & 0x0f);
    b = (((arg & 0xf0) >> 4) * 10) + (arg & 0x0f);
    c = a - b;
    if ((cpu->p & FLAG_C) == 0) c--;
    if (c >= 0) {
      cpu->p |= FLAG_C;
      }
    else {
      cpu->p &= (~FLAG_C);
      c += 100;
      }
    cpu->a = (c / 10) << 4;
    cpu->a |= (c % 10);
    if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
    cpu->p &= (~FLAG_N);
    cpu->p &= (~FLAG_V);
    }
  else {
    arg = (~arg) + 1;
    cpu->p ^= 1;
    c6502_adc(cpu, arg);
    }
  }

void c6502_sta(CPU6502* cpu, byte inst) {
  word address;
  address = getEA(cpu, inst, 0);
  wrmem(cpu, address, cpu->a);
  }

void c6502_stx(CPU6502* cpu, byte inst) {
  word address;
  if (inst == 0x96) address = (fetch(cpu) + cpu->y) & 0xff;
  else {
    address = getEA(cpu, inst, 0);
    }
  wrmem(cpu, address, cpu->x);
  }

void c6502_sty(CPU6502* cpu, byte inst) {
  word address;
  address = getEA(cpu, inst, 0);
  wrmem(cpu, address, cpu->y);
  }

void c6502_op_00(CPU6502* cpu) {                 /* BRK */
  byte p;
  word address;
  cpu->clocks = 7;
  cpu->pc++;
  address = cpu->s;
  address += 0x100;
  cpu->s--;
  wrmem(cpu, address, (cpu->pc >> 8));
  address = cpu->s;
  address += 0x100;
  cpu->s--;
  wrmem(cpu, address, (cpu->pc & 0xff));
  p = cpu->p;
  address = cpu->s;
  address += 0x100;
  cpu->s--;
  wrmem(cpu, address, (cpu->p | FLAG_B));
  address = rdmem(cpu, 0xfffe);
  address |= (rdmem(cpu, 0xffff) << 8);
  cpu->pc = address;
  }

void c6502_op_01(CPU6502* cpu) {                 /* ORA (n,X) */
  cpu->clocks = 6;
  c6502_ora(cpu, 0x01);
  }

void c6502_op_04(CPU6502* cpu) {                 /* TSB n */
  byte v;
  word address;
  cpu->clocks = 5;
  address = fetch(cpu);
  v = cpu->a & rdmem(cpu, address);
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  v = v | cpu->a;
  wrmem(cpu, address, v);
  }

void c6502_op_05(CPU6502* cpu) {                 /* ORA n */
  cpu->clocks = 3;
  c6502_ora(cpu, 0x05);
  }

void c6502_op_06(CPU6502* cpu) {                 /* ASL n */
  cpu->clocks = 5;
  c6502_asl(cpu, 0x06);
  }

void c6502_op_08(CPU6502* cpu) {                 /* PHP */
  word address;
  cpu->clocks = 3;
  address = cpu->s--;
  address += 0x100;
  wrmem(cpu, address, cpu->p);
  }

void c6502_op_09(CPU6502* cpu) {                 /* ORA #n */
  cpu->clocks = 2;
  c6502_ora(cpu, 0x09);
  }

void c6502_op_0A(CPU6502* cpu) {                 /* ASL A */
  cpu->clocks = 2;
  c6502_asl(cpu, 0x0a);
  }

void c6502_op_0C(CPU6502* cpu) {                 /* TSB nn */
  byte v;
  word address;
  cpu->clocks = 6;
  address = fetch(cpu);
  address |= (fetch(cpu) << 8);
  v = cpu->a & rdmem(cpu, address);
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  v |= cpu->a;
  wrmem(cpu, address, v);
  }

void c6502_op_0D(CPU6502* cpu) {                 /* ORA nn */
  cpu->clocks = 4;
  c6502_ora(cpu, 0x0d);
  }

void c6502_op_0E(CPU6502* cpu) {                 /* ASL nn */
  cpu->clocks = 6;
  c6502_asl(cpu, 0x0e);
  }

void c6502_op_10(CPU6502* cpu) {                 /* BPL n */
  word address;
  address = fetch(cpu);
  if (address >= 0x80) address |= 0xff00;
  cpu->clocks = 2;
  if ((cpu->p & FLAG_N) == 0) {
    cpu->clocks++;
    cpu->pc += address;
    }
  }

void c6502_op_11(CPU6502* cpu) {                 /* ORA (n),Y */
  cpu->clocks = 5;
  c6502_ora(cpu, 0x11);
  }

void c6502_op_12(CPU6502* cpu) {                 /* ORA (n) */
  word address;
  cpu->clocks = 6;
  address = IndirectAddress(cpu);
  cpu->a |= rdmem(cpu, address);
  if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->a & 0x80);
  }

void c6502_op_14(CPU6502* cpu) {                 /* TRB n */
  byte v;
  word address;
  cpu->clocks = 5;
  address = fetch(cpu);
  v = cpu->a & rdmem(cpu, address);
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  v = v & (~cpu->a);
  wrmem(cpu, address, v);
  }

void c6502_op_15(CPU6502* cpu) {                 /* ORA n,X */
  cpu->clocks = 4;
  c6502_ora(cpu, 0x15);
  }

void c6502_op_16(CPU6502* cpu) {                 /* ASL n,X */
  cpu->clocks = 6;
  c6502_asl(cpu, 0x16);
  }

void c6502_op_18(CPU6502* cpu) {                 /* CLC */
  cpu->clocks = 2;
  cpu->p &= (~FLAG_C);
  }

void c6502_op_19(CPU6502* cpu) {                 /* ORA nn,Y */
  cpu->clocks = 4;
  c6502_ora(cpu, 0x19);
  }

void c6502_op_1A(CPU6502* cpu) {                 /* INA */
  cpu->clocks = 2;
  cpu->a++;
  if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->a & 0x80);
  }

void c6502_op_1C(CPU6502* cpu) {                 /* TRB nn */
  byte v;
  word address;
  cpu->clocks = 4;
  address = fetch(cpu);
  address |= (fetch(cpu) << 8);
  v = cpu->a & rdmem(cpu, address);
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  v &= (~cpu->a);
  wrmem(cpu, address, v);
  }

void c6502_op_1D(CPU6502* cpu) {                 /* ORA nn,X */
  cpu->clocks = 4;
  c6502_ora(cpu, 0x1d);
  }

void c6502_op_1E(CPU6502* cpu) {                 /* ASL nn,X */
  cpu->clocks = 7;
  c6502_asl(cpu, 0x1e);
  }

void c6502_op_20(CPU6502* cpu) {                 /* JSR nn */
  word address;
  cpu->clocks = 6;
  address = cpu->s;
  address += 0x100;
  cpu->s--;
  wrmem(cpu, address, ((cpu->pc+1) >> 8));
  address = cpu->s;
  address += 0x100;
  cpu->s--;
  wrmem(cpu, address, ((cpu->pc+1) & 0xff));
  address = fetch(cpu);
  address |= (fetch(cpu) << 8);
  cpu->pc = address;
  }

void c6502_op_21(CPU6502* cpu) {                 /* AND (n,X) */
  word address;
  cpu->clocks = 6;
  c6502_and(cpu, 0x21);
  }

void c6502_op_24(CPU6502* cpu) {                 /* BIT n */
  cpu->clocks = 3;
  c6502_bit(cpu, 0x24);
  }

void c6502_op_25(CPU6502* cpu) {                 /* AND n */
  word address;
  cpu->clocks = 3;
  c6502_and(cpu, 0x25);
  }

void c6502_op_26(CPU6502* cpu) {                 /* ROL n */
  cpu->clocks = 5;
  c6502_rol(cpu, 0x26);
  }

void c6502_op_28(CPU6502* cpu) {                 /* PLP */
  word address;
  cpu->clocks = 4;
  cpu->s++;
  address = cpu->s;
  address += 0x100;
  cpu->p = rdmem(cpu, address);
  }

void c6502_op_29(CPU6502* cpu) {                 /* AND #n */
  word address;
  cpu->clocks = 2;
  c6502_and(cpu, 0x29);
  }

void c6502_op_2A(CPU6502* cpu) {                 /* ROL A */
  cpu->clocks = 2;
  c6502_rol(cpu, 0x2a);
  }

void c6502_op_2C(CPU6502* cpu) {                 /* BIT nn */
  cpu->clocks = 4;
  c6502_bit(cpu, 0x2c);
  }

void c6502_op_2D(CPU6502* cpu) {                 /* AND nn */
  word address;
  cpu->clocks = 4;
  c6502_and(cpu, 0x2d);
  }

void c6502_op_2E(CPU6502* cpu) {                 /* ROL nn */
  cpu->clocks = 6;
  c6502_rol(cpu, 0x2e);
  }

void c6502_op_30(CPU6502* cpu) {                 /* BMI n */
  word address;
  address = fetch(cpu);
  if (address >= 0x80) address |= 0xff00;
  cpu->clocks = 2;
  if ((cpu->p & FLAG_N) != 0) {
    cpu->clocks++;
    cpu->pc += address;
    }
  }

void c6502_op_31(CPU6502* cpu) {                 /* AND (n),Y */
  word address;
  cpu->clocks = 5;
  c6502_and(cpu, 0x31);
  }

void c6502_op_32(CPU6502* cpu) {                 /* AND (n) */
  word address;
  cpu->clocks = 6;
  address = IndirectAddress(cpu);
  cpu->a &= rdmem(cpu, address);
  if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->a & 0x80);
  }

void c6502_op_34(CPU6502* cpu) {                 /* BIT n,X */
  byte v;
  word address;
  cpu->clocks = 6;
  address = (fetch(cpu) + cpu->x) & 0xff;
  v = rdmem(cpu, address);
  v = cpu->a & v;
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x3f) | (v & 0xc0);
  }

void c6502_op_35(CPU6502* cpu) {                 /* AND n,X */
  word address;
  cpu->clocks = 4;
  c6502_and(cpu, 0x35);
  }

void c6502_op_36(CPU6502* cpu) {                 /* ROL n,X */
  cpu->clocks = 6;
  c6502_rol(cpu, 0x36);
  }

void c6502_op_38(CPU6502* cpu) {                 /* SEC */
  cpu->clocks = 2;
  cpu->p |= FLAG_C;
  }

void c6502_op_39(CPU6502* cpu) {                 /* AND nn,Y */
  cpu->clocks = 4;
  c6502_and(cpu, 0x39);
  }

void c6502_op_3A(CPU6502* cpu) {                 /* DEA */
  cpu->clocks = 2;
  cpu->a--;
  if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->a & 0x80);
  }

void c6502_op_3C(CPU6502* cpu) {                 /* BIT nn,X */
  byte v;
  word address;
  word old;
  cpu->clocks = 4;
  address = fetch(cpu);
  address |= (fetch(cpu) << 8);
  old = address;
  address += cpu->x;
  if ((old & 0xff00) != (address & 0xff00)) cpu->clocks++;
  v = rdmem(cpu, address);
  v = cpu->a & v;
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x3f) | (v & 0xc0);
  }

void c6502_op_3D(CPU6502* cpu) {                 /* AND nn,X */
  cpu->clocks = 4;
  c6502_and(cpu, 0x3d);
  }

void c6502_op_3E(CPU6502* cpu) {                 /* ROL nn,X */
  cpu->clocks = 7;
  c6502_rol(cpu, 0x3e);
  }

void c6502_op_40(CPU6502* cpu) {                 /* RTI */
  word address;
  cpu->clocks = 6;
  cpu->s++;
  address = cpu->s;
  address += 0x100;
  cpu->p = rdmem(cpu, address) & (~FLAG_B);
  cpu->s++;
  address = cpu->s;
  address += 0x100;
  cpu->pc = rdmem(cpu, address);
  cpu->s++;
  address = cpu->s;
  address += 0x100;
  cpu->pc |= (rdmem(cpu, address) << 8);
  }

void c6502_op_41(CPU6502* cpu) {                 /* EOR (n,X) */
  cpu->clocks = 6;
  c6502_eor(cpu, 0x41);
  }

void c6502_op_45(CPU6502* cpu) {                 /* EOR n */
  cpu->clocks = 3;
  c6502_eor(cpu, 0x45);
  }

void c6502_op_46(CPU6502* cpu) {                 /* LSR n */
  cpu->clocks = 5;
  c6502_lsr(cpu, 0x46);
  }

void c6502_op_48(CPU6502* cpu) {                 /* PHA */
  word address;
  cpu->clocks = 3;
  address = cpu->s--;
  address += 0x100;
  wrmem(cpu, address, cpu->a);
  }

void c6502_op_49(CPU6502* cpu) {                 /* EOR #n */
  cpu->clocks = 2;
  c6502_eor(cpu, 0x49);
  }

void c6502_op_4A(CPU6502* cpu) {                 /* LSR A */
  cpu->clocks = 2;
  c6502_lsr(cpu, 0x4a);
  }

void c6502_op_4C(CPU6502* cpu) {                 /* JMP */
  word addr;
  cpu->clocks = 3;
  addr = fetch(cpu);
  addr |= (fetch(cpu) << 8);
  cpu->pc = addr;
  }

void c6502_op_4D(CPU6502* cpu) {                 /* EOR nn */
  cpu->clocks = 4;
  c6502_eor(cpu, 0x4d);
  }

void c6502_op_4E(CPU6502* cpu) {                 /* LSR nn */
  word address;
  cpu->clocks = 6;
  c6502_lsr(cpu, 0x4e);
  }

void c6502_op_50(CPU6502* cpu) {                 /* BVC n */
  word address;
  address = fetch(cpu);
  if (address >= 0x80) address |= 0xff00;
  cpu->clocks = 2;
  if ((cpu->p & FLAG_V) == 0) {
    cpu->clocks++;
    cpu->pc += address;
    }
  }

void c6502_op_51(CPU6502* cpu) {                 /* EOR (n),Y */
  cpu->clocks = 5;
  c6502_eor(cpu, 0x51);
  }

void c6502_op_52(CPU6502* cpu) {                 /* EOR (n) */
  word address;
  cpu->clocks = 6;
  address = IndirectAddress(cpu);
  cpu->a ^= rdmem(cpu, address);
  if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->a & 0x80);
  }

void c6502_op_55(CPU6502* cpu) {                 /* EOR n,X */
  cpu->clocks = 4;
  c6502_eor(cpu, 0x55);
  }

void c6502_op_56(CPU6502* cpu) {                 /* LSR n,X */
  cpu->clocks = 6;
  c6502_lsr(cpu, 0x56);
  }

void c6502_op_58(CPU6502* cpu) {                 /* CLI */
  cpu->clocks = 2;
  cpu->p &= (~FLAG_I);
  }

void c6502_op_59(CPU6502* cpu) {                 /* EOR nn,Y */
  cpu->clocks = 4;
  c6502_eor(cpu, 0x59);
  }

void c6502_op_5A(CPU6502* cpu) {                 /* PHY */
  word address;
  cpu->clocks = 3;
  address = cpu->s--;
  address += 0x100;
  wrmem(cpu, address, cpu->y);
  }

void c6502_op_5D(CPU6502* cpu) {                 /* EOR nn,X */
  cpu->clocks = 4;
  c6502_eor(cpu, 0x5d);
  }

void c6502_op_5E(CPU6502* cpu) {                 /* LSR nn,X */
  cpu->clocks = 7;
  c6502_lsr(cpu, 0x5e);
  }

void c6502_op_60(CPU6502* cpu) {                 /* RTS */
  word address;
  cpu->clocks = 6;
  cpu->s++;
  address = cpu->s;
  address += 0x100;
  cpu->pc = rdmem(cpu, address);
  cpu->s++;
  address = cpu->s;
  address += 0x100;
  cpu->pc |= (rdmem(cpu, address) << 8);
  cpu->pc++;
  }

void c6502_op_61(CPU6502* cpu) {                 /* ADC (n,X) */
  word address;
  cpu->clocks = 6;
  address = IndexedIndirectXAddress(cpu);
  c6502_adc(cpu, rdmem(cpu, address));
  }

void c6502_op_64(CPU6502* cpu) {                 /* STZ n */
  word address;
  cpu->clocks = 3;
  address = fetch(cpu);
  wrmem(cpu, address, 0);
  }

void c6502_op_65(CPU6502* cpu) {                 /* ADC n */
  word address;
  cpu->clocks = 3;
  address = fetch(cpu);
  c6502_adc(cpu, rdmem(cpu, address));
  }

void c6502_op_66(CPU6502* cpu) {                 /* ROR n */
  cpu->clocks = 5;
  c6502_ror(cpu, 0x66);
  }

void c6502_op_68(CPU6502* cpu) {                 /* PLA */
  word address;
  cpu->clocks = 4;
  cpu->s++;
  address = cpu->s;
  address += 0x100;
  cpu->a = rdmem(cpu, address);
  if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->a & 0x80);
  }

void c6502_op_69(CPU6502* cpu) {                 /* ADC #n */
  word address;
  cpu->clocks = 2;
  c6502_adc(cpu, fetch(cpu));
  }

void c6502_op_6A(CPU6502* cpu) {                 /* ROR A */
  cpu->clocks = 2;
  c6502_ror(cpu, 0x6a);
  }

void c6502_op_6C(CPU6502* cpu) {                 /* JMP (nn) */
  word addr;
  cpu->clocks = 5;
  addr = fetch(cpu);
  addr |= (fetch(cpu) << 8);
  cpu->pc = rdmem(cpu, addr);
  addr++;
  cpu->pc |= (rdmem(cpu, addr) << 8);
  }

void c6502_op_6D(CPU6502* cpu) {                 /* ADC nn */
  word address;
  cpu->clocks = 4;
  address = fetch(cpu);
  address |= (fetch(cpu) << 8);
  c6502_adc(cpu, rdmem(cpu, address));
  }

void c6502_op_6E(CPU6502* cpu) {                 /* ROR nn */
  cpu->clocks = 6;
  c6502_ror(cpu, 0x6e);
  }

void c6502_op_70(CPU6502* cpu) {                 /* BVS n */
  word address;
  address = fetch(cpu);
  if (address >= 0x80) address |= 0xff00;
  cpu->clocks = 2;
  if ((cpu->p & FLAG_V) != 0) {
    cpu->clocks++;
    cpu->pc += address;
    }
  }

void c6502_op_71(CPU6502* cpu) {                 /* ADC (n),Y */
  word address;
  cpu->clocks = 5;
  if ((cpu->pc & 0xff) == 0xff) cpu->clocks++;
  address = IndexedIndirectYAddress(cpu, 1);
  c6502_adc(cpu, rdmem(cpu, address));
  }

void c6502_op_72(CPU6502* cpu) {                 /* ADC (n) */
  word address;
  cpu->clocks = 6;
  address = IndirectAddress(cpu);
  c6502_adc(cpu, rdmem(cpu, address));
  }

void c6502_op_74(CPU6502* cpu) {                 /* STZ n,X */
  word address;
  cpu->clocks = 4;
  address = (fetch(cpu) + cpu->x) & 0xff;
  wrmem(cpu, address, 0);
  }

void c6502_op_75(CPU6502* cpu) {                 /* ADC n,X */
  word address;
  cpu->clocks = 4;
  address = (fetch(cpu) + cpu->x) & 0xff;
  c6502_adc(cpu, rdmem(cpu, address));
  }

void c6502_op_76(CPU6502* cpu) {                 /* ROR n,X */
  cpu->clocks = 6;
  c6502_ror(cpu, 0x76);
  }

void c6502_op_78(CPU6502* cpu) {                 /* SEI */
  cpu->clocks = 2;
  cpu->p |= FLAG_I;
  }

void c6502_op_79(CPU6502* cpu) {                 /* ADC nn,Y */
  word address;
  word old;
  cpu->clocks = 4;
  address = fetch(cpu);
  address |= (fetch(cpu) << 8);
  old = address;
  address += cpu->y;
  if ((old & 0xff00) != (address & 0xff00)) cpu->clocks++;
  c6502_adc(cpu, rdmem(cpu, address));
  }

void c6502_op_7A(CPU6502* cpu) {                 /* PLY */
  word address;
  cpu->clocks = 4;
  cpu->s++;
  address = cpu->s;
  address += 0x100;
  cpu->y = rdmem(cpu, address);
  if (cpu->y == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->y & 0x80);
  }

void c6502_op_7C(CPU6502* cpu) {                 /* JMP (nn,X) */
  word addr;
  cpu->clocks = 5;
  addr = fetch(cpu);
  addr = (fetch(cpu) << 8);
  addr += cpu->x;
  cpu->pc = rdmem(cpu, addr);
  addr++;
  cpu->pc |= (rdmem(cpu, addr) << 8);
  }

void c6502_op_7D(CPU6502* cpu) {                 /* ADC nn,X */
  word address;
  word old;
  cpu->clocks = 4;
  address = fetch(cpu);
  address |= (fetch(cpu) << 8);
  old = address;
  address += cpu->x;
  if ((old & 0xff00) != (address & 0xff00)) cpu->clocks++;
  c6502_adc(cpu, rdmem(cpu, address));
  }

void c6502_op_7E(CPU6502* cpu) {                 /* ROR nn,X */
  cpu->clocks = 7;
  c6502_ror(cpu, 0x7e);
  }

void c6502_op_80(CPU6502* cpu) {                 /* BRA n */
  word address;
  address = fetch(cpu);
  if (address >= 0x80) address |= 0xff00;
  cpu->clocks = 3;
  cpu->pc += address;
  }

void c6502_op_81(CPU6502* cpu) {                 /* STA (n,X) */
  cpu->clocks = 6;
  c6502_sta(cpu, 0x81);
  }

void c6502_op_84(CPU6502* cpu) {                 /* STY n */
  cpu->clocks = 3;
  c6502_sty(cpu, 0x84);
  }

void c6502_op_85(CPU6502* cpu) {                 /* STA n */
  cpu->clocks = 3;
  c6502_sta(cpu, 0x85);
  }

void c6502_op_86(CPU6502* cpu) {                 /* STX n */
  cpu->clocks = 3;
  c6502_stx(cpu, 0x86);
  }

void c6502_op_88(CPU6502* cpu) {                 /* DEY */
  cpu->clocks = 2;
  cpu->y--;
  if (cpu->y == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->y & 0x80);
  }

void c6502_op_89(CPU6502* cpu) {                 /* BIT #n */
  byte v;
  cpu->clocks = 2;
  v = fetch(cpu);
  v = cpu->a & v;
  if (v == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  }

void c6502_op_8A(CPU6502* cpu) {                 /* TXA */
  cpu->clocks = 2;
  cpu->a = cpu->x;
  if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->a & 0x80);
  }

void c6502_op_8C(CPU6502* cpu) {                 /* STY nn */
  cpu->clocks = 4;
  c6502_sty(cpu, 0x8c);
  }

void c6502_op_8D(CPU6502* cpu) {                 /* STA nn */
  cpu->clocks = 4;
  c6502_sta(cpu, 0x8d);
  }

void c6502_op_8E(CPU6502* cpu) {                 /* STX nn */
  cpu->clocks = 4;
  c6502_stx(cpu, 0x8e);
  }

void c6502_op_90(CPU6502* cpu) {                 /* BCC n */
  word address;
  address = fetch(cpu);
  if (address >= 0x80) address |= 0xff00;
  cpu->clocks = 2;
  if ((cpu->p & FLAG_C) == 0) {
    cpu->clocks++;
    cpu->pc += address;
    }
  }

void c6502_op_91(CPU6502* cpu) {                 /* STA (n),Y */
  cpu->clocks = 6;
  c6502_sta(cpu, 0x91);
  }

void c6502_op_92(CPU6502* cpu) {                 /* STA (n) */
  word address;
  cpu->clocks = 6;
  address = IndirectAddress(cpu);
  wrmem(cpu, address, cpu->a);
  }

void c6502_op_94(CPU6502* cpu) {                 /* STY n,X */
  cpu->clocks = 4;
  c6502_sty(cpu, 0x94);
  }

void c6502_op_95(CPU6502* cpu) {                 /* STA n,X */
  cpu->clocks = 4;
  c6502_sta(cpu, 0x95);
  }

void c6502_op_96(CPU6502* cpu) {                 /* STX n,X */
  cpu->clocks = 4;
  c6502_stx(cpu, 0x96);
  }

void c6502_op_98(CPU6502* cpu) {                 /* TYA */
  cpu->clocks = 2;
  cpu->a = cpu->y;
  if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->a & 0x80);
  }

void c6502_op_99(CPU6502* cpu) {                 /* STA nn,Y */
  cpu->clocks = 5;
  c6502_sta(cpu, 0x99);
  }

void c6502_op_9A(CPU6502* cpu) {                 /* TXS */
  cpu->clocks = 2;
  cpu->s = cpu->x;
  }

void c6502_op_9C(CPU6502* cpu) {                 /* STZ nn */
  word address;
  cpu->clocks = 4;
  address = fetch(cpu);
  address |= (fetch(cpu) << 8);
  wrmem(cpu, address, 0);
  }

void c6502_op_9D(CPU6502* cpu) {                 /* STA nn,X */
  cpu->clocks = 5;
  c6502_sta(cpu, 0x9d);
  }

void c6502_op_9E(CPU6502* cpu) {                 /* STZ nn,X */
  word address;
  cpu->clocks = 5;
  address = fetch(cpu);
  address |= (fetch(cpu) << 8);
  address += cpu->x;
  wrmem(cpu, address, 0);
  }

void c6502_op_A0(CPU6502* cpu) {                 /* LDY #n */
  cpu->clocks = 2;
  c6502_ldy(cpu, 0xa0);
  }

void c6502_op_A1(CPU6502* cpu) {                 /* LDA (n,X) */
  cpu->clocks = 6;
  c6502_lda(cpu, 0xa1);
  }

void c6502_op_A2(CPU6502* cpu) {                 /* LDX #n */
  cpu->clocks = 2;
  c6502_ldx(cpu, 0xa2);
  }

void c6502_op_A4(CPU6502* cpu) {                 /* LDY n */
  cpu->clocks = 3;
  c6502_ldy(cpu, 0xa4);
  }

void c6502_op_A5(CPU6502* cpu) {                 /* LDA n */
  cpu->clocks = 3;
  c6502_lda(cpu, 0xa5);
  }

void c6502_op_A6(CPU6502* cpu) {                 /* LDX n */
  cpu->clocks = 3;
  c6502_ldx(cpu, 0xa4);
  }

void c6502_op_A8(CPU6502* cpu) {                 /* TAY */
  cpu->clocks = 2;
  cpu->y = cpu->a;
  if (cpu->y == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->y & 0x80);
  }

void c6502_op_A9(CPU6502* cpu) {                 /* LDA #n */
  cpu->clocks = 2;
  c6502_lda(cpu, 0xa9);
  }

void c6502_op_AA(CPU6502* cpu) {                 /* TAX */
  cpu->clocks = 2;
  cpu->x = cpu->a;
  if (cpu->x == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->x & 0x80);
  }

void c6502_op_AC(CPU6502* cpu) {                 /* LDY nn */
  cpu->clocks = 4;
  c6502_ldy(cpu, 0xac);
  }

void c6502_op_AD(CPU6502* cpu) {                 /* LDA nn */
  cpu->clocks = 4;
  c6502_lda(cpu, 0xad);
  }

void c6502_op_AE(CPU6502* cpu) {                 /* LDX nn */
  cpu->clocks = 4;
  c6502_ldx(cpu, 0xae);
  }

void c6502_op_B0(CPU6502* cpu) {                 /* BCS n */
  word address;
  address = fetch(cpu);
  if (address >= 0x80) address |= 0xff00;
  cpu->clocks = 2;
  if ((cpu->p & FLAG_C) != 0) {
    cpu->clocks++;
    cpu->pc += address;
    }
  }

void c6502_op_B1(CPU6502* cpu) {                 /* LDA (n),Y */
  cpu->clocks = 5;
  c6502_lda(cpu, 0xb1);
  }

void c6502_op_B2(CPU6502* cpu) {                 /* LDA (n) */
  word address;
  cpu->clocks = 5;
  address = IndirectAddress(cpu);
  cpu->a = rdmem(cpu, address);
  if (cpu->a == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->a & 0x80);
  }

void c6502_op_B4(CPU6502* cpu) {                 /* LDY n,X */
  cpu->clocks = 4;
  c6502_ldy(cpu, 0xb4);
  }

void c6502_op_B5(CPU6502* cpu) {                 /* LDA n,X */
  cpu->clocks = 4;
  c6502_lda(cpu, 0xb5);
  }

void c6502_op_B6(CPU6502* cpu) {                 /* LDX n,Y */
  cpu->clocks = 4;
  c6502_ldx(cpu, 0xb6);
  }

void c6502_op_B8(CPU6502* cpu) {                 /* CLV */
  cpu->clocks = 2;
  cpu->p &= (~FLAG_V);
  }

void c6502_op_B9(CPU6502* cpu) {                 /* LDA nn,Y */
  cpu->clocks = 4;
  c6502_lda(cpu, 0xb9);
  }

void c6502_op_BA(CPU6502* cpu) {                 /* TSX */
  cpu->clocks = 2;
  cpu->x = cpu->s;
  if (cpu->x == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->x & 0x80);
  }

void c6502_op_BC(CPU6502* cpu) {                 /* LDY nn,X */
  cpu->clocks = 4;
  c6502_ldy(cpu, 0xbc);
  }

void c6502_op_BD(CPU6502* cpu) {                 /* LDA nn,X */
  cpu->clocks = 4;
  c6502_lda(cpu, 0xbd);
  }

void c6502_op_BE(CPU6502* cpu) {                 /* LDX nn,Y */
  cpu->clocks = 4;
  c6502_ldx(cpu, 0xbe);
  }

void c6502_op_C0(CPU6502* cpu) {                 /* CPY #n */
  cpu->clocks = 2;
  c6502_cpy(cpu, 0xc0);
  }

void c6502_op_C1(CPU6502* cpu) {                 /* CMP (n,X) */
  cpu->clocks = 6;
  c6502_cmp(cpu, 0xc1);
  }

void c6502_op_C4(CPU6502* cpu) {                 /* CPY n */
  cpu->clocks = 3;
  c6502_cpy(cpu, 0xc4);
  }

void c6502_op_C5(CPU6502* cpu) {                 /* CMP n */
  cpu->clocks = 3;
  c6502_cmp(cpu, 0xc5);
  }

void c6502_op_C6(CPU6502* cpu) {                 /* DEC n */
  cpu->clocks = 5;
  c6502_dec(cpu, 0xc6);
  }

void c6502_op_C8(CPU6502* cpu) {                 /* INY */
  cpu->clocks = 2;
  cpu->y++;
  if (cpu->y == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->y & 0x80);
  }

void c6502_op_C9(CPU6502* cpu) {                 /* CMP #n */
  cpu->clocks = 2;
  c6502_cmp(cpu, 0xc9);
  }

void c6502_op_CA(CPU6502* cpu) {                 /* DEX */
  cpu->clocks = 2;
  cpu->x--;
  if (cpu->x == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->x & 0x80);
  }

void c6502_op_CC(CPU6502* cpu) {                 /* CPY nn */
  cpu->clocks = 4;
  c6502_cpy(cpu, 0xcc);
  }

void c6502_op_CD(CPU6502* cpu) {                 /* CMP nn */
  cpu->clocks = 4;
  c6502_cmp(cpu, 0xcd);
  }

void c6502_op_CE(CPU6502* cpu) {                 /* DEC nn */
  cpu->clocks = 6;
  c6502_dec(cpu, 0xce);
  }

void c6502_op_D0(CPU6502* cpu) {                 /* BNE n */
  word address;
  address = fetch(cpu);
  if (address >= 0x80) address |= 0xff00;
  cpu->clocks = 2;
  if ((cpu->p & FLAG_Z) == 0) {
    cpu->clocks++;
    cpu->pc += address;
    }
  }

void c6502_op_D1(CPU6502* cpu) {                 /* CMP (n),Y */
  cpu->clocks = 5;
  c6502_cmp(cpu, 0xd1);
  }

void c6502_op_D2(CPU6502* cpu) {                 /* CMP (n) */
  word address;
  cpu->clocks = 6;
  address = IndirectAddress(cpu);
  c6502_cp(cpu, cpu->a, rdmem(cpu, address));
  }

void c6502_op_D5(CPU6502* cpu) {                 /* CMP n,X */
  cpu->clocks = 4;
  c6502_cmp(cpu, 0xd5);
  }

void c6502_op_D6(CPU6502* cpu) {                 /* DEC n,X */
  cpu->clocks = 6;
  c6502_dec(cpu, 0xd6);
  }

void c6502_op_D8(CPU6502* cpu) {                 /* CLD */
  cpu->clocks = 2;
  cpu->p &= (~FLAG_D);
  }

void c6502_op_D9(CPU6502* cpu) {                 /* CMP nn,Y */
  cpu->clocks = 4;
  c6502_cmp(cpu, 0xd9);
  }

void c6502_op_DA(CPU6502* cpu) {                 /* PHX */
  word address;
  cpu->clocks = 3;
  address = cpu->s--;
  address += 0x100;
  wrmem(cpu, address, cpu->x);
  }

void c6502_op_DD(CPU6502* cpu) {                 /* CMP nn,X */
  cpu->clocks = 4;
  c6502_cmp(cpu, 0xdd);
  }

void c6502_op_DE(CPU6502* cpu) {                 /* DEC nn,X */
  cpu->clocks = 7;
  c6502_dec(cpu, 0xde);
  }

void c6502_op_E0(CPU6502* cpu) {                 /* CPX #n */
  cpu->clocks = 2;
  c6502_cpx(cpu, 0xe0);
  }

void c6502_op_E1(CPU6502* cpu) {                 /* SBC (n,X) */
  word address;
  cpu->clocks = 6;
  address = IndexedIndirectXAddress(cpu);
  c6502_sbc(cpu, rdmem(cpu, address));
  }

void c6502_op_E4(CPU6502* cpu) {                 /* CPX n */
  cpu->clocks = 3;
  c6502_cpx(cpu, 0xe4);
  }

void c6502_op_E5(CPU6502* cpu) {                 /* SBC n */
  word address;
  cpu->clocks = 3;
  address = fetch(cpu);
  c6502_sbc(cpu, rdmem(cpu, address));
  }

void c6502_op_E6(CPU6502* cpu) {                 /* INC n */
  cpu->clocks = 5;
  c6502_inc(cpu, 0xe6);
  }

void c6502_op_E8(CPU6502* cpu) {                 /* INX */
  cpu->clocks = 2;
  cpu->x++;
  if (cpu->x == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->x & 0x80);
  }

void c6502_op_E9(CPU6502* cpu) {                 /* SBC #n */
  word address;
  cpu->clocks = 2;
  c6502_sbc(cpu, fetch(cpu));
  }

void c6502_op_EA(CPU6502* cpu) {                 /* NOP */
  cpu->clocks = 2;
  }

void c6502_op_EC(CPU6502* cpu) {                 /* CPX nn */
  cpu->clocks = 4;
  c6502_cpx(cpu, 0xec);
  }

void c6502_op_ED(CPU6502* cpu) {                 /* SBC nn */
  word address;
  cpu->clocks = 4;
  address = fetch(cpu);
  address |= (fetch(cpu) << 8);
  c6502_sbc(cpu, rdmem(cpu, address));
  }

void c6502_op_EE(CPU6502* cpu) {                 /* INC nn */
  cpu->clocks = 6;
  c6502_inc(cpu, 0xee);
  }

void c6502_op_F0(CPU6502* cpu) {                 /* BEQ n */
  word address;
  address = fetch(cpu);
  if (address >= 0x80) address |= 0xff00;
  cpu->clocks = 2;
  if ((cpu->p & FLAG_Z) != 0) {
    cpu->clocks++;
    cpu->pc += address;
    }
  }

void c6502_op_F1(CPU6502* cpu) {                 /* SBC (n),Y */
  word address;
  cpu->clocks = 5;
  if ((cpu->pc & 0xff) == 0xff) cpu->clocks++;
  address = IndexedIndirectYAddress(cpu, 1);
  c6502_sbc(cpu, rdmem(cpu, address));
  }

void c6502_op_F2(CPU6502* cpu) {                 /* SBC (n) */
  word address;
  cpu->clocks = 6;
  address = IndirectAddress(cpu);
  c6502_sbc(cpu, rdmem(cpu, address));
  }

void c6502_op_F5(CPU6502* cpu) {                 /* SBC n,X */
  word address;
  cpu->clocks = 4;
  address = (fetch(cpu) + cpu->x) & 0xff;
  c6502_sbc(cpu, rdmem(cpu, address));
  }

void c6502_op_F6(CPU6502* cpu) {                 /* INC n,X */
  cpu->clocks = 6;
  c6502_inc(cpu, 0xf6);
  }

void c6502_op_F8(CPU6502* cpu) {                 /* SED */
  cpu->clocks = 2;
  cpu->p |= FLAG_D;
  }

void c6502_op_F9(CPU6502* cpu) {                 /* SBC nn,Y */
  word address;
  word old;
  cpu->clocks = 4;
  address = fetch(cpu);
  address |= (fetch(cpu) << 8);
  old = address;
  address += cpu->y;
  if ((old & 0xff00) != (address & 0xff00)) cpu->clocks++;
  c6502_sbc(cpu, rdmem(cpu, address));
  }

void c6502_op_FA(CPU6502* cpu) {                 /* PLX */
  word address;
  cpu->clocks = 4;
  cpu->s++;
  address = cpu->s;
  address += 0x100;
  cpu->x = rdmem(cpu, address);
  if (cpu->x == 0) cpu->p |= FLAG_Z; else cpu->p &= (~FLAG_Z);
  cpu->p = (cpu->p & 0x7f) | (cpu->x & 0x80);
  }

void c6502_op_FD(CPU6502* cpu) {                 /* SBC nn,X */
  word address;
  word old;
  cpu->clocks = 4;
  address = fetch(cpu);
  address |= (fetch(cpu) << 8);
  old = address;
  address += cpu->x;
  if ((old & 0xff00) != (address & 0xff00)) cpu->clocks++;
  c6502_sbc(cpu, rdmem(cpu, address));
  }

void c6502_op_FE(CPU6502* cpu) {                 /* INC nn,X */
  cpu->clocks = 7;
  c6502_inc(cpu, 0xfe);
  }

void c65c02_setup() {
  op[0x04] = c6502_op_04;                        /* TSB n */
  op[0x0c] = c6502_op_0C;                        /* TSB nn */
  op[0x12] = c6502_op_12;                        /* ORA (n) */
  op[0x14] = c6502_op_14;                        /* TRB n */
  op[0x1a] = c6502_op_1A;                        /* INA */
  op[0x1c] = c6502_op_1C;                        /* TRB nn */
  op[0x32] = c6502_op_32;                        /* AND (n) */
  op[0x34] = c6502_op_34;                        /* BIT n,X */
  op[0x3a] = c6502_op_3A;                        /* DEA */
  op[0x3c] = c6502_op_3C;                        /* BIT nn,X */
  op[0x52] = c6502_op_52;                        /* EOR (n) */
  op[0x5a] = c6502_op_5A;                        /* PHY */
  op[0x64] = c6502_op_64;                        /* STZ n */
  op[0x72] = c6502_op_72;                        /* ADC (n) */
  op[0x74] = c6502_op_74;                        /* STZ n,X */
  op[0x7a] = c6502_op_7A;                        /* PLY */
  op[0x7c] = c6502_op_7C;                        /* JMP (nn,X) */
  op[0x80] = c6502_op_80;                        /* BRA n */
  op[0x89] = c6502_op_89;                        /* BIT #n */
  op[0x92] = c6502_op_92;                        /* STA (n) */
  op[0x9c] = c6502_op_9C;                        /* STZ nn */
  op[0x9e] = c6502_op_9E;                        /* STZ nn,X */
  op[0xb2] = c6502_op_B2;                        /* LDA (n) */
  op[0xd2] = c6502_op_D2;                        /* CMP (n) */
  op[0xda] = c6502_op_DA;                        /* PHX */
  op[0xf2] = c6502_op_F2;                        /* SBC (n) */
  op[0xfa] = c6502_op_FA;                        /* PLX */
  }

void c6502_setup() {
  int i;
  for (i=0; i<256; i++) op[i] = NULL;
  op[0x00] = c6502_op_00;                        /* BRK */
  op[0x01] = c6502_op_01;                        /* ORA (n,X) */
  op[0x05] = c6502_op_05;                        /* ORA n */
  op[0x06] = c6502_op_06;                        /* ASL n */
  op[0x08] = c6502_op_08;                        /* PHP */
  op[0x09] = c6502_op_09;                        /* ORA #n */
  op[0x0a] = c6502_op_0A;                        /* ASL A */
  op[0x0d] = c6502_op_0D;                        /* ORA nn */
  op[0x0e] = c6502_op_0E;                        /* ASL nn */
  op[0x10] = c6502_op_10;                        /* BPL n */
  op[0x11] = c6502_op_11;                        /* ORA (n),Y */
  op[0x15] = c6502_op_15;                        /* ORA n,X */
  op[0x16] = c6502_op_16;                        /* ASL n,X */
  op[0x18] = c6502_op_18;                        /* CLC */
  op[0x19] = c6502_op_19;                        /* ORA nn,Y */
  op[0x1d] = c6502_op_1D;                        /* ORA nn,X */
  op[0x1e] = c6502_op_1E;                        /* ASL nn,X */
  op[0x20] = c6502_op_20;                        /* JSR nn */
  op[0x21] = c6502_op_21;                        /* AND (n,X) */
  op[0x24] = c6502_op_24;                        /* BIT n */
  op[0x25] = c6502_op_25;                        /* AND n */
  op[0x26] = c6502_op_26;                        /* ROL n */
  op[0x28] = c6502_op_28;                        /* PLP */
  op[0x29] = c6502_op_29;                        /* AND #n */
  op[0x2a] = c6502_op_2A;                        /* ROL A */
  op[0x2c] = c6502_op_2C;                        /* BIT nn */
  op[0x2d] = c6502_op_2D;                        /* AND nn */
  op[0x2e] = c6502_op_2E;                        /* ROL nn */
  op[0x30] = c6502_op_30;                        /* BMI n */
  op[0x31] = c6502_op_31;                        /* AND (n),Y */
  op[0x35] = c6502_op_35;                        /* AND n,X */
  op[0x36] = c6502_op_36;                        /* ROL n,X */
  op[0x38] = c6502_op_38;                        /* SEC */
  op[0x39] = c6502_op_39;                        /* AND nn,Y */
  op[0x3d] = c6502_op_3D;                        /* AND nn,X */
  op[0x3e] = c6502_op_3E;                        /* ROL nn,X */
  op[0x40] = c6502_op_40;                        /* RTI */
  op[0x41] = c6502_op_41;                        /* EOR (n,X) */
  op[0x45] = c6502_op_45;                        /* EOR n */
  op[0x46] = c6502_op_46;                        /* LSR n */
  op[0x48] = c6502_op_48;                        /* PHA */
  op[0x49] = c6502_op_49;                        /* EOR #n */
  op[0x4a] = c6502_op_4A;                        /* LSR A */
  op[0x4c] = c6502_op_4C;                        /* JMP */
  op[0x4d] = c6502_op_4D;                        /* EOR nn */
  op[0x4e] = c6502_op_4E;                        /* LSR nn */
  op[0x50] = c6502_op_50;                        /* BVC n */
  op[0x51] = c6502_op_51;                        /* EOR (n),Y */
  op[0x55] = c6502_op_55;                        /* EOR n,X */
  op[0x56] = c6502_op_56;                        /* LSR n,X */
  op[0x58] = c6502_op_58;                        /* CLI */
  op[0x59] = c6502_op_59;                        /* EOR nn,Y */
  op[0x5d] = c6502_op_5D;                        /* EOR nn,X */
  op[0x5e] = c6502_op_5E;                        /* LSR nn,X */
  op[0x60] = c6502_op_60;                        /* RTS */
  op[0x61] = c6502_op_61;                        /* ADC (n,X) */
  op[0x65] = c6502_op_65;                        /* ADC n */
  op[0x66] = c6502_op_66;                        /* ROR n */
  op[0x68] = c6502_op_68;                        /* PLA */
  op[0x69] = c6502_op_69;                        /* ADC #n */
  op[0x6a] = c6502_op_6A;                        /* ROR A */
  op[0x6c] = c6502_op_6C;                        /* JMP (nn) */
  op[0x6d] = c6502_op_6D;                        /* ADC nn */
  op[0x6e] = c6502_op_6E;                        /* ROR nn */
  op[0x70] = c6502_op_70;                        /* BVS n */
  op[0x71] = c6502_op_71;                        /* ADC (n),Y */
  op[0x75] = c6502_op_75;                        /* ADC n,X */
  op[0x76] = c6502_op_76;                        /* ROR n,X */
  op[0x78] = c6502_op_78;                        /* SEI */
  op[0x79] = c6502_op_79;                        /* ADC nn,Y */
  op[0x7d] = c6502_op_7D;                        /* ADC nn,X */
  op[0x7e] = c6502_op_7E;                        /* ROR nn,X */
  op[0x81] = c6502_op_81;                        /* STA (n,X) */
  op[0x84] = c6502_op_84;                        /* STY n */
  op[0x85] = c6502_op_85;                        /* STA n */
  op[0x86] = c6502_op_86;                        /* STX n */
  op[0x88] = c6502_op_88;                        /* DEY */
  op[0x8a] = c6502_op_8A;                        /* TXA */
  op[0x8c] = c6502_op_8C;                        /* STY nn */
  op[0x8d] = c6502_op_8D;                        /* STA nn */
  op[0x8e] = c6502_op_8E;                        /* STX nn */
  op[0x90] = c6502_op_90;                        /* BCC n */
  op[0x91] = c6502_op_91;                        /* STA (n),Y */
  op[0x94] = c6502_op_94;                        /* STY n,X */
  op[0x95] = c6502_op_95;                        /* STA n,X */
  op[0x96] = c6502_op_96;                        /* STX n,X */
  op[0x98] = c6502_op_98;                        /* TYA */
  op[0x99] = c6502_op_99;                        /* STA nn,Y */
  op[0x9a] = c6502_op_9A;                        /* TXS */
  op[0x9d] = c6502_op_9D;                        /* STA nn,X */
  op[0xa0] = c6502_op_A0;                        /* LDY #n */
  op[0xa1] = c6502_op_A1;                        /* LDA (n,X) */
  op[0xa2] = c6502_op_A2;                        /* LDX #n */
  op[0xa4] = c6502_op_A4;                        /* LDY n */
  op[0xa5] = c6502_op_A5;                        /* LDA n */
  op[0xa6] = c6502_op_A6;                        /* LDX n */
  op[0xa8] = c6502_op_A8;                        /* TAY */
  op[0xa9] = c6502_op_A9;                        /* LDA #n */
  op[0xaa] = c6502_op_AA;                        /* TAX */
  op[0xac] = c6502_op_AC;                        /* LDY nn */
  op[0xad] = c6502_op_AD;                        /* LDA nn */
  op[0xae] = c6502_op_AE;                        /* LDX nn */
  op[0xb0] = c6502_op_B0;                        /* BCS n */
  op[0xb1] = c6502_op_B1;                        /* LDA (n),Y */
  op[0xb4] = c6502_op_B4;                        /* LDY n,X */
  op[0xb5] = c6502_op_B5;                        /* LDA n,X */
  op[0xb6] = c6502_op_B6;                        /* LDX n,Y */
  op[0xb8] = c6502_op_B8;                        /* CLV */
  op[0xb9] = c6502_op_B9;                        /* LDA nn,Y */
  op[0xba] = c6502_op_BA;                        /* TSX */
  op[0xbc] = c6502_op_BC;                        /* LDY nn,X */
  op[0xbd] = c6502_op_BD;                        /* LDA nn,X */
  op[0xbe] = c6502_op_BE;                        /* LDX nn,Y */
  op[0xc0] = c6502_op_C0;                        /* CPY #n */
  op[0xc1] = c6502_op_C1;                        /* CMP (n,X) */
  op[0xc4] = c6502_op_C4;                        /* CPY n */
  op[0xc5] = c6502_op_C5;                        /* CMP n */
  op[0xc6] = c6502_op_C6;                        /* DEC n */
  op[0xc8] = c6502_op_C8;                        /* INY */
  op[0xc9] = c6502_op_C9;                        /* CMP #n */
  op[0xca] = c6502_op_CA;                        /* DEX */
  op[0xcc] = c6502_op_CC;                        /* CPY nn */
  op[0xcd] = c6502_op_CD;                        /* CMP nn */
  op[0xce] = c6502_op_CE;                        /* DEC nn */
  op[0xd0] = c6502_op_D0;                        /* BNE n */
  op[0xd1] = c6502_op_D1;                        /* CMP (n),Y */
  op[0xd5] = c6502_op_D5;                        /* CMP n,X */
  op[0xd6] = c6502_op_D6;                        /* DEC n,X */
  op[0xd8] = c6502_op_D8;                        /* CLD */
  op[0xd9] = c6502_op_D9;                        /* CMP nn,Y */
  op[0xdd] = c6502_op_DD;                        /* CMP nn,X */
  op[0xde] = c6502_op_DE;                        /* DEC nn,X */
  op[0xe0] = c6502_op_E0;                        /* CPX #n */
  op[0xe1] = c6502_op_E1;                        /* SBC (n,X) */
  op[0xe4] = c6502_op_E4;                        /* CPX n */
  op[0xe5] = c6502_op_E5;                        /* SBC n */
  op[0xe6] = c6502_op_E6;                        /* INC n */
  op[0xe8] = c6502_op_E8;                        /* INX */
  op[0xe9] = c6502_op_E9;                        /* SBC #n */
  op[0xea] = c6502_op_EA;                        /* NOP */
  op[0xec] = c6502_op_EC;                        /* CPX nn */
  op[0xed] = c6502_op_ED;                        /* SBC nn */
  op[0xee] = c6502_op_EE;                        /* INC nn */
  op[0xf0] = c6502_op_F0;                        /* BEQ n */
  op[0xf1] = c6502_op_F1;                        /* SBC (n),Y */
  op[0xf5] = c6502_op_F5;                        /* SBC n,X */
  op[0xf6] = c6502_op_F6;                        /* INC n,X */
  op[0xf8] = c6502_op_F8;                        /* SED */
  op[0xf9] = c6502_op_F9;                        /* SBC nn,Y */
  op[0xfd] = c6502_op_FD;                        /* SBC nn,X */
  op[0xfe] = c6502_op_FE;                        /* INC nn,X */
  }

void c6502_do_irq(CPU6502* cpu) {
  byte p;
  word address;
  cpu->clocks = 7;
  cpu->pc++;
  address = cpu->s;
  address += 0x100;
  cpu->s--;
  wrmem(cpu, address, (cpu->pc >> 8));
  address = cpu->s;
  address += 0x100;
  cpu->s--;
  wrmem(cpu, address, (cpu->pc & 0xff));
  p = cpu->p;
  address = cpu->s;
  address += 0x100;
  cpu->s--;
  wrmem(cpu, address, cpu->p);
  address = rdmem(cpu, 0xfffe);
  address |= (rdmem(cpu, 0xffff) << 8);
  cpu->pc = address;
  }

void c6502_do_nmi(CPU6502* cpu) {
  byte p;
  word address;
  cpu->clocks = 7;
  cpu->pc++;
  address = cpu->s;
  address += 0x100;
  cpu->s--;
  wrmem(cpu, address, (cpu->pc >> 8));
  address = cpu->s;
  address += 0x100;
  cpu->s--;
  wrmem(cpu, address, (cpu->pc & 0xff));
  p = cpu->p;
  address = cpu->s;
  address += 0x100;
  cpu->s--;
  wrmem(cpu, address, cpu->p);
  address = rdmem(cpu, 0xfffa);
  address |= (rdmem(cpu, 0xfffb) << 8);
  cpu->pc = address;
  }

void c6502_reset(CPU6502* cpu) {
  cpu->idle = 0;
  cpu->pc = rdmem(cpu, 0xfffc) | (rdmem(cpu, 0xfffd) << 8);
  cpu->s = 0xfd;
  cpu->irq = 0;
  cpu->nmi = 0;
  cpu->clocks = 0;
  }

void c6502_irq(CPU6502* cpu) {
  cpu->irq = 0xff;
  }

void c6502_nmi(CPU6502* cpu) {
  cpu->nmi = 0xff;
  }

void c6502_cycle(CPU6502* cpu) {
  byte c;
  if (cpu->idle != 0) {
    cpu->clocks = 0;
    return;
    }
  cpu->clocks = 0;
  if (cpu->nmi != 0) {
    c6502_do_nmi(cpu);
    return;
    }
  if (cpu->irq != 0 && ((cpu->p & FLAG_I) == 0)) {
    c6502_do_irq(cpu);
    return;
    }
  c= fetch(cpu);
  if (op[c] != NULL) op[c](cpu);
    else cpu->clocks = 1;
  }

