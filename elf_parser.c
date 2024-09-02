#include "elf_parser.h"

// Reads the elf file given by file_path and returns some information about it concerning the .text and .rodata sections
void parse_elf_file(char *file_path, int *entry, SectionHeader *text_section, SectionHeader *rodata_section, SectionHeader *data_section, SectionHeader *sdata_section){    
    
    //general variables
    FILE *fptr;

    //ELF header variables
    int phentsize;
    int phoff;
    int phentries;
    int phentry_offset;
    int shoff;
    int shentsize;
    int shentries;
    int shstrndx;
    
    printf("\n\nSTARTING TO PARSE THE ELF FILE %s...\n\n", file_path);
    fptr = fopen(file_path, "rb");

    if (fptr == NULL){
        printf("Failed to open file\n");
    }

    parse_elf_header(fptr, &phoff, &phentsize, &phentries, &shoff, &shentsize, &shentries, &shstrndx,
                    entry);

    // parse_prog_headers(fptr, phoff, phentsize, phentries); //optional

    parse_sec_headers(  fptr, shoff, shentsize, shentries, shstrndx,
                        text_section, rodata_section, data_section, sdata_section);


    printf("\n\nFINISHED PARSING THE ELF FILE...\n\n");
    printf("\nentry = %x, text_off = %x, text_size = %x, text_addr = %x\n", *entry, text_section->offset, text_section->size, text_section->addr);
    printf("rodata_off = %x, rodata_size = %x, rodata_addr = %x\n", rodata_section->offset, rodata_section->size, rodata_section->addr);
    printf("data_off = %x, data_size = %x, data_addr = %x\n", data_section->offset, data_section->size, data_section->addr);
    printf("sdata_off = %x, sdata_size = %x, sdata_addr = %x\n", sdata_section->offset, sdata_section->size, sdata_section->addr);
    
    fclose(fptr);

    return;
}

// Reads the elf header and returns information about the program headers and the section headers
void parse_elf_header(  FILE *fptr, int *phoff, int *phentsize, int *phentries, int *shoff, int *shentsize, int *shentries, int *shstrndx,
                        int *entry){
    
    char byte;
    short half_word;
    unsigned word;

    printf("\n\nPARSING THE ELF HEADER...\n\n");

    //Program 

    fseek(fptr, E_ENTRY, SEEK_SET);
    fread(&word, sizeof(word), 1, fptr);
    *entry = word;
    // printf("Entry point = %x\n", word);

    fseek(fptr, E_PHOFF, SEEK_SET);
    fread(&word, sizeof(word), 1, fptr);
    // printf("Program Header Offset = %x\n", word);
    *phoff = word;

    fseek(fptr, E_PHENTSIZE, SEEK_SET);
    fread(&half_word, sizeof(half_word), 1, fptr);
    // printf("Size of a PH Entry = %x\n", half_word);
    *phentsize = (int)half_word;

    fseek(fptr, E_PHNUM, SEEK_SET);
    fread(&half_word, sizeof(half_word), 1, fptr);
    // printf("Program Header Entries = %x\n", half_word);
    *phentries = (int)half_word;

    //Section

    fseek(fptr, E_SHOFF, SEEK_SET);
    fread(&word, sizeof(word), 1, fptr);
    // printf("Section Header Offset = %x\n", word);
    *shoff = word;

    fseek(fptr, E_SHENTSIZE, SEEK_SET);
    fread(&half_word, sizeof(half_word), 1, fptr);
    // printf("Size of a SH Entry = %x\n", half_word);
    *shentsize = (int)half_word;

    fseek(fptr, E_SHNUM, SEEK_SET);
    fread(&half_word, sizeof(half_word), 1, fptr);
    // printf("Section Header Entries = %x\n", half_word);
    *shentries = (int)half_word;

    fseek(fptr, E_SHSTRNDX, SEEK_SET);
    fread(&half_word, sizeof(half_word), 1, fptr);
    // printf("Section names offset = %x\n", half_word);
    *shstrndx = (int)half_word;

    return;
}

