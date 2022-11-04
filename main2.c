#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifdef ASCIIONLY
#define MAXCHAR 128
#else
#define MAXCHAR 256
#endif

#ifndef BUFFSIZE
#define BUFFSIZE 1024 * 1024 * 4 // 4MiB
#endif

#ifndef MAX_SAMPLE
#define MAX_SAMPLE 1024 * 1024 // 2^20 Characters as max sample size for distribution analysis
#endif

#define MEMALLOCERR() {\
fprintf(stderr, "Failed to allocate memory. Exiting...\n");\
exit(-1);\
}
#define FREADERR() {\
fprintf(stderr, "Failed to read file. Exiting...\n");\
exit(-1);\
}

#define SWAP(a, b, temp) {\
temp = a;\
a = b;\
b = temp;\
}

typedef struct freqdict {
    unsigned int freq;
    char ele;
} freqdict;

//Globals
unsigned int freqtable[MAXCHAR] = {0};

void char_dist(FILE* in) {                   //Finds out distribution of characters in file
    size_t size, totsize = 0;

    char* buff = calloc(sizeof(char), BUFFSIZE);
    if(buff == NULL) {
        MEMALLOCERR();
    }

    do{
        size = fread(buff, sizeof(char), BUFFSIZE, in);

        if(ferror(in)) {
            FREADERR();
        } 
        for(int i = 0; i < size; i++) {
            freqtable[(unsigned char)buff[i]]++;
            if(++totsize > MAX_SAMPLE) break;
        }
    }
    while(size == BUFFSIZE && totsize <= MAX_SAMPLE);
}

void heapify(void *arr, size_t size, int(*cmp)(void const*, void const*)) {

}

int freqcmp(freqdict a, freqdict b) {           //Returns 1 if a.freq is greater, returns 0 if b.freq is greater or equal
    return a.freq > b.freq ? 1 : 0;
}


int main(int argc, char** argv) {
    if(argc == 1) {
        fprintf(stderr, "Provide input file as argument. Exiting...\n");
        exit(0);
    }
    FILE* text;
    errno = 0;
    text = fopen(argv[1], "rb");                //Probably unsafe
    if(errno != 0) {
        FREADERR();
    }

    char_dist(text);

    unsigned int unique_characters = 0;
    for(int i = 0; i < MAXCHAR; i++) {
        if(freqtable[i] != 0) unique_characters++;
    }

    freqdict * dict = calloc(sizeof(freqdict), unique_characters);
    if(dict == NULL) {
        MEMALLOCERR();
    }
    
    for(int i = 0, j = 0; i < MAXCHAR; i++) {                           //Create dictionary of used chararcters
        if(freqtable[i] != 0) {
            dict[j].ele = i;
            dict[j++].freq = freqtable[i];
        }
    }



    for(int i = 0; i < MAXCHAR; i++) printf("%d\n", freqtable[i]);

    fclose(text);
    return 0;
}