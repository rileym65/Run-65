#define MAIN
#include "header.h"
#include "cpu6502.h"

byte readMem(word address) {
  return ram[address];
  }

void writeMem(word address, byte value) {
  ram[address] = value;
  }

void clearMem() {
  int i;
  for (i=0; i<65536; i++) ram[i] = 0;
  }

int main(int argc, char** argv) {
  int i;
  printf("Run/65 v1.0.0\n");
  printf("by Michael H. Riley\n");

  cpu.readMem = readMem;
  cpu.writeMem = writeMem;
  c6502_setup(&cpu);
  clearMem();
  runDebugger = 0;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i], "-d") == 0) runDebugger = 0xff;
    else if (loadFile(argv[i])) {
      printf("Error: Could not load %s\n",argv[i]);
      return 1;
      }
    }
  c6502_reset(&cpu);
  Debugger();
  return 0;
  }

