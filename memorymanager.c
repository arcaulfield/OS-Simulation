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

//tracks how many programs have been executed
//ensures all programs get a unique program id
int filecount = 1;

//****PRIVATE METHODS****

//add a free frame to the back of the free frame linked list
void addFreeFrame(int frameNum){
    struct Frame* frame = (struct Frame*) malloc(sizeof(struct Frame));
    frame->frameNum = frameNum;
    frame->next = NULL;
    if(fhead == NULL){
        fhead = frame;
        ftail = frame;
    }
    else{
        ftail->next = frame;
        ftail = frame;
    }
}

//clears a frame from RAM and updates usedframes array and the free frame linked list accordingly
void clearFrameFromRam(int frame){
    //clear the frame in ram
    clearProgram(frame*4, frame*4 + 3);
    //set the frame's index in the usedframes array to NULL, indicating that no programs are using the frame
    usedframes[frame] = NULL;
    //add the frame to the back of the free frame linked list
    addFreeFrame(frame);
}

//find the next free frame using FIFO
//updates the free frame linked list
//returns -1 if all frames are currently being used
int findFrame(){
    if(fhead == NULL){
        return -1;
    }
    //get the next free frame on the linked list
    int framenum = fhead->frameNum;

    //adjust the emtpy frame queue accordingly
    Frame * f = fhead;
    fhead = fhead->next;
    free(f);

    return framenum;
}


//finds next victim frame
//returns the frame number of the victim
int findVictim(PCB* pcb){
    //get a random number that is between 0 and 9
    int randnum = rand() % 10;

    //check if the active pcb is using the frame randnum
    for(int i = 0; i < 10; i++){
        if(pcb->pageTable[i] == randnum){
            //start iterating from the beginning if you find that the active pcb is using the frame randnum
            i = -1;
            //increment randnum
            randnum = (randnum + 1) % 10;
        }
    }

    return randnum;
}

