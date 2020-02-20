#include <stdio.h>
#include <string.h>
#include "ram.h"
#include "pcb.h"
#include "cpu.h"
#include "interpreter.h"
#include "shell.h"


//Ready queue
PCB *head, *tail;

//clears all programs from the ready queue
//also clears programs from RAM
void clearReadyQueue(){
    while(head != NULL){
        PCB * pcb = head;
        head = head->next;
        clearProgram(pcb->start, pcb->end);
        clearPCB(pcb);
    }
    tail = NULL;
}

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
    if(head == NULL){
        return;
    }
    //if the pcb is the head, then remove it
    if(head->start == pcb->start && head->end == pcb->end){
        head = head->next;
    }
    //we should only be removing from the head in general
    //however this allows for PCBs to be removed from anywhere in the list should this ever be needed
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

    addToRam(file, &start, &end);
    if(checkErrorFlag() == 1){
        //reset the flag
        resetFlag();
        int errorCode = 4; //not enough space in RAM error
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
//It clears the program from RAM
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
                quanta = pcb->end - pcb->PC + 1;
            }

            updateIP(pcb->PC);
            printf("***EXECUTING***\n\n");
            errorCode = runQ(quanta);
            printf("\n******\n");
            if(errorCode != 0 || exitProgramFlag == 1){
                exitProgramFlag = 0;
                finishExecuting(pcb);
            }
            else{
                pcb->PC = pcb->PC + quanta;
                if(pcb->PC > pcb->end){
                    finishExecuting(pcb);
                }
                else{
                    removeFromReady(pcb);
                    addToReady(pcb);
                }
            }
            printf("AFTER ONE QUANTA, THE READY LIST LOOKS LIKE THIS:\n");
            printReadyQueue();

            pcb = head;
            quanta = 2;
        }

    }
    return 0;
}

int main(){


    //instantiate ram
    initRam();
    //instantiate the CPU
    initCPU();

    printf("Kernel 1.0 loaded!\n");
    shellUI();
}

