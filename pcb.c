#include <stdlib.h>
#include "pcb.h"



//Creates a new PCB, which points to a program in RAM
PCB* makePCB(int start, int end){
    struct PCB* newPCB = (struct PCB*) malloc(sizeof(struct PCB));
    newPCB->end = end;
    newPCB->start = start;
    newPCB->PC = start;
    //points to the next PCB in the ready queue
    newPCB->next = NULL;
    return newPCB;
}

//Clears a PCB
void clearPCB(PCB* pcb){
    free(pcb);
}
