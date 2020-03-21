#include <stdlib.h>
#include "pcb.h"

//****PUBLIC METHODS****

//Creates a new PCB, which points to a program in RAM
PCB* makePCB(int pid, int pages_max){
    struct PCB* newPCB = (struct PCB*) malloc(sizeof(struct PCB));
    newPCB->pid = pid;
    newPCB->PC = -1;
    newPCB->pages_max = pages_max;
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

//prints the contents of a pcb for debugging purposes
void printPCB(PCB* pcb){
    printf("Created PCB with PID: %d PC: %d pages_max: %d PC_page: %d PC_offset: %d\n", pcb->pid, pcb->PC, pcb->pages_max, pcb->PC_page, pcb->PC_offset);
    printf("The page table for this PCB is:\n");
    for(int i = 0; i < 10; i++){
        printf("Page %d in frame %d\n", i, pcb->pageTable[i]);
    }
}
