#ifndef ASSIGNMENT2_KERNEL_H
#define ASSIGNMENT2_KERNEL_H
#include "pcb.h"


int myinit(char *filename);
int scheduler();
void clearReadyQueue();
void freeCPU();
int kernel();
void boot();

#endif //ASSIGNMENT2_KERNEL_H