// shows some information about the program header
void parse_prog_headers(FILE *fptr, int phoff, int phentsize, int phentries){
    int i;

    char byte;
    short half_word;
    unsigned word;
    int phentry_offset;

    printf("\n\nPARSING THE PROGRAM HEADERS...\n\n");

    for (i = 0; i < phentries; i++){

        // printf("\nSegment %d:\n", i);
        phentry_offset = phoff + i*(phentsize);

        fseek(fptr, phentry_offset + P_TYPE, SEEK_SET);
        fread(&word, sizeof(word), 1, fptr);
        // printf("Type of segment %d = %x\n", i, word);
        
        fseek(fptr, phentry_offset + P_OFFSET, SEEK_SET);
        fread(&word, sizeof(word), 1, fptr);
        // printf("Offset of segment %d = %x\n", i, word);

        fseek(fptr, phentry_offset + P_VADDR, SEEK_SET);
        fread(&word, sizeof(word), 1, fptr);
        // printf("VMEM addr of segment %d = %x\n", i, word);

        fseek(fptr, phentry_offset + P_FILESZ, SEEK_SET);
        fread(&word, sizeof(word), 1, fptr);
        // printf("File size of segment %d = %x\n", i, word);

        fseek(fptr, phentry_offset + P_MEMSZ, SEEK_SET);
        fread(&word, sizeof(word), 1, fptr);
        // printf("Mem size of segment %d = %x\n", i, word);   
    }
    return;
}

// Reads the section headers and returns information about the .text and .rodata sctions
void parse_sec_headers( FILE *fptr, int shoff, int shentsize, int shentries, int shstrndx, //inputs
                        SectionHeader *text_section, SectionHeader *rodata_section, SectionHeader *data_section, SectionHeader *sdata_section){ //outputs
    int i;

    char byte;
    short half_word;
    unsigned word;
    int shentry_offset;
    int strtab_offset;
    int sh_name;
    char sec_name[20];
    int sec_addr;
    int sec_off;
    int sec_size;
    
    printf("\n\nPARSING THE SECTION HEADERS...\n\n");

    // find the strtab
    shentry_offset = shoff + shstrndx*(shentsize);
    fseek(fptr, shentry_offset + SH_OFFSET, SEEK_SET);
    fread(&word, sizeof(word), 1, fptr);
    // printf("\nOffset of strtab = %x\n", word);
    strtab_offset = word;


    for (i = 0; i < shentries; i++){

        // printf("\nSection %d:\n", i);
        shentry_offset = shoff + i*(shentsize);

        fseek(fptr, shentry_offset + SH_NAME, SEEK_SET);
        fread(&word, sizeof(word), 1, fptr);
        // printf("Index to name of section %d = %x\n", i, word);
        sh_name = word;

        fseek(fptr, strtab_offset + sh_name, SEEK_SET);
        fread(sec_name, sizeof(char), 20, fptr);
        // printf("Name of section %d = %s\n", i, sec_name);

        fseek(fptr, shentry_offset + SH_ADDR, SEEK_SET);
        fread(&word, sizeof(word), 1, fptr);
        sec_addr = word;
        // printf("Addr of section %d = %x\n", i, word);

        fseek(fptr, shentry_offset + SH_OFFSET, SEEK_SET);
        fread(&word, sizeof(word), 1, fptr);
        sec_off = word;
        // printf("Offset of section %d = %x\n", i, word);

        fseek(fptr, shentry_offset + SH_SIZE, SEEK_SET);
        fread(&word, sizeof(word), 1, fptr);
        sec_size = word;
        // printf("Size of section %d = %x\n", i, word);

        //decide if I need to extract the data
        if(strcmp(sec_name, ".text") == 0){

            text_section->offset = sec_off;
            text_section->addr = sec_addr;
            text_section->size = sec_size;

        } else if (strcmp(sec_name, ".rodata") == 0){
            rodata_section->offset = sec_off;
            rodata_section->addr = sec_addr;
            rodata_section->size = sec_size;

        } else if (strcmp(sec_name, ".data") == 0){
            data_section->offset = sec_off;
            data_section->addr = sec_addr;
            data_section->size = sec_size;

        } else if (strcmp(sec_name, ".sdata") == 0){
            sdata_section->offset = sec_off;
            sdata_section->addr = sec_addr;
            sdata_section->size = sec_size;
        }
  
    }

    return;
}