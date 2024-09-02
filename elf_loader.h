#ifndef _ELF_LOADER_H_
#define _ELF_LOADER_H_

#include <stdio.h>
#include "my_types.h"

#define LUI_OPCODE 0b0110111
#define AUIPC_OPCODE 0b0010111
#define JAL_OPCODE 0b1101111
#define JALR_OPCODE 0b1100111
#define JALR_FUNCT3 0b000
#define BEQ_OPCODE 0b1100011
#define BEQ_FUNCT3 0b000
#define BNE_OPCODE 0b1100011
#define BNE_FUNCT3 0b001
#define BLT_OPCODE 0b1100011
#define BLT_FUNCT3 0b100
#define BGE_OPCODE 0b1100011
#define BGE_FUNCT3 0b101
#define BLTU_OPCODE 0b1100011
#define BLTU_FUNCT3 0b110
#define BGEU_OPCODE 0b1100011
#define BGEU_FUNCT3 0b111
#define LB_OPCODE 0b0000011
#define LB_FUNCT3 0b000
#define LH_OPCODE 0b0000011
#define LH_FUNCT3 0b001
#define LW_OPCODE 0b0000011
#define LW_FUNCT3 0b010
#define LBU_OPCODE 0b0000011 
#define LBU_FUNCT3 0b100
#define LHU_OPCODE 0b0000011 
#define LHU_FUNCT3 0b101
#define SB_OPCODE 0b0100011 
#define SB_FUNCT3 0b000
#define SH_OPCODE 0b0100011 
#define SH_FUNCT3 0b001
#define SW_OPCODE 0b0100011 
#define SW_FUNCT3 0b010
#define ADDI_OPCODE 0b0010011 
#define ADDI_FUNCT3 0b000
#define SLTI_OPCODE 0b0010011 
#define SLTI_FUNCT3 0b010
#define SLTIU_OPCODE 0b0010011 
#define SLTIU_FUNCT3 0b011
#define XORI_OPCODE 0b0010011 
#define XORI_FUNCT3 0b100 
#define ORI_OPCODE 0b0010011 
#define ORI_FUNCT3 0b110
#define ANDI_OPCODE 0b0010011 
#define ANDI_FUNCT3 0b111
// The instructions below use the 6 msb of the immediate as an identification. I call those bits pseudo funct7
#define SLLI_OPCODE 0b0010011 
#define SLLI_FUNCT3 0b001
#define SLLI_PSEUDO_FUNCT7 0b000000
#define SRLI_OPCODE 0b0010011 
#define SRLI_FUNCT3 0b101
#define SRLI_PSEUDO_FUNCT7 0b000000
#define SRAI_OPCODE 0b0010011 
#define SRAI_FUNCT3 0b101
#define SRAI_PSEUDO_FUNCT7 0b010000
// Back to normality from here on
#define ADD_OPCODE 0b0110011 
#define ADD_FUNCT3 0b000  
#define ADD_FUNCT7 0b0000000 
#define SUB_OPCODE 0b0110011 
#define SUB_FUNCT3 0b000  
#define SUB_FUNCT7 0b0100000 
#define SLL_OPCODE 0b0110011 
#define SLL_FUNCT3 0b001  
#define SLL_FUNCT7 0b0000000 
#define SLT_OPCODE 0b0110011 
#define SLT_FUNCT3 0b010  
#define SLT_FUNCT7 0b0000000 
#define SLTU_OPCODE 0b0110011 
#define SLTU_FUNCT3 0b011  
#define SLTU_FUNCT7 0b0000000  
#define XOR_OPCODE 0b0110011 
#define XOR_FUNCT3 0b100  
#define XOR_FUNCT7 0b0000000  
#define SRL_OPCODE 0b0110011 
#define SRL_FUNCT3 0b101  
#define SRL_FUNCT7 0b0000000  
#define SRA_OPCODE 0b0110011 
#define SRA_FUNCT3 0b101  
#define SRA_FUNCT7 0b0100000  
#define OR_OPCODE 0b0110011 
#define OR_FUNCT3 0b110  
#define OR_FUNCT7 0b0000000  
#define AND_OPCODE 0b0110011 
#define AND_FUNCT3 0b111  
#define AND_FUNCT7 0b0000000
#define ECALL_OPCODE 0b1110011
#define EBREAK_OPCODE 0b1110011
// now these are RV32M instructions
#define MUL_OPCODE 0b0110011
#define MUL_FUNCT3 0b000
#define MUL_FUNCT7 0b0000001
#define MULH_OPCODE 0b0110011
#define MULH_FUNCT3 0b001
#define MULH_FUNCT7 0b0000001
#define MULHSU_OPCODE 0b0110011
#define MULHSU_FUNCT3 0b010
#define MULHSU_FUNCT7 0b0000001
#define MULHU_OPCODE 0b0110011
#define MULHU_FUNCT3 0b011
#define MULHU_FUNCT7 0b0000001
#define DIV_OPCODE 0b0110011
#define DIV_FUNCT3 0b100
#define DIV_FUNCT7 0b0000001
#define DIVU_OPCODE 0b0110011
#define DIVU_FUNCT3 0b101
#define DIVU_FUNCT7 0b0000001
#define REM_OPCODE 0b0110011
#define REM_FUNCT3 0b110
#define REM_FUNCT7 0b0000001
#define REMU_OPCODE 0b0110011
#define REMU_FUNCT3 0b111
#define REMU_FUNCT7 0b0000001  

