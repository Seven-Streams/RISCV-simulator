#include "src/cpu.h"
#include <cstring>
#include <iostream>
#include <string>
int main() {
  // std::string name;
  // std::cin >> name;
  // freopen(name.c_str(), "r", stdin);
  Yuchuan::CPU barris;
  std::string res;
  unsigned int addr;
  while (!std::cin.eof()) {
    std::cin >> res;
    if (res[0] == '@') {
      res = res.erase(0, 1);
      addr = std::stoi(res, nullptr, 16);
    } else {
      unsigned int x;
      x = std::stoi(res, nullptr, 16);
      unsigned char to_add = x & (0x000000ff);
      barris.memory.mem[addr++] = to_add;
    }
  }
  int last = 0;
  std::pair<bool, int> status;
  do {
    status = barris.work();
  } while (!status.first);
  std::cout << status.second << std::endl;
  return 0;
}