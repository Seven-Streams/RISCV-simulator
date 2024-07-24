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
    if (input.clk >= 2) {
      switch (input.type) {
      case LB: {
        bool signal = (mem[input.addr] >> 7) & 1;
        output.value = (mem[input.addr]);
        if (signal) {
          output.value = output.value | int(0xffffff00);
        }
        output.target = input.target;
        output.busy = true;
        break;
      }
      case LBU: {
        output.value = mem[input.addr];
        output.target = input.target;
        output.busy = true;
        break;
      }
      case LH: {
        bool signal = (mem[input.addr] >> 7) & 1;
        output.value = int(mem[input.addr]) << 8;
        output.value += mem[input.addr + 1];
        if (signal) {
          output.value = output.value | int(0xffff0000);
        }
        output.target = input.target;
        output.busy = true;
        break;
      }
      case LHU: {
        output.value = int(mem[input.addr]) << 8;
        output.value += mem[input.addr + 1];
        output.target = input.target;
        output.busy = true;
        break;
      }
      case LW: {
        output.value = 0;
        for (int i = 0; i < 4; i++) {
          output.value <<= 8;
          output.value += mem[input.addr + i];
        }
        output.target = input.target;
        output.busy = true;
        break;
      }
      case SB: {
        mem[input.addr] = (input.value) & 0xff;
        output.busy = true;
        output.target = input.target;
        break;
      }
      case SH: {
        mem[input.addr] = (input.value >> 8) & 0xff;
        mem[input.addr + 1] = input.value & 0xff;
        output.busy = true;
        output.target = input.target;
        break;
      }
      case SW: {
        for (int i = 3; i >= 0; i--) {
          mem[input.addr + i] = input.value & 0xff;
          input.value >>= 8;
        }
        output.busy = true;
        output.target = input.target;
        break;
      }
      default: {
        throw("Invalid opcode in memory!");
      }
      }
    }
  }
};
} // namespace Yuchuan
#endif