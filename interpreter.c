#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "shellmemory.h"
#include "shell.h"
#include "kernel.h"
#include "ram.h"

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
    char newfile[100];
    memset(newfile, '\0', 100);
    strcat(newfile, "../");
    strcat(newfile, filename);

    int errorCode = 0;
    char line[1000];

    FILE *p = fopen(newfile, "rt");
    if(p == NULL){
        inFileCount--;
        errorCode = 2;
        return errorCode;
    }

    fgets(line, 999, p);
    while(!feof(p)){
        errorCode = parse(line);
        if(errorCode != 0 || closeFlag == 1){
            closeFlag = 0;
            inFileCount--;
            fclose(p);
            return errorCode;
        }
        fgets(line, 999, p);
    }
    fclose(p);
    inFileCount--;
    return errorCode;

}

//executes programs simultaneously
int exec(char** words){
    inProgramCount ++;
    //keep track of filenames to watch for duplicates
    char* filenames[3];
    for(int i = 0; i < 3; i ++ ){
        filenames[i] = NULL;
    }

    int i = 0;
    int errorCode = 0;
    char* filename = words[1];
    errorCode = myinit(filename);
    if(errorCode != 0){
        clearReadyQueue();
        inProgramCount--;
        return errorCode;
    }
    filenames[0] = strdup(filename);

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
        for(int k = 0; k < count; k ++){
            //print an error message if the file has already been loaded and terminate exec
            if(strcmp(filenames[k], filename) == 0){
                printf("Error: Script %s already loaded\n", filename);
                clearReadyQueue();
                inProgramCount--;
                for(int i = 0; i < 3; i++){
                    if(filenames[i] != NULL){
                        free(filenames[i]);
                    }
                }
                return errorCode;
            }
        }
        filenames[count] = strdup(filename);
        //initialise the file
        errorCode = myinit(filename);
        if(errorCode != 0){
            clearReadyQueue();
            inProgramCount--;
            for(int i = 0; i < 3; i++){
                if(filenames[i] != NULL){
                    free(filenames[i]);
                }
            }
            return errorCode;
        }
    }

    for(int i = 0; i < 3; i++){
        if(filenames[i] != NULL){
            free(filenames[i]);
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
