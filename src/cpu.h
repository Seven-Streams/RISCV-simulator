#include "alu.h"
#include "memory.h"
#ifndef CPU_H
#define CPU_H
namespace Yuchuan {
struct CPU {
  ALU alu;
  Memory memory;
  int work() { return 0; }
};
} // namespace Yuchuan
#endif