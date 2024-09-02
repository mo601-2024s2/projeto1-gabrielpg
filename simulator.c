#include "simulator.h"

int simulate_inst(int *PC, int *cycles, SimulatedMemory *simulated_memory, int reg_bank[], ExecLogEntry *exec_log_entry){

    int inst_index;
    int exec_code;

    exec_log_entry->PC = *PC;
    fetch_inst(PC, simulated_memory, &inst_index);
    exec_code = execute_inst(inst_index, simulated_memory, reg_bank, PC, cycles);

    //fill log entry
    exec_log_entry->inst_data = simulated_memory->text_seg.inst_list[inst_index];
    exec_log_entry->rd_index = simulated_memory->text_seg.inst_list[inst_index].rd;
    exec_log_entry->rs1_index = simulated_memory->text_seg.inst_list[inst_index].rs1;
    exec_log_entry->rs2_index = simulated_memory->text_seg.inst_list[inst_index].rs2;
    exec_log_entry->rd_value = reg_bank[exec_log_entry->rd_index];
    exec_log_entry->rs1_value = reg_bank[exec_log_entry->rs1_index];
    exec_log_entry->rs2_value = reg_bank[exec_log_entry->rs2_index];

    return exec_code;
}

void fetch_inst(int *PC, SimulatedMemory *simulated_memory, int *inst_index){

    if( (*PC >= simulated_memory->text_seg.addr) &&
        (*PC < simulated_memory->text_seg.addr + simulated_memory->text_seg.size) ){

            *inst_index = (*PC - simulated_memory->text_seg.addr)/4;

    } else{
        printf("ERROR: PC out of bounds.\n");
    }

    return;
}

// mapping instructions to make the name easier to understand
void memory_read_word(int addr, SimulatedMemory *simulated_memory, int *value){
    memory_read(4, addr, simulated_memory, value);
    return;
}

void memory_read_half(int addr, SimulatedMemory *simulated_memory, int *value){
    memory_read(2, addr, simulated_memory, value);
    return;
}

void memory_read_byte(int addr, SimulatedMemory *simulated_memory, int *value){
    memory_read(1, addr, simulated_memory, value);
    return;
}

void memory_write_word(int addr, SimulatedMemory *simulated_memory, int value){
    memory_write(4, addr, simulated_memory, value);
    return;
}

void memory_write_half(int addr, SimulatedMemory *simulated_memory, int value){
    memory_write(2, addr, simulated_memory, value);
    return;
}

void memory_write_byte(int addr, SimulatedMemory *simulated_memory, int value){
    memory_write(1, addr, simulated_memory, value);
    return;
}

//main memory functions
void memory_read(int num_bytes, int addr, SimulatedMemory *simulated_memory, int *value){

    int text_start_addr, text_end_addr;
    int rodata_start_addr, rodata_end_addr;
    int data_start_addr, data_end_addr;
    int sdata_start_addr, sdata_end_addr;
    int offset;
    int i;

    //clean variable
    *value = 0;

    text_start_addr = simulated_memory->text_seg.addr;
    text_end_addr = simulated_memory->text_seg.addr + simulated_memory->text_seg.size - 1;

    rodata_start_addr = simulated_memory->rodata_seg.addr;
    rodata_end_addr = simulated_memory->rodata_seg.addr + simulated_memory->rodata_seg.size - 1;

    data_start_addr = simulated_memory->data_seg.addr;
    data_end_addr = simulated_memory->data_seg.addr + simulated_memory->data_seg.size - 1;

    sdata_start_addr = simulated_memory->sdata_seg.addr;
    sdata_end_addr = simulated_memory->sdata_seg.addr + simulated_memory->sdata_seg.size - 1;

    //find which memory block should be accessed
    if(addr >= text_start_addr && addr <= text_end_addr){
        printf("ERROR: Trying to read .text segment\n");

    } else if(addr >= rodata_start_addr && addr <= rodata_end_addr){
        offset = addr - rodata_start_addr;
        for(i=0;i<num_bytes;i++){
            *value = *value | ( (unsigned)simulated_memory->rodata_seg.data[offset+i] << 8*i );
        }

    } else if(addr >= data_start_addr && addr <= data_end_addr){
        offset = addr - data_start_addr;
        for(i=0;i<num_bytes;i++){
            *value = *value | ( (unsigned)simulated_memory->data_seg.data[offset+i] << 8*i );
        }

    } else if(addr >= sdata_start_addr && addr <= sdata_end_addr){
        offset = addr - sdata_start_addr;
        for(i=0;i<num_bytes;i++){
            *value = *value | ( (unsigned)simulated_memory->sdata_seg.data[offset+i] << 8*i );
        }

    } else { //read from miscellaneous segment
        for(i=0;i<num_bytes;i++){
            *value = *value | ( (unsigned)simulated_memory->misc_seg[addr%MAX_DATA + i] << 8*i );
        }
    }

    return;
}

