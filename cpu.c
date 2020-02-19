#include <string.h>
#include <stdlib.h>
#include "ram.h"
#include "shell.h"


typedef struct CPU{
    int IP;
    char IR[1000];
    int quanta;
}CPU;

CPU* myCPU;

void initCPU(){
    myCPU = (CPU*) malloc(sizeof(CPU*));
    myCPU->quanta = 2;
    myCPU->IR[0] = '\0';
}

void updateIP(int ip){
    myCPU->IP = ip;
}

//return 0 is the CPU is available, return 1 otherwise
//the CPU is available if the quanta is finished or if nothing has been assigned to the CPU
int cpuAvailable(){
    if(myCPU->quanta == 2 || strcmp(myCPU->IR, "\0") == 0){
        return 0;
    }
    else{
        return 1;
    }
}

int runQ(int quanta){
    myCPU->quanta = 0;
    int errorCode = 0;
    //Copy over quanta number of lines of code
    while(myCPU->quanta < quanta){
        char* line = getLineFromRam(myCPU->IP);
        myCPU->IP ++;
        errorCode = parse(line);
        free(line);
        if(errorCode != 0){
            return errorCode;
        }
        myCPU->quanta ++;
        memset(myCPU->IR, '\0', 999);
    }
    return errorCode;
}