#ifndef LSB_H
#define LSB_H
namespace Yuchuan {
struct LSBInstruct {
  int type = 0;
  bool busy = false;
  int offset = 0;
  int des = 0;
  int v = -1;
  int q = -1;
};
struct LSB {
  LSBInstruct input;
  LSBInstruct buffer[3];
  LSBInstruct output;
  int size = 0;
  void work() {
    if(input.busy && size != 3) {
      buffer[size++] = input;
      input.busy = false;
    }
    if (output.busy || (size == 0)) {
      return;
    }
    if(buffer[0].q == -1) {
      output = buffer[0];
      buffer[0] = buffer[1];
      buffer[1] = buffer[2];
      size--;
    }
    return;
  }
};
}
#endif