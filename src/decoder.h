#include "isa.h"
#include <cmath>
#ifndef DECODER_H
#define DECODER_H
namespace Yuchuan {
struct RawInstruction {
  int type = -1;
  int rs1 = 0;
  int rs2 = 0;
  int rd = 0;
  int imm = 0;
};
RawInstruction decode(unsigned char instruct[4]) {
  RawInstruction ans;
  if((instruct[0] == 0x13) && (instruct[1] == 0x05) && (instruct[2] == 0xf0) && (instruct[3] == 0x0f)) {
    ans.type = HALT;
    return ans;
  }
  unsigned char reversed[4] = {instruct[3], instruct[2], instruct[1],
                               instruct[0]};
  unsigned char opcode = reversed[3] & 0b01111111;
  if ((opcode == 0b0110111) || (opcode == 0b0010111)) { // type U.
    ans.rd += reversed[2] & 0x0f;
    ans.rd <<= 1;
    ans.rd += (reversed[3] >> 7) & 1;
    ans.imm += reversed[0];
    ans.imm <<= 8;
    ans.imm += reversed[1];
    ans.imm <<= 8;
    ans.imm += reversed[2] & 0xf0;
    ans.imm <<= 8;
    if (opcode == 0b0110111) {
      ans.type = LUI;
    } else {
      ans.type = AUIPC;
    }
  }
  if (opcode == 0b1101111) { // type J.
    ans.type = JAL;
    bool signal = reversed[0] & 0b10000000;
    ans.imm += (reversed[0] >> 7) & 1;
    ans.imm <<= 4;
    ans.imm += reversed[1] & 0b00001111;
    ans.imm <<= 4;
    ans.imm += reversed[2] >> 4;
    ans.imm <<= 1;
    ans.imm += (reversed[1] >> 4) & 1;
    ans.imm <<= 7;
    ans.imm += reversed[0] & 0b01111111;
    ans.imm <<= 3;
    ans.imm += (reversed[1] >> 5);
    ans.imm <<= 1;
    if (signal) {
      ans.imm = 0xfff00000 | ans.imm;
    }
  }
  if (opcode == 0b1100011) { // type B.
    unsigned char func;
    func = (reversed[2] >> 4) & (0b0111);
    ans.rs1 += reversed[1] & (0x0f);
    ans.rs1 <<= 1;
    ans.rs1 += (reversed[2] >> 7) & 1;
    ans.rs2 += reversed[0] & 1;
    ans.rs2 <<= 4;
    ans.rs2 += (reversed[1] >> 4) & (0x0f);
    bool signal = (reversed[0] >> 7) & 1;
    ans.imm += (reversed[3] >> 7) & 1;
    ans.imm <<= 6;
    ans.imm += (reversed[0] >> 1) & (0b00111111);
    ans.imm <<= 4;
    ans.imm += reversed[1] & (0xf);
    ans.imm <<= 1;
    if (signal) {
      ans.imm = 0xfffff000 | ans.imm;
    }
    switch (func) {
    case 0: {
      ans.type = BEQ;
      break;
    }
    case 1: {
      ans.type = BNE;
      break;
    }
    case 4: {
      ans.type = BLT;
      break;
    }
    case 5: {
      ans.type = BGE;
      break;
    }
    case 6: {
      ans.type = BLTU;
      break;
    }
    case 7: {
      ans.type = BGEU;
      break;
    }
    default: {
      throw("Invalid Function code in B type!");
    }
    }
  }
  if (opcode == 0b0110011) { // type R.
    unsigned char func;
    func = (reversed[2] >> 4) & (0b0111);
    ans.rs1 += reversed[1] & (0x0f);
    ans.rs1 <<= 1;
    ans.rs1 += (reversed[2] >> 7) & 1;
    ans.rd += reversed[2] & (0x0f);
    ans.rd <<= 1;
    ans.rd += (reversed[3] >> 7) & 1;
    ans.rs2 += reversed[0] & 1;
    ans.rs2 <<= 4;
    ans.rs2 += (reversed[1] >> 4) & (0x0f);
    unsigned char check;
    check = reversed[0] >> 1;
    switch (func) {
    case 0: {
      switch (check) {
      case 0: {
        ans.type = ADD;
        break;
      }
      case 0b0100000: {
        ans.type = SUB;
        break;
      }
      default: {
        throw("Double check failure ADD!");
      }
      }
      break;
    }
    case 1: {
      if (check != 0) {
        throw("Double check failure SLL!");
      } else {
        ans.type = SLL;
      }
      break;
    }
    case 2: {
      if (check != 0) {
        throw("Double check failure SLT!");
      } else {
        ans.type = SLT;
      }
      break;
    }
    case 3: {
      if (check != 0) {
        throw("Double check failure SLTU!");
      } else {
        ans.type = SLTU;
      }
      break;
    }
    case 4: {
      if (check != 0) {
        throw("Double check failure XOR!");
      } else {
        ans.type = XOR;
      }
      break;
    }
    case 5: {
      switch (check) {
      case 0: {
        ans.type = SRL;
        break;
      }
      case 0b0100000: {
        ans.type = SRA;
        break;
      }
      default: {
        throw("Double check failure SRL!");
      }
      }
      break;
    }
    case 6: {
      if (check != 0) {
        throw("Double check failure OR!");
      } else {
        ans.type = OR;
      }
      break;
    }
    case 7: {
      if (check != 0) {
        throw("Double check failure AND!");
      } else {
        ans.type = AND;
      }
      break;
    }
    default: {
      throw("Invalid func code in R type!");
    }
    }
  }
  if (opcode == 0b0100011) { // S type.
    unsigned char func;
    func = (reversed[2] >> 4) & (0b0111);
    ans.rs1 += reversed[1] & (0x0f);
    ans.rs1 <<= 1;
    ans.rs1 += (reversed[2] >> 7) & 1;
    ans.rs2 += reversed[0] & 1;
    ans.rs2 <<= 4;
    ans.rs2 += (reversed[1] >> 4) & (0x0f);
    bool signal = (reversed[0] >> 7) & 1;
    ans.imm += (reversed[0] >> 1) & (0b01111111);
    ans.imm <<= 4;
    ans.imm += reversed[2] & 0x0f;
    ans.imm <<= 1;
    ans.imm += (reversed[3] >> 7) & 1;
    switch (func) {
      case 0: {
        ans.type = SB;
        break;
      }
      case 1: {
        ans.type = SH;
        break;
      }
      case 2: {
        ans.type = SW;
        break;
      }
      default:{
        throw("Invalid function code in S type!");
      }
    }
  }
  if(opcode == 0b1100111 || opcode == 0b0000011 || opcode == 0b0010011) {// type I.
    unsigned char func;
    bool signal = (reversed[0] >> 7) & 1;
    func = (reversed[2] >> 4) & (0b0111);
    ans.rs1 += reversed[1] & (0x0f);
    ans.rs1 <<= 1;
    ans.rs1 += (reversed[2] >> 7) & 1;
    ans.rd += reversed[2] & (0x0f);
    ans.rd <<= 1;
    ans.rd += (reversed[3] >> 7) & 1;
    ans.imm += reversed[0];
    ans.imm <<= 4;
    ans.imm += reversed[1] >> 4;
    if(signal) {
      ans.imm |= 0xfffff000;
    }
    switch (opcode){
    case 0b1100111: {
      if(func != 0) {
        throw("Invalid func code in I!");
      }
      ans.type = JALR;
      break;
    }
    case 0b0000011: {
      switch (func) {
        case 0: {
          ans.type = LB;
          break;
        }
        case 1: {
          ans.type = LH;
          break;
        }
        case 2: {
          ans.type = LW;
          break;
        }
        case 4:{
          ans.type = LBU;
          break;
        }
        case 5:{
          ans.type = LHU;
          break;
        }
        default:{
          throw("Invalid function code in I type!");
        }
      }
      break;
    }
    case 0b0010011: {
      switch (func) {
        case 0: {
          ans.type = ADDI;
          break;
        }
        case 2: {
          ans.type = SLTI;
          break;
        }
        case 3: {
          ans.type = SLTIU;
          break;
        }
        case 4: {
          ans.type = XORI;
          break;
        }
        case 6: {
          ans.type = ORI;
          break;
        }
        case 7: {
          ans.type = ANDI;
          break;
        }
        case 1: {
          ans.type = SLLI;
          if(ans.imm >= 32 || (ans.imm < 0)) {
            throw("Wrong Shift number!");
          }
          break;
        }
        case 5: {
          int check = (ans.imm >> 5);
          ans.imm &= 0x0000001f;
          switch (check) {
            case 0: {
              ans.type = SRLI;
              break;
            }
            case 0b0100000: {
              ans.type = SRAI;
              break;
            }
            default: {
              throw("Invalid number in SRLI and SRAI!");
            }
          }
          break;
        }
        default: {
          throw("Invalid function number in I type!");
        }
      }
    }
    }
  }
  return ans;
}
} // namespace Yuchuan
#endif