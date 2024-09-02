#include "my_types.h"

void initialize_sec_header(SectionHeader *section){
    
    section->offset = 0;
    section->addr = 0;
    section->size = 0;

    return;
}

void initialize_text_segment(TextSegment *text_segment){

    text_segment->entry = 0;
    text_segment->addr = 0;
    text_segment->size = 0;

    return;
}


void initialize_data_segment(DataSegment *data_segment){
    
    data_segment->addr = 0;
    data_segment->size = 0;

    return;
}
void initialize_simulated_memory(SimulatedMemory *simulated_memory){
    
    initialize_text_segment(&simulated_memory->text_seg);
    initialize_data_segment(&simulated_memory->rodata_seg);
    initialize_data_segment(&simulated_memory->data_seg);
    initialize_data_segment(&simulated_memory->sdata_seg);
    
    return;
}