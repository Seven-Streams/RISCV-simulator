#ifndef ALU_H
#define ALU_H
#include "isa.h"
namespace Yuchuan {

struct AluInstruct {
  int opcode;
  int value1;
  int value2;
  int target;
  bool busy = false;
};
struct AluOutput {
  int value;
  int target;
  bool OK = false;
};
struct ALU {
  AluInstruct input;
  AluOutput output;
  void work() {
    if (output.OK) {
      return;
    }
    if (!input.busy) {
      return;
    }
    output.target = input.target;
    switch (input.opcode) {
    case ADD: {
      output.value = input.value1 + input.value2;
      output.OK = true;
      break;
    }
    case AND: {
      output.value = input.value1 & input.value2;
      output.OK = true;
      break;
    }
    case OR: {
      output.value = input.value1 & input.value2;
      output.OK = true;
      break;
    }
    case SLL: {
      output.value = input.value1 << (input.value2 % 32);
      output.OK = true;
      break;
    }
    case SRL: {
      unsigned int res;
      res = input.value1;
      output.value = res >> (input.value2 % 32);
      output.OK = true;
      break;
    }
    case SLT: {
      output.value = input.value1 < input.value2;
      output.OK = true;
      break;
    }
    case SLTU: {
      unsigned int res[2];
      res[0] = input.value1;
      res[1] = input.value2;
      output.value = input.value1 < input.value2;
      output.OK = true;
      break;
    }
    case SRA: {
      output.value = input.value1 >> (input.value2 % 32);
      output.OK = true;
      break;
    }
    case SUB: {
      output.value = input.value1 - input.value2;
      output.OK = true;
      break;
    }
    case XOR: {
      output.value = input.value1 ^ input.value2;
      output.OK = true;
      break;
    }
    default:
      throw("Invalid Opcode in ALU!");
    }
    return;
  }
};
} // namespace Yuchuan
#endif