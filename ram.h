
#ifndef ASSIGNMENT2_RAM_H
#define ASSIGNMENT2_RAM_H
#include <stdio.h>

void initRam();
void printRam(int start, int end);
int addToRam(FILE *p, int* start, int* end);
void clearProgram(int start, int end);
char* getLineFromRam(int line);

#endif //ASSIGNMENT2_RAM_H
