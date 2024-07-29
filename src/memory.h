#ifndef MEM_H
#define MEM_H
#include "isa.h"
#include <cstring>
#include <utility>
namespace Yuchuan {

struct LsInput {
  int value = 0;
  int addr = 0;
  int clk = -1;
  int type = -1;
  int target = 0;
};
struct LsOutput {
  int target = -1;
  int value;
  bool busy = false;
};
struct CacheBlock {
  unsigned char mem[0x100];
  int addr = 0;
  int recently = 0;
  bool busy = false;
};
struct Instruct {
  int addr;
  unsigned char data;
  char type; // l, load; s, store;
  std::pair<unsigned char, bool> OK;
};
struct Memory {
  unsigned char mem[0x100000];
  LsInput reserved[8];
  CacheBlock cache[10];
  LsOutput output;
  LsInput input;
  Instruct instruct[4];
  int instruct_size = 0;
  int reserve_size = 0;
  int cache_size = 0;
  bool reset = false;
  void work() {
    if (reset) {
      if ((reserve_size == 0) && (input.clk == -1)) {
        reset = false;
      }
      if ((reserve_size != 0) && (input.clk == -1)) {
        input = reserved[--reserve_size];
        input.clk = 0;
        input.target = -1;
      }
    }
    if (input.clk != -1) {
      if (input.clk == 0) {
        switch (input.type) {
        case LB: {
          instruct[0].type = 'l';
          instruct[0].addr = input.addr;
          instruct[0].OK.second = false;
          instruct_size = 1;
          break;
        }
        case LBU: {
          instruct[0].type = 'l';
          instruct[0].addr = input.addr;
          instruct[0].OK.second = false;
          instruct_size = 1;
          break;
        }
        case LH: {
          instruct[1].type = 'l';
          instruct[1].addr = input.addr;
          instruct[1].OK.second = false;
          instruct[0].type = 'l';
          instruct[0].addr = input.addr + 1;
          instruct[0].OK.second = false;
          instruct_size = 2;
          break;
        }
        case LHU: {
          instruct[1].type = 'l';
          instruct[1].addr = input.addr;
          instruct[1].OK.second = false;
          instruct[0].type = 'l';
          instruct[0].addr = input.addr + 1;
          instruct[0].OK.second = false;
          instruct_size = 2;
          break;
        }
        case LW: {
          instruct[3].type = 'l';
          instruct[3].addr = input.addr;
          instruct[3].OK.second = false;
          instruct[2].type = 'l';
          instruct[2].addr = input.addr + 1;
          instruct[2].OK.second = false;
          instruct[1].type = 'l';
          instruct[1].addr = input.addr + 2;
          instruct[1].OK.second = false;
          instruct[0].type = 'l';
          instruct[0].addr = input.addr + 3;
          instruct[0].OK.second = false;
          instruct_size = 4;
          break;
        }
        case SB: {
          instruct[0].type = 's';
          instruct[0].addr = input.addr;
          instruct[0].data = input.value & 0xff;
          instruct[0].OK.second = false;
          instruct_size = 1;
          break;
        }
        case SH: {
          instruct[1].type = 's';
          instruct[1].addr = input.addr;
          instruct[1].data = input.value & 0xff;
          instruct[1].OK.second = false;
          instruct[0].type = 's';
          instruct[0].addr = input.addr + 1;
          instruct[0].data = (input.value >> 8) & 0xff;
          instruct[0].OK.second = false;
          instruct_size = 2;
          break;
        }
        case SW: {
          unsigned int res = input.value;
          instruct[0].type = 's';
          instruct[0].addr = input.addr + 3;
          instruct[0].data = (res >> 24) & 0xff;
          instruct[0].OK.second = false;
          instruct[1].type = 's';
          instruct[1].addr = input.addr + 2;
          instruct[1].data = (res >> 16) & 0xff;
          instruct[1].OK.second = false;
          instruct[2].type = 's';
          instruct[2].addr = input.addr + 1;
          instruct[2].data = (res >> 8) & 0xff;
          instruct[2].OK.second = false;
          instruct[3].type = 's';
          instruct[3].addr = input.addr;
          instruct[3].data = input.value & 0xff;
          instruct[3].OK.second = false;
          instruct_size = 4;
          break;
        }
        default: {
          throw(input.type);
        }
        }
      }
      input.clk++;
    } else {
      return;
    }
    for (int i = 0; i < instruct_size; i++) {
      if (!instruct[i].OK.second) {
        if (instruct[i].type == 's') {
          instruct[i].OK = write(instruct[i].addr, instruct[i].data, input.clk);
        } else {
          instruct[i].OK = load(instruct[i].addr, input.clk);
        }
      }
    }
    if (output.busy) {
      return;
    }
    bool complete = true;
    for (int i = 0; i < instruct_size; i++) {
      if (!instruct[i].OK.second) {
        complete = false;
        break;
      }
    }
    if (complete) {
      if (instruct[0].type == 's') {
        unsigned int res = 0;
        for (int i = 0; i < instruct_size; i++) {
          res <<= 8;
          res += instruct[i].OK.first;
        }
        if ((!reset) && (input.target != -1)) {
          reserved[reserve_size].value = res;
          reserved[reserve_size].type = input.type;
          reserved[reserve_size].addr = input.addr;
          reserved[reserve_size].clk = -1;
          reserved[reserve_size++].target = -1;
        }
        output.value = input.value;
        output.busy = true;
        output.target = input.target;
      } else {
        unsigned int res = 0;
        for (int i = 0; i < instruct_size; i++) {
          res <<= 8;
          res += instruct[i].OK.first;
        }
        if (input.type == LB) {
          if (((res >> 7) & 1) != 0) {
            res |= 0xffffff00;
          }
        }
        if (input.type == LH) {
          if (((res >> 15) & 1) != 0) {
            res |= 0xffff0000;
          }
        }
        output.value = res;
        output.busy = true;
        output.target = input.target;
      }
      input.clk = -1;
    }
    if (output.target == -1) {
      output.busy = false;
    }
    return;
  }
  void update() {
    reserve_size--;
    for (int i = 0; i < reserve_size; i++) {
      reserved[i] = reserved[i + 1];
    }
    return;
  }
  void snap() {
    reset = true;
    output.busy = false;
    input.clk = -1;
    return;
  }
  int hit(int addr) {
    for (int i = 0; i < 10; i++) {
      if (cache[i].busy) {
        if ((addr & 0xffffff00) == cache[i].addr) {
          return i;
        }
      }
    }
    return -1;
  }
  void loadblock(int addr, int clk) {
    if (hit(addr) != -1) {
      return;
    }
    if (clk <= 2) {
      return;
    }
    for (int i = 0; i < 10; i++) {
      if (!cache[i].busy) {
        cache_size++;
        cache[i].addr = addr & 0xffffff00;
        for (int j = 0; j < 256; j++) {
          cache[i].mem[j] = mem[cache[i].addr + j];
        }
        cache[i].busy = true;
        for (int j = 0; j < 10; j++) {
          if (cache[j].busy) {
            cache[j].recently = 0;
          }
        }
        return;
      }
    }
    int far = cache[0].recently;
    int num = 0;
    for (int i = 0; i < 10; i++) {
      if (far < cache[i].recently) {
        far = cache[i].recently;
        num = i;
      }
    }
    writeblock(cache[num]);
    cache[num].addr = addr & 0xffffff00;
    for (int i = 0; i < 10; i++) {
      cache[i].recently = 0;
    }
    for (int i = 0; i < 256; i++) {
      cache[num].mem[i] = mem[(addr & 0xffffff00) + i];
    }
    return;
  }
  void writeblock(const CacheBlock &data) {
    for (int i = 0; i < 256; i++) {
      mem[data.addr + i] = data.mem[i];
    }
    return;
  }
  std::pair<unsigned char, bool> write(int addr, unsigned char data, int clk) {
    loadblock(addr, clk);
    int num = hit(addr);
    if (num != -1) {
      unsigned char res = cache[num].mem[addr & 0xff];
      cache[num].mem[addr & 0xff] = data;
      cache[num].recently = 0;
      for (int i = 0; i < 10; i++) {
        if (i != num) {
          cache[i].recently++;
        }
      }
      return std::pair(res, true);
    }
    return std::pair(0, false);
  }
  std::pair<unsigned char, bool> load(int addr, int clk) {
    loadblock(addr, clk);
    int num = hit(addr);
    if (num != -1) {
      cache[num].recently = 0;
      for (int i = 0; i < 10; i++) {
        if (i != num) {
          cache[i].recently++;
        }
      }
      return std::pair(cache[num].mem[addr & 0xff], true);
    }
    return std::pair(0, false);
  }
};
} // namespace Yuchuan
#endif