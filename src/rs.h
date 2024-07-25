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
      for (int cnt = 7; cnt >= 0; cnt--) {
        if (!reserve[cnt].busy) {
          reserve[cnt] = input;
          size++;
          input.busy = false;
          break;
        }
      }
    }
    if (!output.busy) {
      for (int i = 0; i < 8; i++) {
        if (reserve[i].busy && (reserve[i].query[0] == -1) &&
            (reserve[i].query[1] == -1)) {
          output = reserve[i];
          reserve[i].busy = false;
          size--;
          break;
        }
      }
    }
    return;
  }
};
} // namespace Yuchuan

#endif