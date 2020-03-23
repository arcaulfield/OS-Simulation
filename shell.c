#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"
#include "shellmemory.h"

int words_num;
int word_length;

//takes a string of words (userInput) and parses through them to create parsedString
int parse(char* userInput){
    int errorCode = 0;

    char **parsedString = (char **) malloc(sizeof(char *) * words_num);
    for (int i = 0; i < words_num; i++) {
        parsedString[i] = (char *) malloc(sizeof(char) * word_length);
        parsedString[i][0] = '\0';
    }

    int letterNum = 0;
    int wordNum = 0;
    int i = 0;
    int start_string_flag = 0;

    if(userInput[0] == '\n'){
        return errorCode;
    }

    if(userInput[0] == '\0'){
        return errorCode;
    }

    while(userInput[i] != '\0'){
        if(userInput[i] == ' ' && wordNum < 2){
            if(i > 0 && userInput[i - 1] != ' '){
                parsedString[wordNum][letterNum] = '\0';
                wordNum ++;
                if(wordNum == 2 && userInput[i + 1] != ' '){
                    start_string_flag = 1;
                }
                letterNum = 0;
            }
        }
        else if(userInput[i] == ' ' && wordNum >= 2 && start_string_flag == 0){
            if(userInput[i + 1] != ' '){
                start_string_flag = 1;
            }
        }
        else{
            parsedString[wordNum][letterNum] = userInput[i];
            letterNum++;
        }
        i ++;

    }

    if(parsedString[wordNum][letterNum - 1] == '\n'){
        parsedString[wordNum][letterNum - 1] = '\0';
    } else {
        printf("Warning: Your input string is too long. Input can be at most %d characters. ", word_length);
        printf("Your input will be read %d characters at a time.\n", word_length);
    }

    if(parsedString[wordNum][letterNum - 2] == '\r'){
        parsedString[wordNum][letterNum - 2] = '\0';
    }

    errorCode = interpreter(parsedString);

    //free the memory used for the parsed strings
    for (int i = 0; i < words_num; i++) {
        free(parsedString[i]);
    }
    free(parsedString);

    return errorCode;
}


int shellUI() {
    word_length = 100;
    words_num = 3;
    char prompt[100] = "$";
    char userInput[word_length];
    int errorCode = 0;

    //instantiate the shell memory
    initMemory();

    printf("Welcome to the Alison shell!\n");
    printf("Shell Version 2.0 Updated February 2020\n");

    while(1) {
        //ensure that the user regains access to the keyboard after using a testfile
        if(feof(stdin)){
            (void) freopen("/dev/tty", "r", stdin);
            continue;
        }

        printf("%s", prompt);


        memset(userInput, '\0', (word_length));
        fgets(userInput, word_length, stdin);

        //parse through the user input and save the result in parsedString
        errorCode = parse(userInput);

        if(errorCode == -1) exit(99);
            //error code for unknown commands
        else if(errorCode == 1){
            printf("Unknown command.\n");
        }
            //error code for scripts that can't be found
        else if(errorCode == 2){
            printf("Script not found.\n");
        }
        //error code for not being enough space to load programs into ram
        else if(errorCode == 5){
            printf("Error: programs can be at most 40 lines long.\n");
        }

    }

    return 0;
}
