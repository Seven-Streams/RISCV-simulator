#include "alu.h"
#include "memory.h"
#include "rf.h"
#ifndef CPU_H
#define CPU_H
namespace Yuchuan {
struct CPU {
  ALU alu;
  Memory memory;
  Rf rf;
  int work() { return 0; }
};
} // namespace Yuchuan
#endif