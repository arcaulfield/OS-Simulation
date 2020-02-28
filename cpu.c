#include <string.h>
#include <stdlib.h>
#include "ram.h"
#include "shell.h"
#include "interpreter.h"

//****PRIVATE VARIABLES****

typedef struct CPU{
    int IP;
    char IR[1000];
    int quanta;
}CPU;

CPU* myCPU;

//****PUBLIC METHODS****


//initialise the CPU
void initCPU(){
    myCPU = (CPU*) malloc(sizeof(CPU*));
    myCPU->quanta = 2;
    myCPU->IR[0] = '\0';
}

//frees the CPU
void freeMyCPU(){
    free(myCPU);
}

//updates the instruction pointer's value
void updateIP(int ip){
    myCPU->IP = ip;
}

//return 0 if the CPU is available, return 1 otherwise
//the CPU is available if the quanta is finished or if nothing has been assigned to the CPU
int cpuAvailable(){
    if(myCPU->quanta == 2 || strcmp(myCPU->IR, "\0") == 0){
        return 0;
    }
    else{
        return 1;
    }
}

//runs the CPU for a quanta
int run(int quanta){
    myCPU->quanta = 0;
    int errorCode = 0;
    //Run quanta number of lines
    while(myCPU->quanta < quanta){
        //get a line from RAM
        strcpy(myCPU->IR, getLineFromRam(myCPU->IP));

        myCPU->IP ++;

        //parse, interpret and run line
        errorCode = parse(myCPU->IR);

        if(errorCode != 0 || exitProgramFlag == 1){
            myCPU->quanta = 2;
            myCPU->IR[0] = '\0';
            return errorCode;
        }
        myCPU->quanta ++;
        myCPU->IR[0] = '\0';
    }
    return errorCode;
}