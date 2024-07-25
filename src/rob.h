#include "isa.h"
#ifndef ROB_H
#define ROB_H
namespace Yuchuan{
struct Buffer {
  int type = -1;
  int value = -1;
  int des = -1;
  bool busy = false;
  bool OK = false;
};
struct ROB {
  Buffer input;
  Buffer buffer[8];
  Buffer output;
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
    if(output.busy) {
      return;
    }
    if(buffer[head].OK && buffer[head].busy) {
      output = buffer[head];
      buffer[head++].busy = false;
      size--;
      head %= 8;
    }
    return;
  }
};
}
#endif