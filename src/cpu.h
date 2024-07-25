#include "alu.h"
#include "decoder.h"
#include "isa.h"
#include "lsb.h"
#include "memory.h"
#include "rf.h"
#include "rob.h"
#include "rs.h"
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
  int now_pc = 0;
  int advanced_pc = 0;
  bool stop = false;
  std::pair<bool, unsigned char> work() {
    if (now_pc == -1) {
      return std::pair<bool, unsigned char>(true, reg[0]);
    }
    if (!stop) {
      unsigned char code[4] = {
          memory.mem[advanced_pc], memory.mem[advanced_pc + 1],
          memory.mem[advanced_pc + 2], memory.mem[advanced_pc + 3]};
      RawInstruction instruct = decode(code);
      if (!rob.input.busy) {
        Buffer to_push;
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
          rf.dependency[to_push.des] = num;
        }
        if (instruct.type == LUI) {
          to_push.des = instruct.rd;
          to_push.OK = true;
          to_push.value = instruct.imm;
          rob.input = to_push;
          advanced_pc += 4;
          rf.dependency[to_push.des] = num;
        }
        if (instruct.type == JAL) {
          to_push.des = instruct.rd;
          to_push.OK = true;
          to_push.value = instruct.imm;
          rob.input = to_push;
          advanced_pc += instruct.imm;
          rf.dependency[to_push.des] = num;
        }
        if (instruct.type < 34 && (!rs.input.busy)) {
          ReservedInstruct new_instruct;
          new_instruct.busy = true;
          new_instruct.des = num; // To show the number in rob.
          new_instruct.opcode = instruct.type;
          if (instruct.type <= 2) {
            to_push.des = -1;
            new_instruct.imm = instruct.imm;
            if (rf.dependency[instruct.rs1] != -1) {
              new_instruct.query[0] = rf.dependency[instruct.rs1];
            } else {
              new_instruct.value[0] = reg[instruct.rs1];
            }
            if (rf.dependency[instruct.rs2] != -1) {
              new_instruct.query[1] = rf.dependency[instruct.rs2];
            } else {
              new_instruct.value[1] = reg[instruct.rs2];
            }
            advanced_pc += 4;
          }
          if ((instruct.type > 2) && (instruct.type <= 17)) {
            to_push.des = instruct.rd;
            if (instruct.type == JALR) {
              advanced_pc -= 4;
              stop = true;
            }
            new_instruct.imm = instruct.imm;
            if (rf.dependency[instruct.rs1] != -1) {
              new_instruct.query[0] = rf.dependency[instruct.rs1];
            } else {
              new_instruct.value[0] = reg[instruct.rs1];
            }
            rf.dependency[instruct.rd] = num;
            advanced_pc += 4;
          }
          if ((instruct.type > 17) && (instruct.type <= 27)) {
            to_push.des = instruct.rd;
            if (rf.dependency[instruct.rs1] != -1) {
              new_instruct.query[0] = rf.dependency[instruct.rs1];
            } else {
              new_instruct.value[0] = reg[instruct.rs1];
            }
            if (rf.dependency[instruct.rs2] != -1) {
              new_instruct.query[1] = rf.dependency[instruct.rs2];
            } else {
              new_instruct.value[1] = reg[instruct.rs2];
            }
            advanced_pc += 4;
          }
          if ((instruct.type > 27) && (instruct.type <= 33)) {
            stop = true;
            to_push.des = instruct.imm;
            new_instruct.imm = instruct.imm;
            if (rf.dependency[instruct.rs1] != -1) {
              new_instruct.query[0] = rf.dependency[instruct.rs1];
            } else {
              new_instruct.value[0] = reg[instruct.rs1];
            }
            if (rf.dependency[instruct.rs2] != -1) {
              new_instruct.query[1] = rf.dependency[instruct.rs2];
            } else {
              new_instruct.value[1] = reg[instruct.rs2];
            }
            rf.dependency[instruct.rd] = num;
          }
          rs.input = new_instruct;
          rob.input = to_push;
        }
      }
    }
    if (rs.output.busy) {
      if (rs.output.opcode <= 7) {
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
    if (alu.output.busy) {
      rob.buffer[alu.output.target].value = alu.output.value;
      rob.buffer[alu.output.target].OK = true;
      alu.output.busy = false;
    }
    if (memory.output.busy) {
      rob.buffer[memory.output.target].value = memory.output.value;
      rob.buffer[memory.output.target].OK = true;
      memory.output.busy = false;
    }
    if (rob.output.data.busy) {
      rob.output.data.busy = false;
      for (int i = 0; i < 32; i++) {
        if (rf.dependency[i] == rob.output.num) {
          rf.dependency[i] = -1;
        }
      } // This part is to change the data of RF.
      if (rs.input.query[0] == rob.output.num) {
        rs.input.query[0] = -1;
        rs.input.value[0] = rob.output.data.value;
      }
      if (rs.input.query[1] == rob.output.num) {
        rs.input.query[1] = -1;
        rs.input.value[1] = rob.output.data.value;
      }
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 2; j++) {
          if (rs.reserve[i].query[j] == rob.output.num) {
            rs.reserve[i].query[j] = -1;
            rs.reserve[i].value[j] = rob.output.data.value;
          }
        }
      } // This part is to modify RS.
      if (rob.output.data.type <= 2) {
        now_pc += 4;
      }
      if ((rob.output.data.type > 2) && (rob.output.data.type <= 27) &&
          (rob.output.data.type != JALR)) {
        reg[rob.output.data.des] = rob.output.data.value;
        now_pc += 4;
      }
      if (rob.output.data.type == LUI) {
        reg[rob.output.data.des] = rob.output.data.value;
        now_pc += 4;
      }
      if (rob.output.data.type == AUIPC) {
        reg[rob.output.data.des] = rob.output.data.value;
        now_pc = rob.output.data.value;
      }
      if (rob.output.data.type == HALT) {
        now_pc = -1;
      }
      if (rob.output.data.type == JALR || rob.output.data.type == JAL) {
        reg[rob.output.data.des] = (now_pc + 4);
        now_pc += rob.output.data.value;
        stop = false;
      }
      if (rob.output.data.type > 27 && rob.output.data.type < 33) {
        if (rob.output.data.value != 0) {
          now_pc = rob.output.data.des;
        } else {
          now_pc += 4;
        }
        stop = false;
      }
    }
    alu.work();
    lsb.work();
    memory.work();
    rob.work();
    rs.work();
    return std::pair<bool, unsigned char>(false, 0);
  }
};
} // namespace Yuchuan
#endif