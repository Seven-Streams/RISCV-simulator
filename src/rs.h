#ifndef RS_H
#define RS_H
namespace Yuchuan {
struct ReservedInstruct {
  int opcode = -1;
  bool busy = false;
  int value[2] = {0, 0};
  int query[2] = {-1, -1};
  int des = 0;
  int imm = 0;
};
struct RS {
  ReservedInstruct input;
  ReservedInstruct reserve[8];
  ReservedInstruct output;
  int size = 0;
  void work() {
    if (input.busy && (size < 8)) {
      for (int cnt = 0; cnt < 8; cnt++) {
        if (!reserve[cnt].busy) {
          reserve[cnt] = input;
          size++;
          input.busy = false;
          break;
        }
      }
    }
    if (!output.busy) {
      bool OK_tols = true;
      for (int i = 0; i < 8; i++) {
        if((!OK_tols) && (reserve[i].opcode <= 7)) {
          continue;
        }
        if (reserve[i].busy && (reserve[i].query[0] == -1) &&
            (reserve[i].query[1] == -1)) {
          output = reserve[i];
          reserve[i].busy = false;
          size--;
          for(int j = i; j < 7; j++) {
            reserve[j] = reserve[j + 1];
          }
          break;//To make sure it's in order.
        }
        if(reserve[i].opcode <= 7) {
          OK_tols = false;
        }
      }
    }
    return;
  }
  void clean() {
    input.busy = false;
    output.busy = false;
    size = 0;
    for(int i = 0; i < 8; i++) {
      reserve[i].busy = false;
    }
    return;
  }
};
} // namespace Yuchuan

#endif