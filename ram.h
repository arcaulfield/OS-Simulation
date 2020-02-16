
#ifndef ASSIGNMENT2_RAM_H
#define ASSIGNMENT2_RAM_H
#include <stdio.h>

void initRam();
int loadProgram(char* filename);
void printRam();
int addToRam(FILE *p, int* start, int* end);

#endif //ASSIGNMENT2_RAM_H