void memory_write(int num_bytes, int addr, SimulatedMemory *simulated_memory, int value){

    int text_start_addr, text_end_addr;
    int rodata_start_addr, rodata_end_addr;
    int data_start_addr, data_end_addr;
    int sdata_start_addr, sdata_end_addr;
    int offset;
    int i;

    text_start_addr = simulated_memory->text_seg.addr;
    text_end_addr = simulated_memory->text_seg.addr + simulated_memory->text_seg.size - 1;

    rodata_start_addr = simulated_memory->rodata_seg.addr;
    rodata_end_addr = simulated_memory->rodata_seg.addr + simulated_memory->rodata_seg.size - 1;

    data_start_addr = simulated_memory->data_seg.addr;
    data_end_addr = simulated_memory->data_seg.addr + simulated_memory->data_seg.size - 1;

    sdata_start_addr = simulated_memory->sdata_seg.addr;
    sdata_end_addr = simulated_memory->sdata_seg.addr + simulated_memory->sdata_seg.size - 1;

    //find which memory block should be accessed
    if(addr >= text_start_addr && addr <= text_end_addr){
        printf("ERROR: Trying to write to .text segment\n");

    } else if(addr >= rodata_start_addr && addr <= rodata_end_addr){
        offset = addr - rodata_start_addr;
        for(i=0;i<num_bytes;i++){
            simulated_memory->rodata_seg.data[offset+i] = value & 0xff;
            value = value >> 8;
        }

    } else if(addr >= data_start_addr && addr <= data_end_addr){
        offset = addr - data_start_addr;
        for(i=0;i<num_bytes;i++){
           simulated_memory->data_seg.data[offset+i] = value & 0xff;
            value = value >> 8; 
        }

    } else if(addr >= sdata_start_addr && addr <= sdata_end_addr){
        offset = addr - data_start_addr;
        for(i=0;i<num_bytes;i++){
           simulated_memory->sdata_seg.data[offset+i] = value & 0xff;
            value >> 8; 
        }

    } else { //read from miscellaneous segment
        for(i=0;i<num_bytes;i++){
            simulated_memory->misc_seg[addr%MAX_DATA + i] = value & 0xff;
            value = value >> 8;
        }
    }

    return;
}

