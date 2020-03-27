
#ifndef ASSIGNMENT2_RAM_H
#define ASSIGNMENT2_RAM_H
#include <stdio.h>


void resetFlag();
void initRam();
void addLineToRam(char* buffer, int i);
void clearProgram(int start, int end);
char* getLineFromRam(int line);
int checkErrorFlag();
void printRam(int start, int end);

#endif //ASSIGNMENT2_RAM_H
