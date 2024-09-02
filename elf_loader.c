#include "elf_loader.h"


int sign_extend(int imm, int num_bits){
    int extended_imm;

    extended_imm = imm << 32-num_bits;
    extended_imm = extended_imm >> 32-num_bits;

    return extended_imm;
}

//Extracts some bits from the 32bit instruction and return
int extract_field(int input, int start_bit, int end_bit){
    int i;
    
    int field;
    int mask = 0;
    int num_bits;

    // Create mask
    num_bits = end_bit - start_bit + 1;
    for(i=0; i<num_bits; i++){
        mask = (mask << 1) | 0x1;
    }
    mask = mask << start_bit;

    field = input & mask;
    field = field >> start_bit;

    return field;
}

// reads the elf file and stores the .text and .rodata segments in data structures
void load_elf_file(char *file_path, int entry, SectionHeader *text_section, SectionHeader *rodata_section, SectionHeader *data_section, SectionHeader *sdata_section,
                    TextSegment *text_segment, DataSegment *rodata_segment, DataSegment *data_segment, DataSegment *sdata_segment){
    
    int i;
    int word;
    FILE *fptr;
    int raw_instructions[MAX_INSTRUCTIONS];

    printf("\n\nSTARTING TO LOAD THE ELF FILE...\n\n");
    fptr = fopen(file_path, "rb");

    if (fptr == 0){
        printf("Failed to open file\n");
    }

    // printf("\n\nREADING THE .TEXT SECTION...\n\n");

    text_segment->addr = text_section->addr;
    text_segment->entry = entry;
    text_segment->size = text_section->size;

    // Extract the bytes
    fseek(fptr, (text_section->offset) , SEEK_SET);
    for(i=0;i< (text_section->size)/4 ;i++){
        fread(&word, sizeof(word), 1, fptr);
        raw_instructions[i] = word;
        // printf("Instruction %02d = %08x\n", i, raw_instructions[i]);
        text_segment->inst_list[i] = decode_instruction(raw_instructions[i]);
    }
    
    // printf("\n\nREADING THE .RODATA SECTION...\n\n");

    rodata_segment->addr = rodata_section->addr;
    rodata_segment->size = rodata_section->size;

    //extract the bytes
    fseek(fptr, rodata_section->offset, SEEK_SET);
    fread(rodata_segment->data, sizeof(char), rodata_segment->size, fptr);
    // printf(".rodata content: \n");
    // for(i=0;i<rodata_segment->size;i++){
    //     printf("%02x\n",rodata_segment->data[i]);
    // }

    // printf("\n\nREADING THE .DATA SECTION...\n\n");

    data_segment->addr = data_section->addr;
    data_segment->size = data_section->size;

    //extract the bytes
    fseek(fptr, data_section->offset, SEEK_SET);
    fread(data_segment->data, sizeof(char), data_segment->size, fptr);
    // printf(".data content: \n");
    // for(i=0;i<data_segment->size;i++){
    //     printf("%02x\n",data_segment->data[i]);
    // }

    // printf("\n\nREADING THE .SDATA SECTION...\n\n");

    sdata_segment->addr = sdata_section->addr;
    sdata_segment->size = sdata_section->size;

    //extract the bytes
    fseek(fptr, sdata_section->offset, SEEK_SET);
    fread(sdata_segment->data, sizeof(char), sdata_segment->size, fptr);
    // printf(".sdata content: \n");
    // for(i=0;i<sdata_segment->size;i++){
    //     printf("%02x\n",sdata_segment->data[i]);
    // }

    printf("\n\nFINISHED TO LOAD THE ELF FILE...\n\n");

    return;

}

