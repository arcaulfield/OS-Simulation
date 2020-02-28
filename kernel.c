#include <stdio.h>
#include <string.h>
#include "ram.h"
#include "pcb.h"
#include "cpu.h"
#include "interpreter.h"
#include "shell.h"

//****PRIVATE VARIABLES****

//Ready queue
PCB *head, *tail;

//****PRIVATE METHODS****

//clears all programs from the ready queue
//also clears programs from RAM and destroys the PCBs
void clearReadyQueue(){
    //iterate through the queue clearing the programs
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

//removes a PCB from the ready queue
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

//****PUBLIC METHODS****

//initializes a program
//opens the file, adds the program to ram
//then creates a new pcb and adds to the end of the ready queue
int myinit(char *filename){
    printf("myinit has the file %s\n", filename);
    int start = 0;
    int end = 0;

    //FOR DEBUGGING PURPOSES
    char newfile[100];
    memset(newfile, '\0', 100);
    strcat(newfile, "../");
    strcat(newfile, filename);

    //WHEN NOT DEBUGGING, SET newfile to filename
    FILE *file = fopen(filename, "rt");

    if(file == NULL){
        int errorCode = 2; // file not found error
        printf("%s", filename);
        return errorCode;
    }

    //add the program to RAM
    addToRam(file, &start, &end);
    //check if there was an error loading to RAM and handle accordingly
    if(checkErrorFlag() == 1){
        //reset the flag
        resetFlag();
        //clear all of ram
        clearProgram(0, 999);
        int errorCode = 4; //not enough space in RAM error
        return errorCode;
    }

    //create a new PCB
    PCB* newPCB = makePCB(start, end);

    //add the PCB to the read list
    addToReady(newPCB);

    fclose(file);
    return 0;
}


//This function is called once a program has finished executing.
//It clears the program from RAM and destroys the PCB
void finishExecuting(PCB* pcb){

    //clear program from ram
    clearProgram(pcb->start, pcb->end);

    //remove the PCB from the Ready list
    removeFromReady(pcb);

    //clear PCB
    clearPCB(pcb);
}


//runs the programs that are on the ready queue
//each program runs for a quanta then is placed at the back of the ready queue
int scheduler(){
    int quanta = 2;
    int errorCode = 0;
    PCB* pcb = head;

    while(pcb != NULL){
        //check if the CPU is available
        if(cpuAvailable() == 0){

            //shorten the length of the quanta if the number of lines left in the program is less than one quanta
            if(pcb->end - pcb->PC < quanta){
                quanta = pcb->end - pcb->PC + 1;
            }
            //update the instruction pointer of the CPU
            updateIP(pcb->PC);

            //run the CPU
            errorCode = run(quanta);

            //handle any errors
            if(errorCode != 0 || exitProgramFlag == 1){
                //print information about the error code for the user
                if(errorCode == 1){
                    printf("Unknown command.\n");
                }
                //error code for scripts that can't be found
                else if(errorCode == 2){
                    printf("Script not found.\n");
                }
                //error code for not being enough space to load programs into ram
                else if(errorCode == 4){
                    printf("Error: there was not enough space to load the program(s) into RAM.\n");
                }
                exitProgramFlag = 0;
                finishExecuting(pcb);

            } else {
                pcb->PC = pcb->PC + quanta;
                if(pcb->PC > pcb->end){
                    finishExecuting(pcb);
                }
                else{
                    removeFromReady(pcb);
                    addToReady(pcb);
                }
            }

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

