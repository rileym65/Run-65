#include "header.h"

word Disassem(word addr, char* dest) {
  char tmp[80];
  sprintf(dest,"%04x: %02x ", addr, ram[addr]);
  switch (ram[addr]) {
    case 0x00:
         sprintf(tmp,"      BRK");
         addr++; break;
    case 0x01:
         sprintf(tmp,"%02x    ORA  ($%02x,X)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x04:
         sprintf(tmp,"%02x    TSB  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x05:
         sprintf(tmp,"%02x    ORA  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x06:
         sprintf(tmp,"%02x    ASL  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x08:
         sprintf(tmp,"      PHP");
         addr++; break;
    case 0x09:
         sprintf(tmp,"%02x    ORA  #$%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x0a:
         sprintf(tmp,"      ASL  A");
         addr++; break;
    case 0x0c:
         sprintf(tmp,"%02x %02x TSB  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x0d:
         sprintf(tmp,"%02x %02x ORA  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x0e:
         sprintf(tmp,"%02x %02x ASL  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x10:
         sprintf(tmp,"%02x    BPL  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x11:
         sprintf(tmp,"%02x    ORA  ($%02x),Y",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x12:
         sprintf(tmp,"%02x    ORA  ($%02x)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x14:
         sprintf(tmp,"%02x    TRB  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x15:
         sprintf(tmp,"%02x    ORA  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x16:
         sprintf(tmp,"%02x    ASL  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x18:
         sprintf(tmp,"      CLC");
         addr++; break;
    case 0x19:
         sprintf(tmp,"%02x %02x ORA  $%02x%02x,Y",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x1a:
         sprintf(tmp,"      INA");
         addr++; break;
    case 0x1c:
         sprintf(tmp,"%02x %02x TRB  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x1d:
         sprintf(tmp,"%02x %02x ORA  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x1e:
         sprintf(tmp,"%02x %02x ASL  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x20:
         sprintf(tmp,"%02x %02x JSR  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x21:
         sprintf(tmp,"%02x    AND  ($%02x,X)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x24:
         sprintf(tmp,"%02x    BIT  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x25:
         sprintf(tmp,"%02x    AND  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x26:
         sprintf(tmp,"%02x    ROL  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x28:
         sprintf(tmp,"      PLP");
         addr++; break;
    case 0x29:
         sprintf(tmp,"%02x    AND  #$%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x2a:
         sprintf(tmp,"      ROL  A");
         addr++; break;
    case 0x2c:
         sprintf(tmp,"%02x %02x BIT  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x2d:
         sprintf(tmp,"%02x %02x AND  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x2e:
         sprintf(tmp,"%02x %02x ROL  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x30:
         sprintf(tmp,"%02x    BMI  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x31:
         sprintf(tmp,"%02x    AND  ($%02x),Y",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x32:
         sprintf(tmp,"%02x    AND  ($%02x)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x34:
         sprintf(tmp,"%02x    BIT  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x35:
         sprintf(tmp,"%02x    AND  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x36:
         sprintf(tmp,"%02x    ROL  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x38:
         sprintf(tmp,"      SEC");
         addr++; break;
    case 0x39:
         sprintf(tmp,"%02x %02x AND  $%02x%02x,Y",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x3a:
         sprintf(tmp,"      DEA");
         addr++; break;
    case 0x3c:
         sprintf(tmp,"%02x %02x BIT  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x3d:
         sprintf(tmp,"%02x %02x AND  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x3e:
         sprintf(tmp,"%02x %02x ROL  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x40:
         sprintf(tmp,"      RTI");
         addr++; break;
    case 0x41:
         sprintf(tmp,"%02x    EOR  ($%02x,X)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x45:
         sprintf(tmp,"%02x    EOR  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x46:
         sprintf(tmp,"%02x    LSR  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x48:
         sprintf(tmp,"      PHA");
         addr++; break;
    case 0x49:
         sprintf(tmp,"%02x    EOR  #$%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x4a:
         sprintf(tmp,"      LSR  A");
         addr++; break;
    case 0x4c:
         sprintf(tmp,"%02x %02x JMP  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x4d:
         sprintf(tmp,"%02x %02x EOR  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x4e:
         sprintf(tmp,"%02x %02x LSR  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x50:
         sprintf(tmp,"%02x    BVC  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x51:
         sprintf(tmp,"%02x    EOR  ($%02x),Y",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x52:
         sprintf(tmp,"%02x    EOR  ($%02x)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x55:
         sprintf(tmp,"%02x    EOR  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x56:
         sprintf(tmp,"%02x    LSR  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x58:
         sprintf(tmp,"      CLI");
         addr++; break;
    case 0x59:
         sprintf(tmp,"%02x %02x EOR  $%02x%02x,Y",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x5a:
         sprintf(tmp,"      PHY");
         addr++; break;
    case 0x5d:
         sprintf(tmp,"%02x %02x EOR  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x5e:
         sprintf(tmp,"%02x %02x LSR  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x60:
         sprintf(tmp,"      RTS");
         addr++; break;
    case 0x61:
         sprintf(tmp,"%02x    ADC  ($%02x,X)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x64:
         sprintf(tmp,"%02x    STZ  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x65:
         sprintf(tmp,"%02x    ADC  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x66:
         sprintf(tmp,"%02x    ROR  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x68:
         sprintf(tmp,"      PLA");
         addr++; break;
    case 0x69:
         sprintf(tmp,"%02x    ADC  #$%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x6a:
         sprintf(tmp,"      ROR  A");
         addr++; break;
    case 0x6c:
         sprintf(tmp,"%02x %02x JMP  ($%02x%02x)",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x6d:
         sprintf(tmp,"%02x %02x ADC  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x6e:
         sprintf(tmp,"%02x %02x ROR  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x70:
         sprintf(tmp,"%02x    BVS  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x71:
         sprintf(tmp,"%02x    ADC  ($%02x),Y",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x72:
         sprintf(tmp,"%02x    ADC  ($%02x)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x74:
         sprintf(tmp,"%02x    STZ  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x75:
         sprintf(tmp,"%02x    ADC  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x76:
         sprintf(tmp,"%02x    ROR  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x78:
         sprintf(tmp,"      SEI");
         addr++; break;
    case 0x79:
         sprintf(tmp,"%02x %02x ADC  $%02x%02x,Y",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x7a:
         sprintf(tmp,"      PLY");
         addr++; break;
    case 0x7c:
         sprintf(tmp,"%02x %02x JMP  ($%02x%02x,X)",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x7d:
         sprintf(tmp,"%02x %02x ADC  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x7e:
         sprintf(tmp,"%02x %02x ROR  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x80:
         sprintf(tmp,"%02x    BRA  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x81:
         sprintf(tmp,"%02x    STA  ($%02x,X)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x84:
         sprintf(tmp,"%02x    STY  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x85:
         sprintf(tmp,"%02x    STA  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x86:
         sprintf(tmp,"%02x    STX  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x88:
         sprintf(tmp,"      DEY");
         addr++; break;
    case 0x89:
         sprintf(tmp,"%02x    BIT  #$%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x8a:
         sprintf(tmp,"      TXA");
         addr++; break;
    case 0x8c:
         sprintf(tmp,"%02x %02x STY  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x8d:
         sprintf(tmp,"%02x %02x STA  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x8e:
         sprintf(tmp,"%02x %02x STX  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x90:
         sprintf(tmp,"%02x    BCC  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x91:
         sprintf(tmp,"%02x    STA  ($%02x),Y",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x92:
         sprintf(tmp,"%02x    STA  ($%02x)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x94:
         sprintf(tmp,"%02x    STY  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x95:
         sprintf(tmp,"%02x    STA  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x96:
         sprintf(tmp,"%02x    STX  $%02x,Y",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0x98:
         sprintf(tmp,"      TYA");
         addr++; break;
    case 0x99:
         sprintf(tmp,"%02x %02x STA  $%02x%02x,Y",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x9a:
         sprintf(tmp,"      TXS");
         addr++; break;
    case 0x9c:
         sprintf(tmp,"%02x %02x STZ  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x9d:
         sprintf(tmp,"%02x %02x STA  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0x9e:
         sprintf(tmp,"%02x %02x STZ  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xa0:
         sprintf(tmp,"%02x    LDY  #$%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xa1:
         sprintf(tmp,"%02x    LDA  ($%02x,X)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xa2:
         sprintf(tmp,"%02x    LDX  #$%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xa4:
         sprintf(tmp,"%02x    LDY  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xa5:
         sprintf(tmp,"%02x    LDA  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xa6:
         sprintf(tmp,"%02x    LDX  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xa8:
         sprintf(tmp,"      TAY");
         addr++; break;
    case 0xa9:
         sprintf(tmp,"%02x    LDA  #$%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xaa:
         sprintf(tmp,"      TAX");
         addr++; break;
    case 0xac:
         sprintf(tmp,"%02x %02x LDY  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xad:
         sprintf(tmp,"%02x %02x LDA  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xae:
         sprintf(tmp,"%02x %02x LDX  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xb0:
         sprintf(tmp,"%02x    BCS  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xb1:
         sprintf(tmp,"%02x    LDA  ($%02x),Y",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xb2:
         sprintf(tmp,"%02x    LDA  ($%02x)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xb4:
         sprintf(tmp,"%02x    LDY  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xb5:
         sprintf(tmp,"%02x    LDA  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xb6:
         sprintf(tmp,"%02x    LDA  $%02x,Y",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xb8:
         sprintf(tmp,"      CLV");
         addr++; break;
    case 0xb9:
         sprintf(tmp,"%02x %02x LDA  $%02x%02x,Y",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xba:
         sprintf(tmp,"      TSX");
         addr++; break;
    case 0xbc:
         sprintf(tmp,"%02x %02x LDY  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xbd:
         sprintf(tmp,"%02x %02x LDA  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xbe:
         sprintf(tmp,"%02x %02x LDX  $%02x%02x,Y",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xc0:
         sprintf(tmp,"%02x    CPY  #$%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xc1:
         sprintf(tmp,"%02x    CMP  ($%02x,X)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xc4:
         sprintf(tmp,"%02x    CPY  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xc5:
         sprintf(tmp,"%02x    CMP  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xc6:
         sprintf(tmp,"%02x    DEC  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xc8:
         sprintf(tmp,"      INY");
         addr++; break;
    case 0xc9:
         sprintf(tmp,"%02x    CMP  #$%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xca:
         sprintf(tmp,"      DEX");
         addr++; break;
    case 0xcc:
         sprintf(tmp,"%02x %02x CPY  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xcd:
         sprintf(tmp,"%02x %02x CMP  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xce:
         sprintf(tmp,"%02x %02x DEC  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xd0:
         sprintf(tmp,"%02x    BNE  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xd1:
         sprintf(tmp,"%02x    CMP  ($%02x),Y",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xd2:
         sprintf(tmp,"%02x    CMP  ($%02x)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xd5:
         sprintf(tmp,"%02x    CMP  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xd6:
         sprintf(tmp,"%02x    DEC  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xd8:
         sprintf(tmp,"      CLD");
         addr++; break;
    case 0xd9:
         sprintf(tmp,"%02x %02x CMP  $%02x%02x,Y",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xda:
         sprintf(tmp,"      PHX");
         addr++; break;
    case 0xdd:
         sprintf(tmp,"%02x %02x CMP  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xde:
         sprintf(tmp,"%02x %02x DEC  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xe0:
         sprintf(tmp,"%02x    CPX  #$%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xe1:
         sprintf(tmp,"%02x    SBC  ($%02x,X)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xe4:
         sprintf(tmp,"%02x    CPX  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xe5:
         sprintf(tmp,"%02x    SBC  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xe6:
         sprintf(tmp,"%02x    INC  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xe8:
         sprintf(tmp,"      INX");
         addr++; break;
    case 0xe9:
         sprintf(tmp,"%02x    SBC  #$%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xea:
         sprintf(tmp,"      NOP");
         addr++; break;
    case 0xec:
         sprintf(tmp,"%02x %02x CPX  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xed:
         sprintf(tmp,"%02x %02x SBC  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xee:
         sprintf(tmp,"%02x %02x INC  $%02x%02x",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xf0:
         sprintf(tmp,"%02x    BEQ  $%02x",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xf1:
         sprintf(tmp,"%02x    SBC  ($%02x),Y",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xf2:
         sprintf(tmp,"%02x    SBC  ($%02x)",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xf5:
         sprintf(tmp,"%02x    SBC  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xf6:
         sprintf(tmp,"%02x    INC  $%02x,X",ram[addr+1],ram[addr+1]);
         addr += 2; break;
    case 0xf8:
         sprintf(tmp,"      SED");
         addr++; break;
    case 0xf9:
         sprintf(tmp,"%02x %02x SBC  $%02x%02x,Y",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xfa:
         sprintf(tmp,"      PLX");
         addr++; break;
    case 0xfd:
         sprintf(tmp,"%02x %02x SBC  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    case 0xfe:
         sprintf(tmp,"%02x %02x INC  $%02x%02x,X",ram[addr+1],ram[addr+2],
                                                ram[addr+2],ram[addr+1]);
         addr += 3; break;
    default: 
         sprintf(tmp,"      DB   %02x",ram[addr]); addr++; break;
    }
  strcat(dest, tmp);
  return addr;
  }

