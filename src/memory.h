#ifndef MEM_H
#define MEM_H
#include "isa.h"
namespace Yuchuan {

struct LsInput {
  int value = 0;
  int addr = 0;
  int clk = -1;
  int type = -1;
  int target = 0;
};
struct LsOutput {
  int target = -1;
  int value;
  bool busy = false;
};
struct Memory {
  unsigned char mem[0x100000];
  LsOutput output;
  LsInput input;
  void work() {
    if (input.clk != -1) {
      input.clk++;
    }
    if (output.busy) {
      return;
    }
    if (input.clk > 2) {
      input.clk = -1;
      output.target = input.target;
      output.busy = true;
      switch (input.type) {
      case LB: {
        bool signal = (mem[input.addr] >> 7) & 1;
        output.value = (mem[input.addr]);
        if (signal) {
          output.value |=0xffffff00;
        }
        break;
      }
      case LBU: {
        output.value = mem[input.addr];
        break;
      }
      case LH: {
        bool signal = (mem[input.addr + 1] >> 7) & 1;
        output.value = int(mem[input.addr + 1]) << 8;
        output.value += mem[input.addr];
        if (signal) {
          output.value = output.value | int(0xffff0000);
        }
        break;
      }
      case LHU: {
        output.value = int(mem[input.addr + 1]) << 8;
        output.value += mem[input.addr];
        break;
      }
      case LW: {
        output.value = 0;
        for (int i = 3; i >= 0; i--) {
          output.value <<= 8;
          output.value += mem[input.addr + i];
        }
        break;
      }
      case SB: {
        mem[input.addr] = (input.value) & 0xff;
        break;
      }
      case SH: {
        mem[input.addr + 1] = (input.value >> 8) & 0xff;
        mem[input.addr] = input.value & 0xff;
        break;
      }
      case SW: {
        for (int i = 0; i <= 3; i++) {
          mem[input.addr + i] = input.value & 0xff;
          input.value >>= 8;
        }
        break;
      }
      default: {
        throw(input.type);
      }
      }
    }
  }
};
} // namespace Yuchuan
#endif