// updates the used frame array, which tracks which frames are used by which pcbs
// victimFrame is -1 if there isn't a victim
// returns 0 if there isn't an error
int updatePageTable(PCB * p, int pageNumber, int frameNumber, int victimFrame){
    if(victimFrame != -1){
        for(int i = 0; i < 10; i++){
            if(usedframes[victimFrame] != NULL && usedframes[victimFrame]->pageTable[i] == victimFrame){
                //update the victim's page table
                usedframes[victimFrame]->pageTable[i] = -1;
                //update the active pcb's page table
                p->pageTable[pageNumber] = victimFrame;
                usedframes[victimFrame] = p;
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

//load page pageNumber from the BackingStore into the frameNumber frame of Ram
void loadPage(int pageNumber, FILE * f, int frameNumber){
    //get the index in RAM to start loading the page
    int i = 4*frameNumber;

    //get the line of the file in the backing store
    int fileLine = pageNumber * 4;

    //advance the file pointer to the correct line in the backing store
    char buffer[1000];
    while (!feof(f) && fileLine > 0) {
        //get a line from the file
        memset(buffer, '\0', 999);
        fgets(buffer, 999, f);
        fileLine --;

    }

    int k = 4;

    //load 4 lines into RAM
    while (!feof(f) && k > 0){
        //get a line from the file
        memset(buffer, '\0', 999);
        fgets(buffer, 999, f);

        addLineToRam(buffer, i);

        i++;
        k --;
    }
    //ensures the file pointer points to the beginning of the file
    fseek(f, 0, SEEK_SET);
}


//****PUBLIC METHODS****

// get the total number of lines of code in a file
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

    //ensures that lines that don't end with a newline symbol are counted in the total line count
    if(strcmp(buffer, "") != 0 && strcmp(buffer, "\n") != 0 && strcmp(buffer, "\r\n") != 0){
        linecount++;
    }

    //ensures the file pointer points at the beginning of the file
    fseek(f, 0, SEEK_SET);

    return linecount;
}

// get the total number of pages needed for a program
int countTotalPages(FILE *f){

    //get the total number of lines in a file
    int linecount =  countTotalLines(f);

    int pagecount = 0;

    if (linecount == 0){
        return pagecount;
    }

    // calculate the total number of pages needed
    // each page consists of 4 lines of code
    pagecount = (linecount - 1) / 4 + 1;

    return pagecount;
}


//launches k pages into RAM
//finds a frame in ram to store the page
//loads the page into the frame
//updates the page table of the PCB
//updates the PC of the PCB to point to the first page
void launchKPages(int k, PCB* pcb, FILE* p){
    int pageNum = 0;
    while(k > pageNum){

        int frameNum = findFrame();
        //boolean to indicate whether or not there was a victim
        int victim = -1;

        //Note: this should never be true, because initially only at most 6 pages will be loaded into RAM.
        //However, this allows us to increase the number of programs executing should we ever choose to do so
        if(frameNum == -1){
            frameNum = findVictim(pcb);
            victim = frameNum;
        }

        //update the PC to point to the location of the program's first frame in RAM
        if(pcb->PC == -1){
            pcb->PC = frameNum * 4;
        }

        //update the page table accordingly
        updatePageTable(pcb, pageNum, frameNum, victim);

        if(verbose == 1){
            printf("\nThe updated page table has page: %d stored in frame: %d\n", pageNum, pcb->pageTable[pageNum]);
        }

        //load the page into RAM
        loadPage(pageNum, p, frameNum);

        if(verbose == 1){
            printRam(frameNum *4, frameNum *4 + 3);
        }

        pageNum++;
    }
}

//returns 1 if the program is successfully launched, and 0 otherwise
int launcher(FILE *p) {

    //copy the entire file into the backing store

    //create a file in BackingStore
    //the name of the file is based on the pid of the program (for example, if pid = 2, the filename is 2.txt)
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

    //buffer to store lines from the file
    char buffer[1000];

    //load the program into the file (in the Backing Store) line by line
    while (!feof(p)) {
        memset(buffer, '\0', 999);
        fgets(buffer, 999, p);

        //remove blank lines - don't load them into the Backing Store
        if(strcmp(buffer, "") ==  0 || strcmp(buffer, "\n") ==0 || strcmp(buffer, "\r\n") == 0) {
            continue;
        }

        //load a line of the program into the file in the Backing Store
        fprintf(dest, "%s", buffer);

    }

    //close the file pointer of the file in the Backing Store
    fclose(dest);

    //close the file pointer pointing to the original file
    fclose(p);

    //this opens the file pointer in the backing store, then loads at most 2 pages into RAM
    int errorCode = myinit(destination);

    filecount ++;

    return errorCode;
}


//handles page faults
void handlePageFault(PCB* pcb){

    //we only update the PC_offset and PC_page if the PC_offset currently equals 4 (the program has finished executing a frame)
    if(pcb->PC_offset == 4){
        pcb->PC_page ++;
        if(pcb->PC_page >= pcb->pages_max){
            //set a flag indicating that the program is ready to terminate
            exitProgramFlag = 1;
            return;
        }
    }

    int frame = pcb->pageTable[pcb->PC_page];

    //if the page isn't in ram, load the page into RAM
    if(frame == -1){

        int victimSelected = -1;
        frame = findFrame();

        if(frame == -1){
            frame = findVictim(pcb);
            victimSelected = frame;
        }

        //get the name of the file in the backing store. This is determined by the pid
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

        //open the file in the backing store
        FILE* pcbfile = fopen(destination, "rt");


        updatePageTable(pcb, pcb->PC_page, frame, victimSelected);
        loadPage(pcb->PC_page, pcbfile, frame);


        fclose(pcbfile);

    }
    pcb->PC = frame * 4;

    if(pcb->PC_offset == 4){
        pcb->PC_offset = 0;
    }

    if(verbose == 1){
        printf("HANDLED PAGE FAULT\n");
        printUsedFrames();
        printPCB(pcb);
    }


}

//clears all the frames that a program is using from RAM
void clearRam(PCB* pcb){
    int frame = -1;
    //iterate through the program's page table and clear all frames from RAM
    for(int i = 0; i < 10; i++){
        frame = pcb->pageTable[i];
        if(frame != -1){
            clearFrameFromRam(frame);
        }
    }
}

//clear the program from the backing store
void clearBackingStore(PCB* pcb){
    //get the name of the file of the program in the backing store. This is determined by the pid of the program
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

    //remove the file containing the program from the backing store
    remove(rmStr);
}

//initialize the memory manager
//initialize the seed for the random number generator and the queue for empty frames
//initialize the array of usedFrames to NULL
void initMemoryManager(){
    //initialize a queue of all the emtpy frames
    //this queue allows us to quickly determine the next empty frame
    //frames are chosen using FIFO
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

    //initialize the usedframes array to NULL, which indicates no frames are currently being used
    for(int i = 0; i < 10; i++){
        usedframes[i] = NULL;
    }

    //seed for the random number generator
    srand(time(0));
}

//prints all the frames that are currently being used by programs
//this method is only used for debugging purposes
void printUsedFrames(){
    printf("FRAMES ARE ASSIGNED TO THE FOLLOWING PROGRAMS\n");
    for(int i = 0; i < 10; i++){
        if(usedframes[i] != NULL){
            printf("FRAME: %d PID: %d\n", i, usedframes[i]->pid);
        }
    }
}

//clears the memory manager
//frees all frames in the free frame linked list
void clearMemManager(){
    Frame* node;
    while(fhead != NULL){
        node = fhead;
        fhead = fhead->next;
        free(node);
    }
}

