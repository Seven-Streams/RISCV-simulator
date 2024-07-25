#include "alu.h"
#include "decoder.h"
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
    unsigned char code[4] = {memory.mem[now_pc], memory.mem[now_pc + 1],
                             memory.mem[now_pc + 2], memory.mem[now_pc + 3]};
    RawInstruction instruct = decode(code);
    return std::pair<bool, unsigned char>(false, 0);
  }
};
} // namespace Yuchuan
#endif