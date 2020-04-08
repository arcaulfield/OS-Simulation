#include <stdlib.h>
#include "pcb.h"

//****PUBLIC METHODS****

//Creates a new PCB, which points to a program in RAM
PCB* makePCB(int pid, int pages_max, int linecount){
    struct PCB* newPCB = (struct PCB*) malloc(sizeof(struct PCB));
    newPCB->pid = pid;
    newPCB->PC = -1;
    newPCB->pages_max = pages_max;
    newPCB->linecount = linecount;
    newPCB->PC_offset = 0;
    newPCB->PC_page = 0;
    for(int i = 0; i < 10; i++){
        newPCB->pageTable[i] = -1;
    }
    //this points to the next PCB in the ready queue
    newPCB->next = NULL;
    return newPCB;
}

//Destroys a PCB
void clearPCB(PCB* pcb){
    free(pcb);
}

