#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ram.h"
#include "pcb.h"
#include "cpu.h"


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

//removes a PCB from the ready list
void removeFromReady(PCB* pcb){
    //remove from ready list
    PCB* node = head;
    if(head->next == NULL){
        head = NULL;
        tail = NULL;
    }
        //we should only be removing from the head in general
    else if(head->start == pcb->start && head->end == pcb->end){
        head = head->next;
    }
    else{
        while(node->next != NULL ){
            if(node->next->start == pcb->start && node->next->end == pcb->end){
                node->next = node->next->next;
                if(node->next == NULL){
                    tail = node;
                }
            }
            node = node->next;
        }
    }
    node->next = NULL;
}

//prints the contents of the ready queue
//this is useful for debugging
void printReadyQueue(){
    printf("PRINTING CONTENTS OF QUEUE\n");
    PCB* node = head;
    while(node != NULL){
        printf("PCB with start %d, end %d and program counter %d\n", node->start, node->end, node->PC);
        //printRam(node->start, node->end);
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

   // printf("Created a PCB with start %d, end %d and program counter %d\n", newPCB->start, newPCB->end, newPCB->PC);

    addToReady(newPCB);
    //printReadyQueue();
    fclose(file);
    return 0;
}


//This function is called once a program has finished executing.
void finishExecuting(PCB* pcb){

    removeFromReady(pcb);

    //clear program from ram
    clearProgram(pcb->start, pcb->end);

    //clear PCB
    clearPCB(pcb);
}


int scheduler(){
    int quanta = 2;
    int errorCode = 0;
    PCB* pcb = head;

    printf("AFTER PUTTING ON RUN LIST, WE GET THE FOLLOWING READY LIST:\n");
    printReadyQueue();
    while(pcb != NULL){
        if(cpuAvailable() == 0){
            //shorten the length of the quanta if the number of lines left in the program is less than the quanta
            if(pcb->end - pcb->PC < quanta){
                quanta = pcb->PC - pcb->end + 1;
            }

            updateIP(pcb->PC);
            printf("***EXECUTING***\n\n");
            errorCode = runQ(quanta);
            printf("\n******\n");
            if(errorCode != 0){
                return errorCode;
            }
            pcb->PC = pcb->PC + quanta;
            if(pcb->PC > pcb->end){
                finishExecuting(pcb);
            }
            else{
                removeFromReady(pcb);
                addToReady(pcb);
            }
            printf("AFTER ONE QUANTA, THE READY LIST LOOKS LIKE THIS:\n");
            printReadyQueue();

            pcb = head;
            quanta = 2;
        }

    }
    return 0;
}

