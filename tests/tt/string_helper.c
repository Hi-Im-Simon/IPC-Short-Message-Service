#include "string_helper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define LINE_SIZE 256
char* readLine(){
    int k = 1;
    char* line;
    line = (char*) malloc(sizeof(char)*LINE_SIZE);
    int i = 0, c;
    while(1){
        c = getchar();
        if(c == EOF || c == '\n') break;

        line[i++] = c;
        if(i == k*LINE_SIZE){
            k++;
            line = (char*) realloc(line, sizeof(char)*k*LINE_SIZE);
        }
    }
    line[i] = '\0';
    return line;   
}

char* substring(const char* str, int begin, int end){
    int len = strlen(str);
    if(len < end) end = len;
    if(len < begin) begin = len;

    int size = end-begin+1;
    char* ret = (char*) malloc(sizeof(char) * size);
    int i = 0;
    while(begin!=end){
        ret[i++] = str[begin++];
    }
    ret[i] = '\0';
    return ret;
}

char** split(const char* str, char separator){
    #define WORDS_SIZE 10
    int k = 1;
    char** words = (char**) malloc(sizeof(char*)*WORDS_SIZE);
    int wordsNumber = 0;

    int len = strlen(str);
    int begin=0, end=0;
    while(end<len){
        if(str[end] == separator){
            words[wordsNumber++] = substring(str, begin, end);
            begin = end+1;
            if(wordsNumber == k*WORDS_SIZE-1){
                k++;
                words = (char**) realloc(words, sizeof(char*)*k*WORDS_SIZE);
            }
        }
        end++;
    }
    words[wordsNumber++] = substring(str, begin, end);
    words[wordsNumber] = (char*) malloc(sizeof(char));
    words[wordsNumber][0] = EOF;
    return words;
}

void cleanBuffer(){
    int c;
    while ( (c = getchar()) != '\n' && c != EOF ) { }
}


