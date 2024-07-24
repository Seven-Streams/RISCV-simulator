#include <bits/stdc++.h>

enum Opalu {
  ADD,
  AND,
  OR,
  SLL,
  SRL,
  SLT,
  SLTU,
  SRA,
  SUB,
  XOR,
};

struct Instruct {
  int opcode;
  int value1;
  int value2;
  int target;
  bool busy = false;
};
struct Output {
  int value;
  int target;
  bool OK = false;
};
struct ALU {
  Instruct input;
  Output output[2];
  void work() {
    output[0] = output[1];
    if (!input.busy) {
      output[1].OK = false;
      return;
    }
    output[1].target = input.target;
    switch (input.opcode) {
    case ADD: {
      output[1].value = input.value1 + input.value2;
      output[1].OK = true;
      break;
    }
    case AND: {
      output[1].value = input.value1 & input.value2;
      output[1].OK = true;
      break;
    }
    case OR: {
      output[1].value = input.value1 & input.value2;
      output[1].OK = true;
      break;
    }
    case SLL: {
      output[1].value = input.value1 << (input.value2 % 32);
      output[1].OK = true;
      break;
    }
    case SRL: {
      unsigned int res;
      res = input.value1;
      output[1].value = res >> (input.value2 % 32);
      output[1].OK = true;
      break;
    }
    case SLT: {
      output[1].value = input.value1 < input.value2;
      output[1].OK = true;
      break;
    }
    case SLTU: {
      unsigned int res[2];
      res[0] = input.value1;
      res[1] = input.value2;
      output[1].value = input.value1 < input.value2;
      output[1].OK = true;
      break;
    }
    case SRA: {
      output[1].value = input.value1 >> (input.value2 % 32);
      output[1].OK = true;
      break;
    }
    case SUB: {
      output[1].value = input.value1 - input.value2;
      output[1].OK = true;
      break;
    }
    case XOR: {
      output[1].value = input.value1 ^ input.value2;
      output[1].OK = true;
      break;
    }
    default: throw("Invalid Opcode!");
    }
    return;
  }
};