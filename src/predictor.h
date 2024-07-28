#ifndef PREDICTOR_H
#define PREDICTOR_H
namespace Yuchuan {
struct Predictor{
  int status[100][3];
  int size = 0;
  Predictor() {
    for(int i = 0; i < 100; i++) {
      for(int j = 0; j < 3; j++) {
        status[i][j] = 0;
      }
    }
  }
  bool Predict(int addr) {
    for(int i = 0; i < size; i++) {
      if(status[i][0] == addr) {
        status[i][2]++;
        return (status[i][1] >= 2);
      }
    }
    if(size < 100) {
      status[size++][0] = addr;
      status[size][2] = 1;
    } else {
      int mini = status[0][2];
      int num = 0;
      for(int i = 0; i < 100; i++) {
        if(status[i][2] < mini) {
          mini = status[i][2];
          num = i;
        }
      }
      status[num][0] = addr;
      status[num][1] = 0;
      status[num][2] = 1; 
    }
    return false;
  }
  void update(int addr, bool up) {
    for(int i = 0; i < size; i++) {
      if(status[i][0] == addr) {
        if(up) {
          if(status[i][1] < 3) {
            status[i][1]++;
          }
        } else {
          if(status[i][1] > 0) {
            status[i][1]--;
          }
        }
      }
    }
    return;
  }
};
}
#endif