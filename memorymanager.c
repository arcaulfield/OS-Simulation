#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "memorymanager.h"
#include "ram.h"
#include "pcb.h"
#include "kernel.h"

//****PRIVATE VARIABLES****


//the index is the frame number and the PCB* is the pcb currently using this frame number
PCB* usedframes[10];

//Frame struct
typedef struct Frame{
    int frameNum;
    struct Frame* next;
}Frame;

//linked list of empty frames
Frame* fhead = NULL;
Frame* ftail = NULL;

//****PUBLIC VARIABLES****

int filecount = 1;

//****PRIVATE METHODS****

//find the next free frame using FIFO
//updates the free frame linked list
int findFrame(){
    if(fhead == NULL){
        return -1;
    }
    int framenum = fhead->frameNum;

    //adjust the emtpy frame queue accordingly
    Frame * f = fhead;
    fhead = fhead->next;
    free(f);

    return framenum;
}

//add a freed frame to the back of the free frame linked list
void addFreeFrame(int frameNum){
    struct Frame* frame = (struct Frame*) malloc(sizeof(struct Frame));
    frame->frameNum = frameNum;
    frame->next = NULL;
    if(ftail == NULL){
        fhead = frame;
        ftail = frame;
    }
    else{
        ftail->next = frame;
        ftail = frame;
    }
}


//finds next victim frame
int findVictim(PCB* pcb){
    srand(time(0));

    //get a random number that is between 0 and 9
    int randnum = rand() % 10;
    for(int i = 0; i < 10; i++){
        if(pcb->pageTable[i] == randnum){
            //start iterating from the beginning if you find a frame number that isn't used by the active pcb
            i = -1;
            randnum = (randnum + 1) % 10;
        }
    }

    return randnum;
}

// make victimFrame be 1 if there is a victim
// updates the used frame array, which tracks which frames are used by which pcbs
// returns 0 if there isn't an error
int updatePageTable(PCB * p, int pageNumber, int frameNumber, int victimFrame){
    if(victimFrame == 1){
        for(int i = 0; i < 10; i++){
            if(usedframes[frameNumber]->pageTable[i] == frameNumber){
                usedframes[frameNumber]->pageTable[i] = -1;
                p->pageTable[pageNumber] = frameNumber;
                usedframes[frameNumber] = p;
                return 0;
            }
        }

    }
    else{
        p->pageTable[pageNumber] = frameNumber;
        usedframes[frameNumber] = p;
        return 0;
    }
    //return 1 if there is an error updating page tables
    return 1;
}

// get the total number of necessary pages for a file
int countTotalPages(FILE *f){
    int linecount = 0;
    int pagecount = 0;

    char buffer[1000];
    while (!feof(f)) {
        //get a line from the file
        memset(buffer, '\0', 999);
        fgets(buffer, 999, f);
        linecount++;

    }
    linecount -- ;

    if (linecount == 0){
        return 0;
    }
    // each page consists of 4 lines of code
    pagecount = (linecount - 1) / 4 + 1;

    //ensures the file pointer points at the beginning of the file
    fseek(f, 0, SEEK_SET);

    return pagecount;
}


//load page pageNumber from the BackingStore into the frameNumber of Ram
void loadPage(int pageNumber, FILE * f, int frameNumber){
    int i = 4*frameNumber;

    if(ram[i] != NULL){
        //RAM ERROR - figure out how to handle
        return;
    }

    int fileLine = pageNumber * 4;

    char buffer[1000];
    while (!feof(f) && fileLine > 0) {
        //get a line from the file
        memset(buffer, '\0', 999);
        fgets(buffer, 999, f);
        fileLine --;

    }

    int k = 4;

    while (!feof(f) && k > 0){
        //get a line from the file
        memset(buffer, '\0', 999);
        fgets(buffer, 999, f);

        ram[i] = strdup(buffer);
        i++;
        //if there isn't enough space in RAM, deal with the error
        if(i > 39 || ram[i] != NULL){
            //set the load error flag to 1, indicating that there isn't enough space in RAM
            return;
        }
        k --;
    }
    //ensures the file pointer points at the beginning of the file
    fseek(f, 0, SEEK_SET);
}


//****PUBLIC METHODS****

//returns 1 if the program is successfully launched, and 0 otherwise
int launcher(FILE *p) {

    //create the name of the file in BackingStore
    char numbuffer[5];
    sprintf(numbuffer, "%d", filecount);

    char destination[30];
    memset(destination, '\0', 30);
    //REMOVE ../ WHEN NOT DEBUGGING
    strncpy(destination, "../BackingStore/", 29);

    strcat(destination, numbuffer);
    strcat(destination, ".txt");

    //create the new file in BackingStore
    FILE *dest = fopen(destination, "w");

    if(dest == NULL){
        //there was an error loading the file into backing store
        return 0;
    }

    //buffer to store lines from the file
    char buffer[1000];

    //load the file into RAM line by line
    while (!feof(p)) {
        memset(buffer, '\0', 999);
        fgets(buffer, 999, p);

        //remove blank lines - don't load them into the Backing Store
        if(strcmp(buffer, "") ==  0 || strcmp(buffer, "\n") ==0 || strcmp(buffer, "\r\n") == 0) {
            continue;
        }

        fprintf(dest, "%s", buffer);

    }

    //close the destination file pointer
    fclose(dest);

    //close the original file pointer
    fclose(p);

    myinit(destination);

    filecount ++;

    return 1;
}


//handles page faults
void handlePageFault(PCB* pcb){
    pcb->PC_page ++;
    if(pcb->PC_page >= pcb->pages_max){
        finishExecuting(pcb);
        return;
    }



    int frame = pcb->pageTable[pcb->PC_page];
    //if the page is already in ram
    if(frame != -1){
        pcb->PC = 4*frame;
    }else{
        //open a file pointer to the file in the Backing Store

        int victimSelected = 0;

        frame = findFrame();
        if(frame == -1){
            frame = findVictim(pcb);
            victimSelected = 1;

        }

        //get the name of the file in the backing store -> this is determined by the pid
        char numbuffer[5];
        sprintf(numbuffer, "%d", pcb->pid);

        char destination[30];
        memset(destination, '\0', 30);
        //REMOVE ../ WHEN NOT DEBUGGING
        strncpy(destination, "../BackingStore/", 29);

        strcat(destination, numbuffer);
        strcat(destination, ".txt");

        FILE* pcbfile = fopen(destination, "rt");


        updatePageTable(pcb, pcb->PC_page, frame, victimSelected);
        loadPage(pcb->PC_page, pcbfile, frame);

        pcb->PC = frame * 4;
        pcb->PC_offset = 0;

        fclose(pcbfile);


    }

}


//initialize a queue of all the emtpy frames
//this queue allows us to quickly determine the next empty frame
//frames are chosen using FIFO
void  initEmptyFrameQueue(){
    struct Frame* oldFrame = (struct Frame*) malloc(sizeof(struct Frame));
    oldFrame->frameNum = 0;
    fhead = oldFrame;
    for(int i = 1; i < 10; i ++){
        struct Frame* newFrame = (struct Frame*) malloc(sizeof(struct Frame));
        newFrame->frameNum = i;
        newFrame->next = NULL;
        oldFrame->next = newFrame;
        oldFrame = newFrame;
    }
    ftail = oldFrame;
}

void printUsedFrames(){
    printf("FRAMES ARE ASSIGNED TO THE FOLLOWING PROGRAMS\n");
    for(int i = 0; i < 10; i++){
        if(usedframes[i] != NULL){
            printf("FRAME: %d PID: %d\n", i, usedframes[i]->pid);
        }
    }
}



