#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "memorymanager.h"
#include "ram.h"
#include "pcb.h"

//****PRIVATE VARIABLES****
int filecount = 1;

//Frame struct
typedef struct Frame{
    int frameNum;
    struct Frame* next;
}Frame;

Frame* head;
Frame* tail;

//****PRIVATE METHODS****

//find the next free frame using FIFO
int findFrame(){
    if(head == NULL){
        return -1;
    }
    int framenum = head->frameNum;

    //adjust the emtpy frame queue accordingly
    Frame * f = head;
    head = head->next;
    free(f);

    return framenum;
}

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

// get the total count of necessary pages for a file
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
    printf("linecount %d\n", linecount);

    if (linecount == 0){
        // handle
    }
    // each page consists of 4 lines of code
    pagecount = (linecount - 1) / 4 + 1;

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
        return 0;
    }

    //get a line from the file
    char buffer[1000];


    //load the file into RAM line by line
    while (!feof(p)) {
        memset(buffer, '\0', 999);
        fgets(buffer, 999, p);

        //remove blank lines - don't load them into
        if(strcmp(buffer, "") ==0 || strcmp(buffer, "\n") ==0 || strcmp(buffer, "\r\n") ==0) {
            continue;
        }

        fprintf(dest, "%s", buffer);

    }

    fclose(dest);

    //close the original file pointer
    fclose(p);


    //open the new file in BackingStore
    dest = fopen(destination, "rt");
    if(dest == NULL){
        return 0;
    }

    int pagecount = countTotalPages(dest);

    printf("pagecount is: %d\n", pagecount);
    //ensures the file pointer points at the beginning of the file
    fseek(dest, 0, SEEK_SET);





    fclose(dest);
    filecount ++;

    return 1;
}


//initialize a queue of all the emtpy frames
//this queue allows us to quickly determine the next empty frame
//frames are chosen using FIFO
void initEmptyFrameQueue(){
    struct Frame* oldFrame = (struct Frame*) malloc(sizeof(struct Frame));
    oldFrame->frameNum = 0;
    head = oldFrame;
    for(int i = 1; i < 10; i ++){
        struct Frame* newFrame = (struct Frame*) malloc(sizeof(struct Frame));
        newFrame->frameNum = i;
        newFrame->next = NULL;
        oldFrame->next = newFrame;
        oldFrame = newFrame;
    }
    tail = oldFrame;
}

