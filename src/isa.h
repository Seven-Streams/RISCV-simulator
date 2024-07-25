#ifndef ISA_H
#define ISA_H
namespace Yuchuan {
enum opcode {
  SB,
  SH,
  SW, // S type, 2
  LB,
  LBU,
  LH,
  LHU,
  LW,
  ADDI,
  SLTI,
  SLTIU,
  XORI,
  ORI,
  ANDI,
  SLLI,
  SRLI,
  SRAI,
  JALR, // I type, 17
  ADD,
  AND,
  OR,
  SLL,
  SRL,
  SLT,
  SLTU,
  SRA,
  SUB,
  XOR, // R type, 27
  BEQ,
  BGE,
  BLT,
  BNE,
  BGEU,
  BLTU, // B type, 33
  JAL, // This instruction doesn't need to emit to somewhere else. J type, 34.
  LUI,
  AUIPC, // This instruction doesn't need to emit to somewhere else. U type, 36.
  HALT
};
}
#endif