/*
execute_inst return codes:
 0 = ready for next instruction
 1 = executed last instruction. Halt
-1 = ERROR
*/
int execute_inst(int inst_index, SimulatedMemory *simulated_memory, int reg_bank[], int *PC, int *cycles){

    int i;
    int shamt;
    int ecall_type;
    int ecall_fd;
    int ecall_buffer_addr;
    char ecall_buffer[50];
    int ecall_n;
    int aux_ecall_buffer;
    int aux_addr;
    int aux_value;
    long aux_mul;
    InstData inst_data;
    int inst_addr;

    inst_data = simulated_memory->text_seg.inst_list[inst_index];
    inst_addr = simulated_memory->text_seg.addr+inst_index*4;

    //printf("Executing instruction %08x: %08x\n", inst_addr, inst_data.raw_instruction);


    if(inst_data.opcode == LUI_OPCODE){ //1

        reg_bank[inst_data.rd] = inst_data.imm;
        *PC = *PC + 4;
        *cycles++;

    } else if(inst_data.opcode == AUIPC_OPCODE){ //2

        reg_bank[inst_data.rd] = *PC + inst_data.imm;
        *PC = *PC + 4;
        *cycles++;

    } else if(inst_data.opcode == JAL_OPCODE){ //3

        reg_bank[inst_data.rd] = *PC + 4;
        *PC = *PC + inst_data.imm; 
        *cycles++;

    } else if(  inst_data.opcode == JALR_OPCODE &&
                inst_data.funct3 == JALR_FUNCT3){ //4

        reg_bank[inst_data.rd] = *PC + 4;
        *PC = reg_bank[inst_data.rs1] + inst_data.imm & ~1; //mask LSB 
        *cycles++;

    } else if(  inst_data.opcode == BEQ_OPCODE &&
                inst_data.funct3 == BEQ_FUNCT3){ //5

        if (reg_bank[inst_data.rs1] == reg_bank[inst_data.rs2]){
            *PC = *PC + inst_data.imm; 
        }else{
            *PC = *PC + 4;
        }
        *cycles++;

    } else if(  inst_data.opcode == BNE_OPCODE &&
                inst_data.funct3 == BNE_FUNCT3){ //6

        if (reg_bank[inst_data.rs1] != reg_bank[inst_data.rs2]){
            *PC = *PC + inst_data.imm; 
        }else{
            *PC = *PC + 4;
        }
        *cycles++;

    } else if(  inst_data.opcode == BLT_OPCODE &&
                inst_data.funct3 == BLT_FUNCT3){ //7

        if (reg_bank[inst_data.rs1] < reg_bank[inst_data.rs2]){
            *PC = *PC + inst_data.imm; 
        }else{
            *PC = *PC + 4;
        }
        *cycles++;

    } else if(  inst_data.opcode == BGE_OPCODE &&
                inst_data.funct3 == BGE_FUNCT3){ //8

        if (reg_bank[inst_data.rs1] >= reg_bank[inst_data.rs2]){
            *PC = *PC + inst_data.imm; 
        }else{
            *PC = *PC + 4;
        }
        *cycles++;

    } else if(  inst_data.opcode == BLTU_OPCODE &&
                inst_data.funct3 == BLTU_FUNCT3){ //9

        if ( (unsigned)reg_bank[inst_data.rs1] < (unsigned)reg_bank[inst_data.rs2]){
            *PC = *PC + inst_data.imm; 
        }else{
            *PC = *PC + 4;
        }
        *cycles++;

    } else if(  inst_data.opcode == BGEU_OPCODE &&
                inst_data.funct3 == BGEU_FUNCT3){ //10

        if ( (unsigned)reg_bank[inst_data.rs1] >= (unsigned)reg_bank[inst_data.rs2]){
            *PC = *PC + inst_data.imm; 
        }else{
            *PC = *PC + 4;
        }
        *cycles++;

    } else if(  inst_data.opcode == LB_OPCODE &&
                inst_data.funct3 == LB_FUNCT3){ //11

        aux_addr = reg_bank[inst_data.rs1] + inst_data.imm; 
        memory_read_byte(aux_addr, simulated_memory, &aux_value);
        reg_bank[inst_data.rd] = sign_extend(aux_value,8);
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == LH_OPCODE &&
                inst_data.funct3 == LH_FUNCT3){ //12

        aux_addr = reg_bank[inst_data.rs1] + inst_data.imm; 
        memory_read_half(aux_addr, simulated_memory, &aux_value);
        reg_bank[inst_data.rd] = sign_extend(aux_value,16);
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == LW_OPCODE &&
                inst_data.funct3 == LW_FUNCT3){ //13

        aux_addr = reg_bank[inst_data.rs1] + inst_data.imm; 
        memory_read_word(aux_addr, simulated_memory, &aux_value);
        reg_bank[inst_data.rd] = aux_value;
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == LBU_OPCODE &&
                inst_data.funct3 == LBU_FUNCT3){ //14

        aux_addr = reg_bank[inst_data.rs1] + inst_data.imm; 
        memory_read_byte(aux_addr, simulated_memory, &aux_value);
        reg_bank[inst_data.rd] = aux_value;
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == LHU_OPCODE &&
                inst_data.funct3 == LHU_FUNCT3){ //15

        aux_addr = reg_bank[inst_data.rs1] + inst_data.imm; 
        memory_read_half(aux_addr, simulated_memory, &aux_value);
        reg_bank[inst_data.rd] = aux_value;
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == SB_OPCODE &&
                inst_data.funct3 == SB_FUNCT3){ //16

        aux_addr = reg_bank[inst_data.rs1] + inst_data.imm;
        aux_value = reg_bank[inst_data.rs2];
        memory_write_byte(aux_addr, simulated_memory, aux_value);
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == SH_OPCODE &&
                inst_data.funct3 == SH_FUNCT3){ //17

        aux_addr = reg_bank[inst_data.rs1] + inst_data.imm;
        aux_value = reg_bank[inst_data.rs2]; 
        memory_write_half(aux_addr, simulated_memory, aux_value);
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == SW_OPCODE &&
                inst_data.funct3 == SW_FUNCT3){ //18

        aux_addr = reg_bank[inst_data.rs1] + inst_data.imm;
        aux_value = reg_bank[inst_data.rs2];
        memory_write_word(aux_addr, simulated_memory, aux_value);
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == ADDI_OPCODE &&
                inst_data.funct3 == ADDI_FUNCT3){ //19
        
        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] + inst_data.imm;  
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == SLTI_OPCODE &&
                inst_data.funct3 == SLTI_FUNCT3){ //20

        if(reg_bank[inst_data.rs1] < inst_data.imm){ 
            reg_bank[inst_data.rd] = 1;
        }else{
            reg_bank[inst_data.rd] = 0;
        }
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == SLTIU_OPCODE &&
                inst_data.funct3 == SLTIU_FUNCT3){ //21

        if((unsigned)reg_bank[inst_data.rs1] < (unsigned)inst_data.imm){ 
            reg_bank[inst_data.rd] = 1;
        }else{
            reg_bank[inst_data.rd] = 0;
        }
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == XORI_OPCODE &&
                inst_data.funct3 == XORI_FUNCT3){ //22

        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] ^ inst_data.imm;  
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == ORI_OPCODE &&
                inst_data.funct3 == ORI_FUNCT3){ //23

        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] | inst_data.imm;  
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == ANDI_OPCODE &&
                inst_data.funct3 == ANDI_FUNCT3){ //24

        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] & inst_data.imm;  
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == SLLI_OPCODE &&
                inst_data.funct3 == SLLI_FUNCT3){ //25

        shamt = inst_data.imm & 0b111111;
        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] << shamt;
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == SRLI_OPCODE &&
                inst_data.funct3 == SRLI_FUNCT3 &&
                inst_data.funct7 == SRLI_PSEUDO_FUNCT7){ //26

        shamt = inst_data.imm & 0b111111;
        reg_bank[inst_data.rd] = (unsigned)reg_bank[inst_data.rs1] >> shamt;
        *PC = *PC + 4;
        *cycles++;


    } else if(  inst_data.opcode == SRAI_OPCODE &&
                inst_data.funct3 == SRAI_FUNCT3 &&
                inst_data.funct7 == SRAI_PSEUDO_FUNCT7){ //27

        shamt = inst_data.imm & 0b111111;
        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] >> shamt;
        *PC = *PC + 4;
        *cycles++;


    } else if(  inst_data.opcode == ADD_OPCODE &&
                inst_data.funct3 == ADD_FUNCT3 &&
                inst_data.funct7 == ADD_FUNCT7){ //28

        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] + reg_bank[inst_data.rs2];
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == SUB_OPCODE &&
                inst_data.funct3 == SUB_FUNCT3 &&
                inst_data.funct7 == SUB_FUNCT7){//29

        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] - reg_bank[inst_data.rs2];
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == SLL_OPCODE &&
                inst_data.funct3 == SLL_FUNCT3 &&
                inst_data.funct7 == SLL_FUNCT7){//30

        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] << (reg_bank[inst_data.rs2] & 0b11111);
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == SLT_OPCODE &&
                inst_data.funct3 == SLT_FUNCT3 &&
                inst_data.funct7 == SLT_FUNCT7){//31

        if(reg_bank[inst_data.rs1] < reg_bank[inst_data.rs2]){
            reg_bank[inst_data.rd] = 1;
        }else{
            reg_bank[inst_data.rd] = 0;
        }
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == SLTU_OPCODE &&
                inst_data.funct3 == SLTU_FUNCT3 &&
                inst_data.funct7 == SLTU_FUNCT7){//32{

        if((unsigned)reg_bank[inst_data.rs1] < (unsigned)reg_bank[inst_data.rs2]){
            reg_bank[inst_data.rd] = 1;
        }else{
            reg_bank[inst_data.rd] = 0;
        }
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == XOR_OPCODE &&
                inst_data.funct3 == XOR_FUNCT3 &&
                inst_data.funct7 == XOR_FUNCT7){//33

        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] ^ reg_bank[inst_data.rs2];
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == SRL_OPCODE &&
                inst_data.funct3 == SRL_FUNCT3 &&
                inst_data.funct7 == SRL_FUNCT7){//34

        reg_bank[inst_data.rd] = (unsigned)reg_bank[inst_data.rs1] >> (reg_bank[inst_data.rs2] & 0b11111);
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == SRA_OPCODE &&
                inst_data.funct3 == SRA_FUNCT3 &&
                inst_data.funct7 == SRA_FUNCT7){//35

        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] >> (reg_bank[inst_data.rs2] & 0b11111);
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == OR_OPCODE &&
                inst_data.funct3 == OR_FUNCT3 &&
                inst_data.funct7 == OR_FUNCT7){ //36

        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] | reg_bank[inst_data.rs2];
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == AND_OPCODE &&//37
                inst_data.funct3 == AND_FUNCT3 &&
                inst_data.funct7 == AND_FUNCT7){ 

        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] & reg_bank[inst_data.rs2];
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == ECALL_OPCODE &&
                inst_data.imm == 0b000000000000){//38 

        //check type of call
        ecall_type = reg_bank[17]; //a7
        if(ecall_type == ECALL_WRITE_CODE){
            ecall_fd = reg_bank[10];
            ecall_buffer_addr = reg_bank[11];
            ecall_n = reg_bank[12];

            for (i = 0; i < ecall_n; i++){
                memory_read_byte(ecall_buffer_addr+i, simulated_memory, &aux_ecall_buffer);
                ecall_buffer[i] = (char)aux_ecall_buffer;
            }
            
            fwrite(ecall_buffer, sizeof(char), ecall_n, (FILE*)(unsigned long)ecall_fd);

        } else if(ecall_type == ECALL_READ_CODE){
            ecall_fd = reg_bank[10];
            ecall_buffer_addr = reg_bank[11];
            ecall_n = reg_bank[12];

            fread(ecall_buffer, sizeof(char), ecall_n, (FILE*)(unsigned long)ecall_fd);

            for (i = 0; i < ecall_n; i++){
                memory_write_byte(ecall_buffer_addr+i, simulated_memory, (int)ecall_buffer[i]);
            }
            
        } else if(ecall_type == ECALL_EXIT_CODE){
              return 1;
        }
        
        
    } else if(  inst_data.opcode == EBREAK_OPCODE &&
                inst_data.imm == 0b000000000001){//39 

        *PC = *PC + 4;
        *cycles++;
        //end program
        return 1;

    } else if(  inst_data.opcode == MUL_OPCODE &&
                inst_data.funct3 == MUL_FUNCT3 &&
                inst_data.funct7 == MUL_FUNCT7){ //40

        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] * reg_bank[inst_data.rs2];
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == MULH_OPCODE &&
                inst_data.funct3 == MULH_FUNCT3 &&
                inst_data.funct7 == MULH_FUNCT7){ //41

        aux_mul = (long)reg_bank[inst_data.rs1] * (long)reg_bank[inst_data.rs2];
        reg_bank[inst_data.rd] = aux_mul >> 32;
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == MULHSU_OPCODE &&
                inst_data.funct3 == MULHSU_FUNCT3 &&
                inst_data.funct7 == MULHSU_FUNCT7){//42 

        aux_mul = (long)reg_bank[inst_data.rs1] * (unsigned long)reg_bank[inst_data.rs2];
        reg_bank[inst_data.rd] = aux_mul >> 32;
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == MULHU_OPCODE &&
                inst_data.funct3 == MULHU_FUNCT3 &&
                inst_data.funct7 == MULHU_FUNCT7){//43

        aux_mul = (unsigned long)reg_bank[inst_data.rs1] * (unsigned long)reg_bank[inst_data.rs2];
        reg_bank[inst_data.rd] = aux_mul >> 32;
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == DIV_OPCODE &&
                inst_data.funct3 == DIV_FUNCT3 &&
                inst_data.funct7 == DIV_FUNCT7){ //44

        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] / reg_bank[inst_data.rs2];
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == DIVU_OPCODE &&
                inst_data.funct3 == DIVU_FUNCT3 &&
                inst_data.funct7 == DIVU_FUNCT7){//45 

        reg_bank[inst_data.rd] = (unsigned)reg_bank[inst_data.rs1] / (unsigned)reg_bank[inst_data.rs2];
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == REM_OPCODE &&
                inst_data.funct3 == REM_FUNCT3 &&
                inst_data.funct7 == REM_FUNCT7){ //46

        reg_bank[inst_data.rd] = reg_bank[inst_data.rs1] % reg_bank[inst_data.rs2];
        *PC = *PC + 4;
        *cycles++;

    } else if(  inst_data.opcode == REMU_OPCODE &&
                inst_data.funct3 == REMU_FUNCT3 &&
                inst_data.funct7 == REMU_FUNCT7){ //47

        reg_bank[inst_data.rd] = (unsigned)reg_bank[inst_data.rs1] % (unsigned)reg_bank[inst_data.rs2];
        *PC = *PC + 4;
        *cycles++;

    } else{
        printf("ERROR: executing invalid instruction.\n");
        return -1;
    }

    //need to refresh zero every instruction
    reg_bank[0] = 0;

    return 0;
}