#include <stdio.h>
#include "pcb.h"

#ifndef ASSIGNMENT2_MEMORYMANAGER_H
#define ASSIGNMENT2_MEMORYMANAGER_H
extern int filecount;


int launcher(FILE *p);
void initEmptyFrameQueue();
void handlePageFault(PCB* pcb);
void loadPage(int pageNumber, FILE * f, int frameNumber);
int countTotalPages(FILE *f);
int updatePageTable(PCB * p, int pageNumber, int frameNumber, int victimFrame);
int findVictim(PCB* pcb);
int findFrame();
void printUsedFrames();


#endif //ASSIGNMENT2_MEMORYMANAGER_H
