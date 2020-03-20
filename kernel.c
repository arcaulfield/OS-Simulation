#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ram.h"
#include "pcb.h"
#include "cpu.h"
#include "interpreter.h"
#include "shell.h"
#include "memorymanager.h"

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
        //******************************************************************************************************************************************************
        //clearProgram(pcb->start, pcb->end);
        clearPCB(pcb);
    }
    tail = NULL;
}

//PUBLIC
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
    if(head == NULL){
        return;
    }
    //if the pcb is the head, then remove it
    //******************************************************************************************************************************************************
//    if(head->start == pcb->start && head->end == pcb->end){
//        if(ftail->start == pcb->start && ftail->end == pcb->end){
//            ftail = NULL;
//            head = NULL;
//        }
//        else{
//            head = head->next;
//            pcb->next = NULL;
//        }
//    }
    //we should only be removing from the head in general
    //however this allows for PCBs to be removed from anywhere in the list should this ever be needed
    else{
        PCB* node = head;
        //******************************************************************************************************************************************************

//        while(node->next != NULL ){
//            if(node->next->start == pcb->start && node->next->end == pcb->end){
//                node->next = node->next->next;
//                if(node->next == NULL){
//                    ftail = node;
//                }
//            }
//            node = node->next;
//        }
        node->next = NULL;
    }

}

//prints the contents of the ready queue
//this is useful for debugging
void printReadyQueue(){
    printf("PRINTING CONTENTS OF QUEUE\n");
    PCB* node = head;
    while(node != NULL){
        //******************************************************************************************************************************************************
        //printf("PCB with start %d, end %d and program counter %d\n", node->start, node->end, node->PC);
        node = node->next;
    }
}

void initReadyQueue(){
    head = NULL;
    tail = NULL;
}

//****PUBLIC METHODS****

//initializes a program
//then creates a new pcb and adds to the end of the ready queue
int myinit(char *filename){

    //open a file pointer to the new file in BackingStore
    FILE* p = fopen(filename, "rt");
    if(p == NULL){
        return 0;
    }

    int max_pages = countTotalPages(p);

    //create a new PCB
    PCB* newPCB = makePCB(filecount, max_pages);

    int i = 0;
    //number of pages to load into ram when launch program
    int k = 2;
    if(max_pages < 2){
        k = 1;
    }

    int pageNum = 0;
    while(k > pageNum){

        int frameNum = findFrame();

        if(newPCB->PC == -1){
            newPCB->PC = frameNum * 4;
        }
        updatePageTable(newPCB, pageNum, frameNum, 0);
        printf("\nThe updated page table has page: %d stored in frame: %d\n", pageNum, newPCB->pageTable[pageNum]);

        loadPage(pageNum, p, frameNum);
        printRam(frameNum *4, frameNum *4 + 3);

        i++;
        pageNum++;
    }

    //close the destination file pointer
    fclose(p);

    printPCB(newPCB);
    //add the PCB to the read list
    addToReady(newPCB);


    return 0;
}


//This function is called once a program has finished executing.
//It clears the program from RAM and destroys the PCB
void finishExecuting(PCB* pcb){

    //clear program from ram

    //******************************************************************************************************************************************************
    //clearProgram(pcb->start, pcb->end);

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
            if(4 - pcb->PC_offset < quanta){
                quanta = 4 - pcb->PC_offset;
            }
            //update the instruction pointer of the CPU
            updateIP(pcb->PC, pcb->PC_offset);

            //run the CPU
            errorCode = run(quanta);

            if(pageFaultFlag == 1){
                pageFaultFlag = 0;
                handlePageFault(pcb);

            }
            //handle any errors
            else if(exitProgramFlag == 1 || errorCode != 0){

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
                //******************************************************************************************************************************************************
//                if(pcb->PC > pcb->end){
//                    finishExecuting(pcb);
//                }
//                else{
//                    removeFromReady(pcb);
//                    addToReady(pcb);
//                }
            }

            pcb = head;
            quanta = 2;
        }

    }
    return 0;
}



void freeCPU(){
    freeMyCPU();
}


int kernel(){
    //instantiate the CPU
    initCPU();
    //initialize the ready queue
    initReadyQueue();

    printf("Kernel 1.0 loaded!\n");
    int error = shellUI();
    return error;
}


void boot(){
    //initialize all cells of ram to null, indicating that there are no pages of code in RAM
    for(int i = 0; i < 40; i++){
        ram[i] = NULL;
    }

    //initialize the queue containing all empty frames
    initEmptyFrameQueue();

    // REMOVE THE ../ FOR NON DEBUG MODE
    system("rm -rf ../BackingStore/");
    //ERROR HANDLING!
    system("mkdir ../BackingStore");
}


int main(){
    int error = 0;
    boot();
    error = kernel();
    return error;
}
