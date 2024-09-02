#include "log_generator.h"

char *reg_names[32] = {
    "zero", "ra",   "sp",   "gp",
    "tp",   "t0",   "t1",   "t2",
    "s0",   "s1",   "a0",   "a1",
    "a2",   "a3",   "a4",   "a5",
    "a6",   "a7",   "s2",   "s3",
    "s4",   "s5",   "s6",   "s7",
    "s8",   "s9",   "s10",  "s11",
    "t3",   "t4",   "t5",   "t6"
};

void print_dissassembly(InstData inst_data, char *dissassembly){
    
    dissassembly[0] = 0; //make sure we are starting with a null string
    char aux_str[30];
    int shamt;


    if(inst_data.opcode == LUI_OPCODE){ //1
        sprintf(aux_str, "lui     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %d", reg_names[inst_data.rd], ((unsigned)inst_data.imm >> 12) );
        strcat(dissassembly, aux_str);

    } else if(inst_data.opcode == AUIPC_OPCODE){ //2
        sprintf(aux_str, "auipc   ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %d", reg_names[inst_data.rd], ((unsigned)inst_data.imm >> 12) );
        strcat(dissassembly, aux_str);

    } else if(inst_data.opcode == JAL_OPCODE){ //3
        sprintf(aux_str, "jal     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %d", reg_names[inst_data.rd], inst_data.imm);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == JALR_OPCODE &&
                inst_data.funct3 == JALR_FUNCT3){ //4
        sprintf(aux_str, "jalr    ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %d(%s)", reg_names[inst_data.rd], inst_data.imm, reg_names[inst_data.rs1]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == BEQ_OPCODE &&
                inst_data.funct3 == BEQ_FUNCT3){ //5
        sprintf(aux_str, "beq     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rs1], reg_names[inst_data.rs2], inst_data.imm);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == BNE_OPCODE &&
                inst_data.funct3 == BNE_FUNCT3){ //6
        sprintf(aux_str, "bne     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rs1], reg_names[inst_data.rs2], inst_data.imm);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == BLT_OPCODE &&
                inst_data.funct3 == BLT_FUNCT3){ //7
        sprintf(aux_str, "blt     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rs1], reg_names[inst_data.rs2], inst_data.imm);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == BGE_OPCODE &&
                inst_data.funct3 == BGE_FUNCT3){ //8
        sprintf(aux_str, "bge     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rs1], reg_names[inst_data.rs2], inst_data.imm);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == BLTU_OPCODE &&
                inst_data.funct3 == BLTU_FUNCT3){ //9
        sprintf(aux_str, "bltu    ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rs1], reg_names[inst_data.rs2], inst_data.imm);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == BGEU_OPCODE &&
                inst_data.funct3 == BGEU_FUNCT3){ //10
        sprintf(aux_str, "bgeu    ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rs1], reg_names[inst_data.rs2], inst_data.imm);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == LB_OPCODE &&
                inst_data.funct3 == LB_FUNCT3){ //11
        sprintf(aux_str, "lb      ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %d(%s)", reg_names[inst_data.rd], inst_data.imm, reg_names[inst_data.rs1]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == LH_OPCODE &&
                inst_data.funct3 == LH_FUNCT3){ //12
        sprintf(aux_str, "lh      ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %d(%s)", reg_names[inst_data.rd], inst_data.imm, reg_names[inst_data.rs1]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == LW_OPCODE &&
                inst_data.funct3 == LW_FUNCT3){ //13
        sprintf(aux_str, "lw      ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %d(%s)", reg_names[inst_data.rd], inst_data.imm, reg_names[inst_data.rs1]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == LBU_OPCODE &&
                inst_data.funct3 == LBU_FUNCT3){ //14
        sprintf(aux_str, "lbu     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %d(%s)", reg_names[inst_data.rd], inst_data.imm, reg_names[inst_data.rs1]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == LHU_OPCODE &&
                inst_data.funct3 == LHU_FUNCT3){ //15
        sprintf(aux_str, "lhu     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %d(%s)", reg_names[inst_data.rd], inst_data.imm, reg_names[inst_data.rs1]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SB_OPCODE &&
                inst_data.funct3 == SB_FUNCT3){ //16
        sprintf(aux_str, "sb      ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %d(%s)", reg_names[inst_data.rs2], inst_data.imm, reg_names[inst_data.rs1]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SH_OPCODE &&
                inst_data.funct3 == SH_FUNCT3){ //17
        sprintf(aux_str, "sh      ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %d(%s)", reg_names[inst_data.rs2], inst_data.imm, reg_names[inst_data.rs1]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SW_OPCODE &&
                inst_data.funct3 == SW_FUNCT3){ //18
        sprintf(aux_str, "sw      ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %d(%s)", reg_names[inst_data.rs2], inst_data.imm, reg_names[inst_data.rs1]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == ADDI_OPCODE &&
                inst_data.funct3 == ADDI_FUNCT3){ //19
        sprintf(aux_str, "addi    ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rd], reg_names[inst_data.rs1], inst_data.imm);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SLTI_OPCODE &&
                inst_data.funct3 == SLTI_FUNCT3){ //20
        sprintf(aux_str, "slti    ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rd], reg_names[inst_data.rs1], inst_data.imm);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SLTIU_OPCODE &&
                inst_data.funct3 == SLTIU_FUNCT3){ //21
        sprintf(aux_str, "sltiu   ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rd], reg_names[inst_data.rs1], inst_data.imm);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == XORI_OPCODE &&
                inst_data.funct3 == XORI_FUNCT3){ //22
        sprintf(aux_str, "xori    ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rd], reg_names[inst_data.rs1], inst_data.imm);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == ORI_OPCODE &&
                inst_data.funct3 == ORI_FUNCT3){ //23
        sprintf(aux_str, "ori     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rd], reg_names[inst_data.rs1], inst_data.imm);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == ANDI_OPCODE &&
                inst_data.funct3 == ANDI_FUNCT3){ //24
        sprintf(aux_str, "andi    ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rd], reg_names[inst_data.rs1], inst_data.imm);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SLLI_OPCODE &&
                inst_data.funct3 == SLLI_FUNCT3 &&
                inst_data.funct7 == SLLI_PSEUDO_FUNCT7){ //25
        sprintf(aux_str, "slli    ");
        strcat(dissassembly, aux_str);
        shamt = inst_data.imm & 0b111111;
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rd], reg_names[inst_data.rs1], shamt);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SRLI_OPCODE &&
                inst_data.funct3 == SRLI_FUNCT3 &&
                inst_data.funct7 == SRLI_PSEUDO_FUNCT7){ //26
        sprintf(aux_str, "srli    ");
        strcat(dissassembly, aux_str);
        shamt = inst_data.imm & 0b111111;
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rd], reg_names[inst_data.rs1], shamt);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SRAI_OPCODE &&
                inst_data.funct3 == SRAI_FUNCT3 &&
                inst_data.funct7 == SRAI_PSEUDO_FUNCT7){ //27
        sprintf(aux_str, "srai    ");
        strcat(dissassembly, aux_str);
        shamt = inst_data.imm & 0b111111;
        sprintf(aux_str, "%s, %s, %d", reg_names[inst_data.rd], reg_names[inst_data.rs1], shamt);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == ADD_OPCODE &&
                inst_data.funct3 == ADD_FUNCT3 &&
                inst_data.funct7 == ADD_FUNCT7){ //28
        sprintf(aux_str, "add     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SUB_OPCODE &&
                inst_data.funct3 == SUB_FUNCT3 &&
                inst_data.funct7 == SUB_FUNCT7){//29
        sprintf(aux_str, "sub     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SLL_OPCODE &&
                inst_data.funct3 == SLL_FUNCT3 &&
                inst_data.funct7 == SLL_FUNCT7){//30
        sprintf(aux_str, "sll     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SLT_OPCODE &&
                inst_data.funct3 == SLT_FUNCT3 &&
                inst_data.funct7 == SLT_FUNCT7){//31
        sprintf(aux_str, "slt     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SLTU_OPCODE &&
                inst_data.funct3 == SLTU_FUNCT3 &&
                inst_data.funct7 == SLTU_FUNCT7){//32{
        sprintf(aux_str, "sltu    ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == XOR_OPCODE &&
                inst_data.funct3 == XOR_FUNCT3 &&
                inst_data.funct7 == XOR_FUNCT7){//33
        sprintf(aux_str, "xor     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SRL_OPCODE &&
                inst_data.funct3 == SRL_FUNCT3 &&
                inst_data.funct7 == SRL_FUNCT7){//34
        sprintf(aux_str, "srl     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == SRA_OPCODE &&
                inst_data.funct3 == SRA_FUNCT3 &&
                inst_data.funct7 == SRA_FUNCT7){//35
        sprintf(aux_str, "sra     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == OR_OPCODE &&
                inst_data.funct3 == OR_FUNCT3 &&
                inst_data.funct7 == OR_FUNCT7){ //36
        sprintf(aux_str, "or      ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == AND_OPCODE &&//37
                inst_data.funct3 == AND_FUNCT3 &&
                inst_data.funct7 == AND_FUNCT7){ 
        sprintf(aux_str, "and     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == ECALL_OPCODE &&
                inst_data.imm == 0b000000000000){//38 
        sprintf(aux_str, "ecall   ");
        strcat(dissassembly, aux_str);
        
    } else if(  inst_data.opcode == EBREAK_OPCODE &&
                inst_data.imm == 0b000000000001){//39 
        sprintf(aux_str, "ebreak  ");
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == MUL_OPCODE &&
                inst_data.funct3 == MUL_FUNCT3 &&
                inst_data.funct7 == MUL_FUNCT7){ //40
        sprintf(aux_str, "mul     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == MULH_OPCODE &&
                inst_data.funct3 == MULH_FUNCT3 &&
                inst_data.funct7 == MULH_FUNCT7){ //41
        sprintf(aux_str, "mulh    ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == MULHSU_OPCODE &&
                inst_data.funct3 == MULHSU_FUNCT3 &&
                inst_data.funct7 == MULHSU_FUNCT7){//42 
        sprintf(aux_str, "mulhsu  ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == MULHU_OPCODE &&
                inst_data.funct3 == MULHU_FUNCT3 &&
                inst_data.funct7 == MULHU_FUNCT7){//43
        sprintf(aux_str, "mulhu   ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == DIV_OPCODE &&
                inst_data.funct3 == DIV_FUNCT3 &&
                inst_data.funct7 == DIV_FUNCT7){ //44
        sprintf(aux_str, "div     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == DIVU_OPCODE &&
                inst_data.funct3 == DIVU_FUNCT3 &&
                inst_data.funct7 == DIVU_FUNCT7){//45 
        sprintf(aux_str, "divu    ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == REM_OPCODE &&
                inst_data.funct3 == REM_FUNCT3 &&
                inst_data.funct7 == REM_FUNCT7){ //46
        sprintf(aux_str, "rem     ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else if(  inst_data.opcode == REMU_OPCODE &&
                inst_data.funct3 == REMU_FUNCT3 &&
                inst_data.funct7 == REMU_FUNCT7){ //47
        sprintf(aux_str, "remu    ");
        strcat(dissassembly, aux_str);
        sprintf(aux_str, "%s, %s, %s", reg_names[inst_data.rd], reg_names[inst_data.rs1], reg_names[inst_data.rs2]);
        strcat(dissassembly, aux_str);

    } else{
        sprintf(aux_str, "invalid ");
        strcat(dissassembly, aux_str);
    }

    strcat(dissassembly, "\n");

    return;
}

void generate_dissassembly_log( InstData *inst_list, char *file_path, int text_size){
    int i;
    FILE *fptr;
    char aux_str[30];

    printf("\n\nSTARTING TO LOG THE INSTRUCTIONS...\n\n");
    fptr = fopen(file_path, "w");

    if (fptr == 0){
        printf("Failed to open file\n");
    }

    for (i = 0; i < text_size/4; i++){
        
        print_dissassembly(inst_list[i], aux_str);
        
        fwrite(aux_str, strlen(aux_str), 1, fptr);
    }


    fclose(fptr);

    printf("\n\nFINISHED TO LOG THE INSTRUCTIONS...\n\n");
    
    
    return;
}

void print_exec_info(ExecLogEntry *execlog_entry, char *exec_info){
    
    exec_info[0] = 0; //make sure we are starting with a null string
    char aux_str[80];
    //renaming
    int PC = execlog_entry->PC,
    raw_instruction = execlog_entry->inst_data.raw_instruction,
    rd_index = execlog_entry->rd_index,
    rs1_index = execlog_entry->rs1_index,
    rs2_index = execlog_entry->rs2_index,
    rd_value = execlog_entry->rd_value,
    rs1_value = execlog_entry->rs1_value,
    rs2_value = execlog_entry->rs2_value;

    sprintf(exec_info, "PC=%08x [%08x] x%02d=%08x x%02d=%08x x%02d=%08x ", PC, raw_instruction, rd_index, rd_value, rs1_index, rs1_value, rs2_index, rs2_value);

    return;
}

void print_log_line(ExecLogEntry *execlog_entry, char* log_line){

    log_line[0] = 0;
    char aux_str[80];

    print_exec_info(execlog_entry, aux_str);
    strcat(log_line, aux_str);
    print_dissassembly(execlog_entry->inst_data, aux_str);
    strcat(log_line, aux_str);

    return;
}

void generate_log(ExecLogEntry *exec_log, char* file_path, int exec_log_size){
    int i;
    FILE *fptr;
    char aux_str[100];

    printf("\n\nSTARTING TO LOG THE SIMULATION...\n\n");
    fptr = fopen(file_path, "w");

    if (fptr == 0){
        printf("Failed to open file\n");
    }

    for (i = 0; i < exec_log_size; i++){
        
        print_log_line(&exec_log[i], aux_str);
        
        fwrite(aux_str, strlen(aux_str), 1, fptr);
    }


    fclose(fptr);

    printf("\n\nFINISHED TO LOG THE SIMULATION...\n\n");

    return;
};