// extracts the fields of the instruction and do a non-exhaustive check for validity
InstData decode_instruction(int raw_instruction){
    InstData inst_data;
    int aux_imm;
    int invalid=0;

    inst_data.raw_instruction = raw_instruction;
    inst_data.opcode = extract_field(raw_instruction, 0, 6);

    // decode the instruction depending on the type
    if (inst_data.opcode == I_TYPE1 || 
        inst_data.opcode == I_TYPE2 || 
        inst_data.opcode == I_TYPE3){

        inst_data.type = I;
        inst_data.rd = extract_field(raw_instruction, 7, 11);
        inst_data.rs1 = extract_field(raw_instruction, 15, 19);
        inst_data.rs2 = 0;

        aux_imm = extract_field(raw_instruction, 20, 31);
        aux_imm = sign_extend(aux_imm, 12);
        inst_data.imm = aux_imm;

        inst_data.funct3 = extract_field(raw_instruction, 12, 14);
        inst_data.funct7 = extract_field(raw_instruction, 26, 31); //pseudo funct7

        if (inst_data.funct3 != 0b000 && //jalr, lb, addi
            inst_data.funct3 != 0b001 && //lh, slli
            inst_data.funct3 != 0b010 && //lw, slti
            inst_data.funct3 != 0b011 && //sltiu
            inst_data.funct3 != 0b100 && //lbu, xori
            inst_data.funct3 != 0b101 && //lhu, srli, srai, 
            inst_data.funct3 != 0b110 && //ori
            inst_data.funct3 != 0b111){//andi

            invalid = 1;
        }
        

    } else if (inst_data.opcode == S_TYPE){

        inst_data.type = S;
        inst_data.rd = 0;
        inst_data.rs1 = extract_field(raw_instruction, 15, 19);
        inst_data.rs2 = extract_field(raw_instruction, 20, 24);

        aux_imm = extract_field(raw_instruction, 25, 31);
        aux_imm = (aux_imm << 5) | extract_field(raw_instruction, 7, 11);
        aux_imm = sign_extend(aux_imm, 12);
        inst_data.imm = aux_imm; 

        inst_data.funct3 = extract_field(raw_instruction, 12, 14);
        inst_data.funct7 = 0;

        if (inst_data.funct3 != 0b000 && // sb
            inst_data.funct3 != 0b001 && // sh
            inst_data.funct3 != 0b010){ // sw
            
            invalid = 1;
        }

    } else if(inst_data.opcode == B_TYPE){

        inst_data.type = B;
        inst_data.rd = 0;
        inst_data.rs1 = extract_field(raw_instruction, 15, 19);
        inst_data.rs2 = extract_field(raw_instruction, 20, 24);

        aux_imm = extract_field(raw_instruction, 31, 31) << 12;
        aux_imm = aux_imm | (extract_field(raw_instruction, 7, 7) << 11);
        aux_imm = aux_imm | (extract_field(raw_instruction, 25, 30) << 5);
        aux_imm = aux_imm | (extract_field(raw_instruction, 8, 11) << 1);
        aux_imm = sign_extend(aux_imm, 13);
        inst_data.imm = aux_imm;

        inst_data.funct3 = extract_field(raw_instruction, 12, 14);
        inst_data.funct7 = 0;

        if (inst_data.funct3 != 0b000 && // beq
            inst_data.funct3 != 0b001 && // bne
            inst_data.funct3 != 0b100 && // blt
            inst_data.funct3 != 0b101 && // bge
            inst_data.funct3 != 0b110 && // bltu
            inst_data.funct3 != 0b111){// bgeu

            invalid = 1;
        }

    } else if(  inst_data.opcode == U_TYPE1 ||
                inst_data.opcode == U_TYPE2){
        
        inst_data.type = U;
        inst_data.rd = extract_field(raw_instruction, 7, 11);
        inst_data.rs1 = 0;
        inst_data.rs2 = 0;
        inst_data.imm = extract_field(raw_instruction, 12, 31) << 12;
        inst_data.funct3 = 0;
        inst_data.funct7 = 0;

        // only opcode can be checked here

    } else if(inst_data.opcode == J_TYPE){

        inst_data.type = J;
        inst_data.rd = extract_field(raw_instruction, 7, 11);
        inst_data.rs1 = 0;
        inst_data.rs2 = 0;
        
        aux_imm = extract_field(raw_instruction, 31, 31) << 20;
        aux_imm = aux_imm | (extract_field(raw_instruction, 12, 19) << 12);
        aux_imm = aux_imm | (extract_field(raw_instruction, 20, 20) << 11);
        aux_imm = aux_imm | (extract_field(raw_instruction, 21, 30) << 1);
        aux_imm = sign_extend(aux_imm, 21);

        inst_data.imm = aux_imm;

        inst_data.funct3 = 0;
        inst_data.funct7 = 0;

        // only opcode can be checked here

    } else if(inst_data.opcode == R_TYPE){

        inst_data.type = R;
        inst_data.rd = extract_field(raw_instruction, 7, 11);
        inst_data.rs1 = extract_field(raw_instruction, 15, 19);
        inst_data.rs2 = extract_field(raw_instruction, 20, 24);
        inst_data.imm = 0;
        inst_data.funct3 = extract_field(raw_instruction, 12, 14);
        inst_data.funct7 = extract_field(raw_instruction, 25, 31);

        //check funct3 first
        if (inst_data.funct3 != 0b000 && // add, sub, mul
            inst_data.funct3 != 0b001 && // sll, mulh
            inst_data.funct3 != 0b010 && // slt, mulhsu
            inst_data.funct3 != 0b011 && // sltu, mulhu
            inst_data.funct3 != 0b100 && // xor, div
            inst_data.funct3 != 0b101 && // srl, sra, divu  
            inst_data.funct3 != 0b110 && // or, rem
            inst_data.funct3 != 0b111){// and, remu

            invalid = 1;
        //check funct7
        } else if(  inst_data.funct7 != 0b0000000 && //add,sub,etc
                    inst_data.funct7 != 0b0100000 && //sra and srai
                    inst_data.funct7 != 0b0000001){ //mul and others

            invalid = 1;
        }

    } else if(inst_data.opcode == ECALL_EBREAK_TYPE){

        inst_data.type = 0;
        inst_data.rd = 0;
        inst_data.rs1 = 0;
        inst_data.rs2 = 0;
        inst_data.imm = extract_field(raw_instruction, 20, 31);
        inst_data.funct3 = 0;
        inst_data.funct7 = 0;

        
        if(inst_data.imm == 0b000000000000){ //ecall
            //all bits other than opcode must be 0
            if( (raw_instruction >> 7) != 0){
                invalid = 1;
            }
        } else if(inst_data.imm == 0b000000000001){ //ebreak 
            //rest of the instruction must be 0b0 0000 0000 0010 0000 0000 0000
            if( (raw_instruction >> 7) != 0b0000000000010000000000000){
                invalid = 1;
            }
        } else{
            invalid = 1;
        }
    } else{
        invalid = 1;
    }

    // printf("inst_data.opcode = %x;\n inst_data.type = %x;\n inst_data.rd = %x;\n inst_data.rs1 = %x;\n inst_data.rs2 = %x;\n inst_data.imm = %x;\n inst_data.funct3 = %x;\n inst_data.funct7 = %x;\n",
    //     inst_data.opcode, inst_data.type, inst_data.rd, inst_data.rs1, inst_data.rs2, inst_data.imm, inst_data.funct3, inst_data.funct7);

    if (invalid == 1){
        printf("ERROR: instruction %x is invalid.\n\n", raw_instruction);
    }
    
    return inst_data;
}