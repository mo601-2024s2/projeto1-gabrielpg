#include <stdio.h>
#include "elf_parser.h"
#include "elf_loader.h"
#include "log_generator.h"
#include "simulator.h"

int main(int argc, char *argv[]){

    // Check if a file name is provided as an argument
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_filename> <output_filename>\n", argv[0]);
        return 1;
    }

    //elf parser variables
    char *elf_file_path = argv[1];
    char *log_file_path = argv[2];

    int entry;
    SectionHeader text_section;
    SectionHeader rodata_section;
    SectionHeader data_section;
    SectionHeader sdata_section;
    //execution structures
    int i,j;
    int PC, cycles;
    int exec_log_size;
    int exec_code;
    SimulatedMemory simulated_memory;
    int reg_bank[32];
    ExecLogEntry exec_log[MAX_LOG];
    //debug
    int mem_data;
    int base_addr = (0x00500000 - 28)%MAX_DATA;

    //initialize data structures
    initialize_sec_header(&text_section);
    initialize_sec_header(&rodata_section);
    initialize_sec_header(&data_section);
    initialize_sec_header(&sdata_section);
    initialize_simulated_memory(&simulated_memory);
    reg_bank[0] = 0;

    //extract contents of elf file
    parse_elf_file(elf_file_path, &entry, &text_section, &rodata_section, &data_section, &sdata_section);

    load_elf_file(elf_file_path, entry, &text_section, &rodata_section, &data_section, &sdata_section,
                    &simulated_memory.text_seg, &simulated_memory.rodata_seg, &simulated_memory.data_seg, &simulated_memory.sdata_seg);

    // generate_dissassembly_log(simulated_memory.text_seg.inst_list, log_file_path, simulated_memory.text_seg.size);

    //start simulation
    exec_code = 0;
    PC = entry;
    i=0;
    while(exec_code == 0){
        exec_code = simulate_inst(&PC, &cycles, &simulated_memory, reg_bank, &exec_log[i++]);
    }
    exec_log_size = i;

    generate_log(exec_log, log_file_path, exec_log_size);


    return 0;
}

//debug lines
/*      
        //byte
        printf("Value in memory position %08x = %02x\n", base_addr, simulated_memory.misc_seg[base_addr]);
        //half word
        printf("Value in memory position %08x = %02x %02x\n", base_addr, simulated_memory.misc_seg[base_addr], simulated_memory.misc_seg[base_addr + 1]);
        //word
        printf("Value in memory position %08x = %02x %02x %02x %02x\n", base_addr, simulated_memory.misc_seg[base_addr], simulated_memory.misc_seg[base_addr + 1], simulated_memory.misc_seg[base_addr + 2], simulated_memory.misc_seg[base_addr + 3]);
        //double word
        printf("Value in memory position %08x = %02x %02x %02x %02x %02x %02x %02x %02x\n", base_addr+4, simulated_memory.misc_seg[base_addr+4], simulated_memory.misc_seg[base_addr+4 + 1], simulated_memory.misc_seg[base_addr+4 + 2], simulated_memory.misc_seg[base_addr+4 + 3], simulated_memory.misc_seg[base_addr+4 + 4], simulated_memory.misc_seg[base_addr+4 + 5], simulated_memory.misc_seg[base_addr+4 + 6], simulated_memory.misc_seg[base_addr+4 + 7]);
*/