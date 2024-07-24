#ifndef RF_H
#define RF_H
namespace Yuchuan {
struct Rf {
  int dependency[32];
  Rf() {
    for(int i = 0; i < 32; i++) {
      dependency[i] = -1;
    }
  }
};
} // namespace Yuchuan
#endif