/*
Only a few OPCODES are actually used:
+-------------------------------+-------------------------------+-----------+ 
|             TYPE              |        INSTRUCTIONS           |   AMOUNT  |
+-------------------------------+-------------------------------+-----------+
| I_TYPE1 = 7'b1100111;         | jalr                          | 1         |
+-------------------------------+-------------------------------+-----------+
| I_TYPE2 = 7'b0000011;         | lw, lh, lhu, lb, lbu          | 5         |
+-------------------------------+-------------------------------+-----------+
| I_TYPE3 = 7'b0010011;         | andi, ori, xori, slli, srli,  | 9         |
|                               | srai, addi, slti, sltiu       |           |
+-------------------------------+-------------------------------+-----------+
| S_TYPE = 7'b0100011;          | sw, sh, sb                    | 3         |
+-------------------------------+-------------------------------+-----------+
| B_TYPE = 7'b1100011;          | beq, bne, blt, bltu, bge,     | 6         |
|                               | bgeu                          |           |
+-------------------------------+-------------------------------+-----------+
| U_TYPE1 = 7'b0110111;         | lui                           | 1         |
+-------------------------------+-------------------------------+-----------+
| U_TYPE2 = 7'b0010111;         | auipc                         | 1         |
+-------------------------------+-------------------------------+-----------+
| J_TYPE = 7'b1101111;          | jal                           | 1         |
+-------------------------------+-------------------------------+-----------+
| R_TYPE = 7'b0110011;          | and, or, xor, sll, srl, sra,  | 18        |
|                               | add, sub, mul, mulh, mulhsu,  |           |
|                               | mulhu, div, divu, rem, remu,  |           |
|                               | slt, sltu                     |           |
+-------------------------------+-------------------------------+-----------+
| ECALL_EBREAK_TYPE             | ecall, ebreak                 | 2         |
+-------------------------------+-------------------------------+-----------+
|                             TOTAL                             | 47        |
+-------------------------------+-------------------------------+-----------+
*/

#define I_TYPE1 0b1100111
#define I_TYPE2 0b0000011
#define I_TYPE3 0b0010011
#define S_TYPE 0b0100011
#define B_TYPE 0b1100011
#define U_TYPE1 0b0110111
#define U_TYPE2 0b0010111
#define J_TYPE 0b1101111
#define R_TYPE 0b0110011
#define ECALL_EBREAK_TYPE 0b1110011

int sign_extend(int imm, int num_bits);

int extract_field(int input, int start_bit, int end_bit);

void load_elf_file(char *file_path, int entry, SectionHeader *text_section, SectionHeader *rodata_section, SectionHeader *data_section, SectionHeader *sdata_section,
                    TextSegment *text_seg, DataSegment *rodata_seg, DataSegment *data_segment, DataSegment *sdata_segment);

InstData decode_instruction(int raw_instruction);

#endif