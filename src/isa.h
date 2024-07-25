#ifndef ISA_H
#define ISA_H
namespace Yuchuan {
enum opcode {
  ADD,
  AND,
  OR,
  SLL,
  SRL,
  SLT,
  SLTU,
  SRA,
  SUB,
  XOR,
  LB,
  LBU,
  LH,
  LHU,
  LW,
  SB,
  SH,
  SW,
  AUIPC,//This instruction doesn't need to emit to somewhere else.
  BEQ,
  BGE,
  BLT,
  BNE,
  BGEU,
  BLTU,
  JAL,//This instruction doesn't need to emit to somewhere else.
  JALR,
  LUI
};
}
#endif