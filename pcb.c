#include <stdlib.h>
#include "pcb.h"

PCB* makePCB(int start, int end){
    struct PCB* newPCB = (struct PCB*) malloc(sizeof(struct PCB));
    newPCB->end = end;
    newPCB->start = start;
    newPCB->PC = start;
    return newPCB;
}
