#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ram.h"
#include "pcb.h"


PCB *head, *tail;

//adds a PCB to the back of the ready queue
void addToReady(PCB* newPCB){
    if(head == NULL){
        head = newPCB;
        tail = newPCB;
    }
    else{
        tail->next = newPCB;
        tail = newPCB;
    }
}

//prints the contents of the ready queue
//this is useful for debugging
void printReadyQueue(){
    printf("PRINTING CONTENTS OF QUEUE\n");
    PCB* node = head;
    while(node != NULL){
        printf("Visited a PCB with start %d, end %d and program counter %d\n", node->start, node->end, node->PC);
        printRam(node->start, node->end);
        node = node->next;
    }
}

int myinit(char *filename){
    int start = 0;
    int end = 0;

    //FOR DEBUGGING PURPOSES
    char newfile[100];
    memset(newfile, '\0', 100);
    strcat(newfile, "../");
    strcat(newfile, filename);

    //WHEN NOT DEBUGGING, SET newfile to filename
    FILE *file = fopen(newfile, "rt");

    if(file == NULL){
        int errorCode = 2; // file not found error
        return errorCode;
    }

    int errorCode = addToRam(file, &start, &end);
    if(errorCode !=  0){
        return errorCode;
    }
    PCB* newPCB = makePCB(start, end);

    printf("Created a PCB with start %d, end %d and program counter %d\n", newPCB->start, newPCB->end, newPCB->PC);

    addToReady(newPCB);
    printReadyQueue();
    return 0;
}

int finishExecuting(char* filename){
    return 1;
}

