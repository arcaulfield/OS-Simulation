#ifndef ASSIGNMENT2_PCB_H
#define ASSIGNMENT2_PCB_H

typedef struct PCB{
    int PC;
    int start;
    int end;
}PCB;


PCB* makePCB(int start, int end);

#endif //ASSIGNMENT2_PCB_H
