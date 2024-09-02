#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <stdio.h>
#include "my_types.h"
#include "elf_loader.h"


#define ECALL_READ_CODE 63
#define ECALL_WRITE_CODE 64
#define ECALL_EXIT_CODE 93

int simulate_inst(int *PC, int *cycles, SimulatedMemory *simulated_memory, int reg_bank[], ExecLogEntry *exec_log_entry);
void fetch_inst(int *PC, SimulatedMemory *simulated_memory, int *inst_index);
int execute_inst(int inst_index, SimulatedMemory *simulated_memory, int reg_bank[], int *next_PC, int *cycles);
void memory_read(int num_bytes, int addr, SimulatedMemory *simulated_memory, int *value);
void memory_read_word(int addr, SimulatedMemory *simulated_memory, int *value);
void memory_read_half(int addr, SimulatedMemory *simulated_memory, int *value);
void memory_read_byte(int addr, SimulatedMemory *simulated_memory, int *value);
void memory_write_word(int addr, SimulatedMemory *simulated_memory, int value);
void memory_write_half(int addr, SimulatedMemory *simulated_memory, int value);
void memory_write_byte(int addr, SimulatedMemory *simulated_memory, int value);
void memory_write(int num_bytes, int addr, SimulatedMemory *simulated_memory, int value);

#endif