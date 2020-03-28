#include <stdio.h>
#include "pcb.h"

#ifndef ASSIGNMENT2_MEMORYMANAGER_H
#define ASSIGNMENT2_MEMORYMANAGER_H
extern int filecount;

void launchKPages(int k, PCB *pcb, FILE *p);
int launcher(FILE *p);
void initMemoryManager();
void handlePageFault(PCB* pcb);
int countTotalPages(FILE *f);
void printUsedFrames();
int countTotalLines(FILE *f);
void clearRam(PCB* pcb);
void clearBackingStore(PCB* pcb);
void clearMemManager();


#endif //ASSIGNMENT2_MEMORYMANAGER_H
