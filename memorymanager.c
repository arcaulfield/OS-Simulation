#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "memorymanager.h"
#include "ram.h"
#include "pcb.h"
#include "kernel.h"
#include "interpreter.h"

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


// get the total number of lines for a file
int countTotalLines(FILE *f){
    int linecount = 0;

    char buffer[1000];
    while (!feof(f)) {
        //get a line from the file
        memset(buffer, '\0', 999);
        fgets(buffer, 999, f);
        linecount++;

    }
    linecount -- ;

    //ensures the file pointer points at the beginning of the file
    fseek(f, 0, SEEK_SET);

    return linecount;
}

//load page pageNumber from the BackingStore into the frameNumber of Ram
void loadPage(int pageNumber, FILE * f, int frameNumber){
    int i = 4*frameNumber;

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
        if(i > 39 || ram[i] != NULL){
            //set the load error flag to 1, indicating that there isn't enough space in RAM
            return;
        }
        //get a line from the file
        memset(buffer, '\0', 999);
        fgets(buffer, 999, f);

        ram[i] = strdup(buffer);
        i++;
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
    if(debug == 1){
        strncpy(destination, "../", 29);
    }
    strcat(destination, "BackingStore/");

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

    int errorCode = myinit(destination);

    filecount ++;

    return errorCode;
}


//handles page faults
void handlePageFault(PCB* pcb){

    //handle page faults that occur when the page that is needed for a program is taken by another program
    //we only update the PC_offset and PC_page if the PC_offset currently equals 4
    if(pcb->PC_offset == 4){
        pcb->PC_page ++;
        if(pcb->PC_page >= pcb->pages_max){
            //set a flag indicating that the program is ready to terminate
            exitProgramFlag = 1;
            return;
        }
    }

    int frame = pcb->pageTable[pcb->PC_page];

    //if the page is already in ram
    if(frame == -1){
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
        if(debug == 1){
            strncpy(destination, "../", 29);
        }
        strcat(destination, "BackingStore/");

        strcat(destination, numbuffer);
        strcat(destination, ".txt");

        FILE* pcbfile = fopen(destination, "rt");


        updatePageTable(pcb, pcb->PC_page, frame, victimSelected);
        loadPage(pcb->PC_page, pcbfile, frame);


        fclose(pcbfile);

    }
    pcb->PC = frame * 4;
    if(pcb->PC_offset == 4) {
        pcb->PC_offset = 0;
    }

    printf("HANDLED PAGE FAULT\n");
    printUsedFrames();
    printPCB(pcb);

}

//clears a frame from RAM and updates usedframes and the free frame linked list accordingly
void clearFrameFromRam(int frame){
    clearProgram(frame*4, frame*4 + 3);
    usedframes[frame] = NULL;
    addFreeFrame(frame);
}

//clears all the frames that a program is using from RAM
void clearRam(PCB* pcb){
    int frame = -1;
    for(int i = 0; i < 10; i++){
        frame = pcb->pageTable[i];
        if(frame != -1){
            clearFrameFromRam(frame);
        }
    }
}


//clear the program from the backing store
void clearBackingStore(PCB* pcb){
    //get the name of the file in the backing store -> this is determined by the pid
    char numbuffer[5];
    sprintf(numbuffer, "%d", pcb->pid);

    char rmStr[30];
    memset(rmStr, '\0', 30);

    if(debug == 1){
        strncpy(rmStr, "../", 29);
    }
    strcat(rmStr, "BackingStore/");

    strcat(rmStr, numbuffer);
    strcat(rmStr, ".txt");

    remove(rmStr);
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

void clearMemManager(){
    Frame* node;
    while(fhead != NULL){
         node = fhead;
        fhead = fhead->next;
        free(node);
    }
}

