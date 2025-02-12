#include "alu.h"
#include "decoder.h"
#include "isa.h"
#include "lsb.h"
#include "memory.h"
#include "rf.h"
#include "predictor.h"
#include "rob.h"
#include "rs.h"
#include <iostream>
#include <string>
#ifndef CPU_H
#define CPU_H
namespace Yuchuan {
struct CPU {
  int reg[32];
  ALU alu;
  Memory memory;
  RF rf;
  RS rs;
  LSB lsb;
  ROB rob;
  Predictor pre;
  int now_pc = 0;
  int advanced_pc = 0;
  int last_pc = 0;
  int correct = 0;
  int total = 0;
  int clk = 0;
  bool predictions[8];
  int pre_size = 0;
  bool stop = false;
  std::pair<bool, unsigned char> work() {
    clk++;
    if (now_pc == -1) {
      std::cerr << "Prediction accuracy:" << (double(correct) / double(total) * 100) << "%\n";
      std::cerr << "Total clock cycles:" << clk << std::endl;
      return std::pair<bool, unsigned char>(true, reg[10]);
    }
    GetInstruct();
    WireRSOut();
    WireLSBOut();
    WireALUOut();
    WireMemoryOut();
    WireRobOut();
    if (!memory.reset) {
      alu.work();
      lsb.work();
      rob.work();
      rs.work();
    }
    memory.work();
    return std::pair<bool, unsigned char>(false, 0);
  }
  void GetInstruct() {
    if ((!stop) && (!memory.reset)) {
      unsigned char code[4] = {
          memory.mem[advanced_pc], memory.mem[advanced_pc + 1],
          memory.mem[advanced_pc + 2], memory.mem[advanced_pc + 3]};
      RawInstruction instruct = decode(code);
      if (rob.size < 7) {
        Buffer to_push;
        to_push.now_pc = advanced_pc;
        to_push.type = instruct.type;
        to_push.busy = true;
        to_push.OK = false;
        int num = rob.tail;
        if (instruct.type == HALT) {
          to_push.des = -1;
          to_push.OK = true;
          to_push.value = 0;
          stop = true;
          rob.input = to_push;
        }
        if (instruct.type == AUIPC) {
          to_push.des = instruct.rd;
          to_push.OK = true;
          to_push.value = advanced_pc + instruct.imm;
          rob.input = to_push;
          advanced_pc += instruct.imm;
          if (to_push.des != 0) {
            rf.dependency[to_push.des] = num;
          }
        }
        if (instruct.type == LUI) {
          to_push.des = instruct.rd;
          to_push.OK = true;
          to_push.value = instruct.imm;
          rob.input = to_push;
          advanced_pc += 4;
          if (to_push.des != 0) {
            rf.dependency[to_push.des] = num;
          }
        }
        if (instruct.type == JAL) {
          to_push.des = instruct.rd;
          to_push.OK = true;
          to_push.value = instruct.imm;
          rob.input = to_push;
          advanced_pc += instruct.imm;
          if (to_push.des != 0) {
            rf.dependency[to_push.des] = num;
          }
        }
        if (instruct.type < 34 && (!rs.input.busy)) {
          ReservedInstruct new_instruct;
          new_instruct.busy = true;
          new_instruct.des = num; // To show the number in rob.
          new_instruct.opcode = instruct.type;
          if (instruct.type <= 2) {
            to_push.des = -1;
            new_instruct.imm = instruct.imm;
            if (rf.dependency[instruct.rs1] != -1 &&
                (rf.dependency[instruct.rs1] != num)) {
              new_instruct.query[0] = rf.dependency[instruct.rs1];
            } else {
              new_instruct.value[0] = reg[instruct.rs1];
            }
            if (rf.dependency[instruct.rs2] != -1 &&
                (rf.dependency[instruct.rs2] != num)) {
              new_instruct.query[1] = rf.dependency[instruct.rs2];
            } else {
              new_instruct.value[1] = reg[instruct.rs2];
            }
            advanced_pc += 4;
          }
          if (instruct.type == JALR) {
            to_push.des = instruct.rd;
            stop = true;
            new_instruct.imm = instruct.imm;
            if (rf.dependency[instruct.rs1] != -1 &&
                (rf.dependency[instruct.rs1] != num)) {
              new_instruct.query[0] = rf.dependency[instruct.rs1];
            } else {
              new_instruct.value[0] = reg[instruct.rs1];
            }
            if (instruct.rd != 0) {
              rf.dependency[instruct.rd] = num;
            }
          }
          if ((instruct.type > 2) && (instruct.type < 17)) {
            to_push.des = instruct.rd;
            new_instruct.imm = instruct.imm;
            if (rf.dependency[instruct.rs1] != -1 &&
                (rf.dependency[instruct.rs1] != num)) {
              new_instruct.query[0] = rf.dependency[instruct.rs1];
            } else {
              new_instruct.value[0] = reg[instruct.rs1];
            }
            if (instruct.rd != 0) {
              rf.dependency[instruct.rd] = num;
            }
            advanced_pc += 4;
          }
          if ((instruct.type > 17) && (instruct.type <= 27)) {
            to_push.des = instruct.rd;
            if (rf.dependency[instruct.rs1] != -1 &&
                (rf.dependency[instruct.rs1] != num)) {
              new_instruct.query[0] = rf.dependency[instruct.rs1];
            } else {
              new_instruct.value[0] = reg[instruct.rs1];
            }
            if (rf.dependency[instruct.rs2] != -1 &&
                (rf.dependency[instruct.rs2] != num)) {
              new_instruct.query[1] = rf.dependency[instruct.rs2];
            } else {
              new_instruct.value[1] = reg[instruct.rs2];
            }
            if (instruct.rd != 0) {
              rf.dependency[instruct.rd] = num;
            }
            advanced_pc += 4;
          }
          if ((instruct.type > 27) && (instruct.type <= 33)) {
            to_push.des = instruct.imm;
            new_instruct.imm = instruct.imm;
            if (rf.dependency[instruct.rs1] != -1 &&
                (rf.dependency[instruct.rs1] != num)) {
              new_instruct.query[0] = rf.dependency[instruct.rs1];
            } else {
              new_instruct.value[0] = reg[instruct.rs1];
            }
            if (rf.dependency[instruct.rs2] != -1 &&
                (rf.dependency[instruct.rs2] != num)) {
              new_instruct.query[1] = rf.dependency[instruct.rs2];
            } else {
              new_instruct.value[1] = reg[instruct.rs2];
            }
            if (pre.Predict(advanced_pc)) {
              predictions[pre_size++] = true;
              advanced_pc += instruct.imm;
            } else {
              predictions[pre_size++] = false;
              advanced_pc += 4;
            }
          }
          rs.input = new_instruct;
          rob.input = to_push;
        }
      }
    }
  }
  void WireRSOut() {
    if (rs.output.busy) {
      if (rs.output.opcode <= 7) {
        if (rs.output.opcode == -1) {
          bool check = true;
        }
        if (!lsb.input.busy) {
          rs.output.busy = false;
          LSBInstruct lsb_ins;
          lsb_ins.busy = true;
          lsb_ins.addr = rs.output.value[0] + rs.output.imm;
          lsb_ins.value = rs.output.value[1];
          lsb_ins.des = rs.output.des;
          lsb_ins.type = rs.output.opcode;
          lsb.input = lsb_ins;
        }
      } else {
        if (!alu.input.busy) {
          rs.output.busy = false;
          AluInstruct alu_ins;
          alu_ins.busy = true;
          alu_ins.opcode = rs.output.opcode;
          alu_ins.target = rs.output.des;
          alu_ins.value1 = rs.output.value[0];
          if (alu_ins.opcode >= 17) {
            alu_ins.value2 = rs.output.value[1];
            if (alu_ins.opcode == JALR) {
              alu_ins.opcode = ADD;
            }
          } else {
            alu_ins.value2 = rs.output.imm;
            switch (alu_ins.opcode) {
            case ADDI: {
              alu_ins.opcode = ADD;
              break;
            }
            case SLTI: {
              alu_ins.opcode = SLT;
              break;
            }
            case SLTIU: {
              alu_ins.opcode = SLTU;
              break;
            }
            case XORI: {
              alu_ins.opcode = XOR;
              break;
            }
            case ORI: {
              alu_ins.opcode = OR;
              break;
            }
            case ANDI: {
              alu_ins.opcode = AND;
              break;
            }
            case SLLI: {
              alu_ins.opcode = SLL;
              break;
            }
            case SRLI: {
              alu_ins.opcode = SRL;
              break;
            }
            case SRAI: {
              alu_ins.opcode = SRA;
              break;
            }
            default: {
              throw(alu_ins.opcode);
            }
            }
          }
          alu.input = alu_ins;
        }
      }
    }
  }
  void WireLSBOut() {
    if (lsb.output.busy) {
      if (memory.input.clk == -1) {
        LsInput to_ls;
        to_ls.addr = lsb.output.addr;
        to_ls.clk = 0;
        to_ls.type = lsb.output.type;
        to_ls.target = lsb.output.des;
        to_ls.value = lsb.output.value;
        memory.input = to_ls;
        lsb.output.busy = false;
      }
    }
  }
  void WireALUOut() {
    if (alu.output.busy) {
      rob.buffer[alu.output.target].value = alu.output.value;
      rob.buffer[alu.output.target].OK = true;
      alu.output.busy = false;
    }
  }
  void WireMemoryOut() {
    if (memory.output.busy) {
      rob.buffer[memory.output.target].value = memory.output.value;
      rob.buffer[memory.output.target].OK = true;
      memory.output.busy = false;
    }
  }
  void WireRobOut() {
    if (rob.output.data.busy) {
      rob.output.data.busy = false;
      for (int i = 0; i < 32; i++) {
        if (rf.dependency[i] == rob.output.num) {
          rf.dependency[i] = -1;
        }
      } // This part is to change the data of RF.
      if (rs.input.query[0] == rob.output.num) {
        rs.input.query[0] = -1;
        if (rob.output.data.type == JAL || rob.output.data.type == JALR) {
          rs.input.value[0] = now_pc + 4;
        } else {
          rs.input.value[0] = rob.output.data.value;
        }
      }
      if (rs.input.query[1] == rob.output.num) {
        rs.input.query[1] = -1;
        if (rob.output.data.type == JAL || rob.output.data.type == JALR) {
          rs.input.value[1] = now_pc + 4;
        } else {
          rs.input.value[1] = rob.output.data.value;
        }
      }
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 2; j++) {
          if (rs.reserve[i].query[j] == rob.output.num) {
            rs.reserve[i].query[j] = -1;
            if (rob.output.data.type == JAL || rob.output.data.type == JALR) {
              rs.reserve[i].value[j] = now_pc + 4;
            } else {
              rs.reserve[i].value[j] = rob.output.data.value;
            }
          }
        }
      } // This part is to modify RS.
      if (rob.output.data.type <= 2) {
        memory.update();
        now_pc += 4;
      }
      if ((rob.output.data.type > 2) && (rob.output.data.type <= 27) &&
          (rob.output.data.type != JALR)) {
        if (rob.output.data.des != 0) {
          reg[rob.output.data.des] = rob.output.data.value;
        }
        now_pc += 4;
      }
      if (rob.output.data.type == LUI) {
        if (rob.output.data.des != 0) {
          reg[rob.output.data.des] = rob.output.data.value;
        }
        now_pc += 4;
      }
      if (rob.output.data.type == AUIPC) {
        now_pc += rob.output.data.value;
        if (rob.output.data.des != 0) {
          reg[rob.output.data.des] = now_pc;
        }
      }
      if (rob.output.data.type == HALT) {
        now_pc = -1;
      }
      if (rob.output.data.type == JALR) {
        if (rob.output.data.des != 0) {
          reg[rob.output.data.des] = (now_pc + 4);
        }
        now_pc = rob.output.data.value;
        advanced_pc = rob.output.data.value;
        stop = false;
      }
      if (rob.output.data.type == JAL) {
        if (rob.output.data.des != 0) {
          reg[rob.output.data.des] = (now_pc + 4);
        }
        now_pc += rob.output.data.value;
      }
      if (rob.output.data.type > 27 && rob.output.data.type <= 33) {
        if (rob.output.data.value != 0) {
          pre.update(now_pc, true);
          total++;
          now_pc += rob.output.data.des;
          if (predictions[0]) {
            correct++;
            pre_size--;
            for (int i = 0; i < pre_size; i++) {
              predictions[i] = predictions[i + 1];
            }
          } else {
            advanced_pc = now_pc;
            stop = false;
            alu.clean();
            lsb.clean();
            rf.clean();
            rob.clean();
            rs.clean();
            memory.snap();
            pre_size = 0;
          }
        } else {
          total++;
          now_pc += 4;
          pre.update(now_pc, false);
          if (!predictions[0]) {
            correct++;
            pre_size--;
            for (int i = 0; i < pre_size; i++) {
              predictions[i] = predictions[i + 1];
            }
          } else {
            advanced_pc = now_pc;
            stop = false;
            alu.clean();
            lsb.clean();
            rf.clean();
            rob.clean();
            rs.clean();
            memory.snap();
            pre_size = 0;
          }
        }
      }
    }
  }
};
} // namespace Yuchuan
#endif