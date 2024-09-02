#ifndef _LOG_GENERATOR_H_
#define _LOG_GENERATOR_H_

#include <stdio.h>
#include <string.h>
#include "my_types.h"
#include "elf_loader.h"

void print_dissassembly(InstData inst_data, char* inst_string);

void generate_dissassembly_log( InstData *inst_list, char* file_path, int text_size);

void print_exec_info(ExecLogEntry *execlog_entry, char *exec_info);

void print_log_line(ExecLogEntry *execlog_entry, char* log_line);

void generate_log(ExecLogEntry *execlog_entry, char* file_path, int log_size);

#endif