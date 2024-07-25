#include "alu.h"
#include "decoder.h"
#include "isa.h"
#include "lsb.h"
#include "memory.h"
#include "rf.h"
#include "rob.h"
#include "rs.h"
#ifndef CPU_H
#define CPU_H
namespace Yuchuan {
struct CPU {
  int reg[32];
  ALU alu;
  Memory memory;
  RF rf;
  RS rs;
  LSB lsb;
  ROB rob;
  int now_pc = 0;
  int advanced_pc = 0;
  std::pair<bool, unsigned char> work() {
    if (now_pc == -1) {
      return std::pair<bool, unsigned char>(true, reg[0]);
    }
    unsigned char code[4] = {
        memory.mem[advanced_pc], memory.mem[advanced_pc + 1],
        memory.mem[advanced_pc + 2], memory.mem[advanced_pc + 3]};
    RawInstruction instruct = decode(code);
    if (!rob.input.busy) {
      Buffer to_push;
      to_push.type = instruct.type;
      to_push.busy = true;
      to_push.OK = false;
      int num = rob.tail;
      if (instruct.type == HALT) {
        to_push.des = -1;
        to_push.OK = true;
        to_push.value = 0;
        rob.input = to_push;
      }
      if (instruct.type == AUIPC) {
        to_push.des = instruct.rd;
        to_push.OK = true;
        to_push.value = advanced_pc + instruct.imm;
        rob.input = to_push;
        advanced_pc += instruct.imm;
        rf.dependency[to_push.des] = num;
      }
      if (instruct.type == LUI) {
        to_push.des = instruct.rd;
        to_push.OK = true;
        to_push.value = instruct.imm;
        rob.input = to_push;
        advanced_pc += 4;
        rf.dependency[to_push.des] = num;
      }
      if(instruct.type == JAL) {
        to_push.des = instruct.rd;
        to_push.OK = true;
        to_push.value = instruct.imm;
        rob.input = to_push;
        advanced_pc += instruct.imm;
        rf.dependency[to_push.des] = num;
      }
    }
    return std::pair<bool, unsigned char>(false, 0);
  }
};
} // namespace Yuchuan
#endif