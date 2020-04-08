#ifndef ASSIGNMENT2_PCB_H
#define ASSIGNMENT2_PCB_H

#include <stdio.h>
//PCB struct
typedef struct PCB{
    int pid;
    int PC;
    int pageTable[10];
    int PC_page;
    int PC_offset;
    int pages_max;
    int linecount;
    struct PCB* next;
}PCB;

PCB* makePCB(int pid, int pages_max, int linecount);
void clearPCB(PCB*);

#endif //ASSIGNMENT2_PCB_H
