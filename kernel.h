#ifndef ASSIGNMENT2_KERNEL_H
#define ASSIGNMENT2_KERNEL_H
#include "pcb.h"

extern int debug;
extern int verbose;

int myinit(char *filename);
int scheduler();
void clearReadyQueue();
void freeCPU();

#endif //ASSIGNMENT2_KERNEL_H
