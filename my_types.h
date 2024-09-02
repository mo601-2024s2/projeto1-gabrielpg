#ifndef _MY_TYPES_H_
#define _MY_TYPES_H_


#define MAX_INSTRUCTIONS 16384
#define MAX_DATA 65536
#define MAX_LOG 65536

// Elf file types

typedef struct SectionHeader{
    int addr;
    int offset;
    int size;
} SectionHeader;

//instructions types

typedef enum InstType { R, I, S, B, U, J} InstType;

typedef struct InstData {
    int raw_instruction;
    int opcode,
        rd,
        rs1,
        rs2,
        imm,
        funct3,
        funct7;
    InstType type;
} InstData;


// memory segments types

typedef struct TextSegment{
    int entry;
    int addr;
    int size;
    InstData inst_list[MAX_INSTRUCTIONS];
} TextSegment;

typedef struct DataSegment{
    int addr;
    int size;
    unsigned char data[MAX_DATA];
} DataSegment;

typedef struct SimulatedMemory{
    TextSegment text_seg;
    DataSegment rodata_seg;
    DataSegment data_seg;
    DataSegment sdata_seg;
    unsigned char misc_seg[MAX_DATA]; //used for bss section and other things
} SimulatedMemory;

typedef struct ExecLogEntry{
    InstData inst_data;
    int PC,
    rd_index,
    rs1_index,
    rs2_index,
    rd_value,
    rs1_value,
    rs2_value;
} ExecLogEntry;


//auxiliary functions

void initialize_sec_header(SectionHeader *section);
void initialize_text_segment(TextSegment *text_segment);
void initialize_data_segment(DataSegment *data_segment);
void initialize_simulated_memory(SimulatedMemory *simulated_memory);

#endif