#ifndef ASSIGNMENT2_PCB_H
#define ASSIGNMENT2_PCB_H

#include <stdio.h>
//PCB struct
typedef struct PCB{
    int PC;
    int start;
    int end;
    int pageTable[10];
    int PC_page;
    int PC_offset;
    int pages_max;
    FILE* f;
    struct PCB* next;
}PCB;

PCB* makePCB(int PID, int start, int end, int pages_max, FILE* f);
void clearPCB(PCB*);

#endif //ASSIGNMENT2_PCB_H
