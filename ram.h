
#ifndef ASSIGNMENT2_RAM_H
#define ASSIGNMENT2_RAM_H
#include <stdio.h>

//string arrays of size 40
extern char *ram[1000];


void resetFlag();
void addToRam(FILE *p, int* start, int* end);
void clearProgram(int start, int end);
char* getLineFromRam(int line);
int checkErrorFlag();
void printRam(int start, int end);

#endif //ASSIGNMENT2_RAM_H
