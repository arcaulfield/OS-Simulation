#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "shellmemory.h"
#include "shell.h"
#include "kernel.h"
#include "ram.h"
#include "memorymanager.h"

int exitProgramFlag = 0;

//keeps track of the number of files that are open
int inFileCount = 0;

//keeps track of the number of programs that are open
int inProgramCount = 0;
int closeFlag = 0;

//prints a help menu
int help(){
    printf("POSSIBLE COMMANDS FOR THE ALISON SHELL:\nhelp - displays all the commands\n");
    printf("quit - exits/terminates the shell\nset VAR STRING - assigns a value to shell memory\n");
    printf("print VAR - displays the STRING assigned to VAR\n");
    printf("run SCRIPT.TXT - executes the file SCRIPT.TXT\n");
    printf("exec p1 p2  p3 - executes concurrent programs\n");
    return 0;
}

//Terminates program.
//Note that, by assumption, this will still terminate the program if called by a script.
int quit(){
    printf("Bye!\n");
    if(inFileCount  == 0 && inProgramCount == 0){
        clearMemory();
        clearMemManager();
        freeCPU();
        exit(0);
    }
    else{
        if(inFileCount > 0){
            closeFlag = 1;

        }else if(inProgramCount > 0){
            exitProgramFlag = 1;
        }
        return 0;
    }
}

//sets the value stored in variables in memory
int set(char** words){
    char *var = words[1];
    char *val = words[2];;
    setVal(var, val);
    return 0;
}

//prints the values stored in variables in memory
int print(char ** words){
    char *var = words[1];
    char *val = getVal(var);
    printf("%s\n", val);
    return 0;
}

//runs scripts
static int run(char **words){

    inFileCount ++;
    char *filename = words[1];

    //FOR DEBUGGING PURPOSES

    FILE *p;
    if(debug == 1){
        char newfile[100];
        memset(newfile, '\0', 100);
        strcat(newfile, "../");
        strcat(newfile, filename);
        p = fopen(newfile, "rt");
    }
    else {
        p = fopen(filename, "rt");
    }


    int errorCode = 0;
    char line[1000];

    if(p == NULL){
        inFileCount--;
        errorCode = 2;
        return errorCode;
    }

    fgets(line, 999, p);
    while(!feof(p)){
        errorCode = parse(line);
        memset(line, '\0', 999);
        if(errorCode != 0 || closeFlag == 1){
            closeFlag = 0;
            inFileCount--;
            fclose(p);
            return errorCode;
        }
        fgets(line, 999, p);
    }
    fclose(p);
    if(strcmp(line, "") != 0){
        strcat(line, "\n");
        errorCode = parse(line);
        memset(line, '\0', 999);
        if(errorCode != 0 || closeFlag == 1){
            closeFlag = 0;
            inFileCount--;
            fclose(p);
            return errorCode;
        }
    }
    inFileCount--;
    return errorCode;

}

//executes programs simultaneously
int exec(char** words){
    inProgramCount ++;
    filecount = 1;

    int i = 0;
    int errorCode = 0;
    char* filename = words[1];

    FILE *file;
    if(debug == 1){
        char newfile[100];
        memset(newfile, '\0', 100);
        strcat(newfile, "../");
        strcat(newfile, filename);

        file = fopen(newfile, "rt");
    }
    else{
        file = fopen(filename, "rt");
    }


    if(file == NULL){
        errorCode = 2; // file not found error
        clearReadyQueue();
        inProgramCount--;
        return errorCode;
    }

    //NOTE: launcher clauses the file pointer
    int err = launcher(file);

    if(err != 1){
        clearReadyQueue();
        inProgramCount--;
        errorCode = 5;
        return errorCode;
    }

    //parse words[2] to get 2 filenames
    //print an error message if too many files are loaded or if a file name is repeated in the exec call
    int count  = 0;
    while(words[2][i] != '\0'){
        while(words[2][i] == ' '){
            i ++;
        }
        if(words[2][i] == '\0'){
            break;
        }
        count ++;
        //ensure that only 3 programs can be executed at a time
        if(count > 2){
            printf("Error: a maximum of three programs can be executed at a time.\n");
            clearReadyQueue();
            inProgramCount--;
            return errorCode;
        }

        int j = 0;
        while(words[2][i] != ' ' && words[2][i] != '\0'){
            filename[j] = words[2][i];
            i++;
            j++;
        }

        filename[j] = '\0';

        FILE* file2;
        if(debug == 1){
            char newfile2[100];
            memset(newfile2, '\0', 100);
            strcat(newfile2, "../");
            strcat(newfile2, filename);

            file2 = fopen(newfile2, "rt");
        }
        else{
            file2 = fopen(filename, "rt");
        }

        if(file2 == NULL){
            errorCode = 2; // file not found error
            clearReadyQueue();
            inProgramCount--;
            return errorCode;
        }

        //NOTE: launcher closes the file pointer
        err = launcher(file2);

        if(err != 1){
            clearReadyQueue();
            inProgramCount--;
            errorCode = 5;
            return errorCode;
        }
    }

    //call the scheduler
    scheduler();

    inProgramCount--;
    return errorCode;
}

int interpreter(char ** words){
    int errorCode = 0;
    char *argument = words[0];

    if (strcmp(argument, "help") == 0) errorCode = help();
    else if (strcmp(argument, "quit") == 0) errorCode = quit();
    else if (strcmp(argument, "set") == 0) errorCode = set(words);
    else if (strcmp(argument, "print") == 0) errorCode = print(words);
    else if (strcmp(argument, "run") == 0) errorCode = run(words);
    else if (strcmp(argument, "exec") == 0) errorCode = exec(words);
    else errorCode = 1;

    return errorCode;
}
