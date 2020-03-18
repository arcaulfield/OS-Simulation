#include <stdlib.h>
#include "pcb.h"

//****PUBLIC METHODS****

//Creates a new PCB, which points to a program in RAM
PCB* makePCB(int start, int end, int pages_max, FILE* f){
    struct PCB* newPCB = (struct PCB*) malloc(sizeof(struct PCB));
    newPCB->end = end;
    newPCB->start = start;
    newPCB->PC = start;
    newPCB->pages_max = pages_max;
    newPCB->PC_page = 0;
    for(int i = 0; i < 10; i++){
        newPCB->pageTable[i] = -1;
    }
    newPCB->f = f;
    //this points to the next PCB in the ready queue
    newPCB->next = NULL;
    return newPCB;
}

//Destroys a PCB
void clearPCB(PCB* pcb){
    free(pcb);
}
