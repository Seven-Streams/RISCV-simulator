#include "alu.h"
#include "memory.h"
#include "rf.h"
#include "rs.h"
#include "lsb.h"
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
  int work() { return 0; }
};
} // namespace Yuchuan
#endif