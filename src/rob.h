#include "isa.h"
#ifndef ROB_H
#define ROB_H
namespace Yuchuan{
struct Buffer {
  int type = -1;
  int value = -1;
  int des = -1;
  int now_pc;
  bool busy = false;
  bool OK = false;
};
struct RobOutput {
  Buffer data;
  int num = 0;
};
struct ROB {
  Buffer input;
  Buffer buffer[8];
  RobOutput output;
  int size = 0;
  int head = 0;
  int tail = 0;
  void work() {
    if(input.busy && (size != 8)) {
      buffer[tail++] = input;
      input.busy = false;
      tail %= 8;
      size++;
    }
    if(output.data.busy) {
      return;
    }
    if(buffer[head].OK && buffer[head].busy) {
      output.data = buffer[head];
      output.num = head;
      buffer[head++].busy = false;
      size--;
      head %= 8;
    }
    return;
  }
  void clean() {
    input.busy = false;
    output.data.busy = false;
    head = 0;
    tail = 0;
    size = 0;
    for(int i = 0; i < 8; i++) {
      buffer[i].busy = false;
    }
    return;
  }
};
}
#endif