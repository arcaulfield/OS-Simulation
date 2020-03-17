#ifndef ASSIGNMENT2_PCB_H
#define ASSIGNMENT2_PCB_H

//PCB struct
typedef struct PCB{
    int PC;
    int start;
    int end;
    int pageTable[10];
    struct PCB* next;
}PCB;

PCB* makePCB(int start, int end);
void clearPCB(PCB*);

#endif //ASSIGNMENT2_PCB_H
