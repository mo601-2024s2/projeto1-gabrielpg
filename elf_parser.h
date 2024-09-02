#ifndef _ELF_PARSER_H_
#define _ELF_PARSER_H_

#include<stdio.h>
#include <string.h>
#include "my_types.h"

// offsets inside the elf file
#define E_ENTRY     0X18
#define E_PHOFF     0X1C
#define E_PHENTSIZE 0X2A
#define E_PHNUM     0X2C
#define E_SHOFF     0X20
#define E_SHENTSIZE 0X2E
#define E_SHNUM     0X30
#define E_SHSTRNDX  0X32
#define P_TYPE      0X00
#define P_OFFSET    0x04
#define P_VADDR     0x08
#define P_FILESZ    0x10
#define P_MEMSZ     0x14
#define SH_NAME     0x00
#define SH_ADDR     0x0C
#define SH_OFFSET   0x10
#define SH_SIZE     0x14


void parse_elf_file(char *file_path, int *entry, SectionHeader *text_section, SectionHeader *rodata_section, SectionHeader *data_section, SectionHeader *sdata_section);
void parse_elf_header(  FILE *fptr, int *phoff, int *phentsize, int *phentries, int *shoff, int *shentsize, int *shentries, int *shstrndx,
                        int *entry);
void parse_prog_headers(FILE *fptr, int phoff, int phentsize, int phentries);
void parse_sec_headers( FILE *fptr, int shoff, int shentsize, int shentries, int shstrndx, //inputs
                        SectionHeader *text_section, SectionHeader *rodata_section, SectionHeader *data_section, SectionHeader *sdata_section); //outputs

#endif