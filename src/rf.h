#ifndef RF_H
#define RF_H
namespace Yuchuan {
struct RF {
  int dependency[32];
  RF() {
    for(int i = 0; i < 32; i++) {
      dependency[i] = -1;
    }
  }
};
} // namespace Yuchuan
#endif