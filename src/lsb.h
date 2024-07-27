#ifndef LSB_H
#define LSB_H
namespace Yuchuan {
struct LSBInstruct {
  int type = 0;
  bool busy = false;
  int addr = 0;
  int des = 0;
  int value = 0;
};
struct LSB {
  LSBInstruct input;
  LSBInstruct buffer[3];
  LSBInstruct output;
  int size = 0;
  void work() {
    if (input.busy && size != 3) {
      buffer[size++] = input;
      input.busy = false;
    }
    if (output.busy || (size == 0)) {
      return;
    }
    output = buffer[0];
    buffer[0] = buffer[1];
    buffer[1] = buffer[2];
    size--;
    return;
  }
  void clean() {
    input.busy = false;
    output.busy = false;
    size = 0;
    for(int i = 0; i < 3; i++) {
      buffer[i].busy = false;
    }
  }
};
} // namespace Yuchuan
#endif