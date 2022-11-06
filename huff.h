#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#ifdef ASCIIONLY
#define MAXCHAR 128
#else
#define MAXCHAR 256
#endif

#ifndef BUFFSIZE
#define BUFFSIZE 1024 * 1024 * 4 // 4MiB
#endif

#ifndef MAXHCODE
#define MAXHCODE 32         //Set maximum huffman code to 32 bit
#endif

#define MEMALLOCERR() {\
fprintf(stderr, "Failed to allocate memory. Exiting...\n");\
exit(-1);\
}
#define FREADERR() {\
fprintf(stderr, "Failed to read file. Exiting...\n");\
exit(-1);\
}

typedef struct freqdict {
    unsigned int freq;                      //Frequency of the value
    unsigned char ele;                      //The element i.e. the 8 bit value it is representing

} freqdict;

typedef struct node
{
    unsigned int key;                       //Same as freq
    struct node *left;
    struct node *right;
    unsigned char ele;
}node;

/*
TODO: Use struct to make writing function more consise
Need to rewrite functions that takes size to accept *size
typedef struct heapargs {
    void *arr;
    size_t *size;
    size_t bytes;
    int (*cmp)(const void*, const void*);
}
*/

void swap(void * a, void* b, size_t bytes);
int freqcmp(const void *pa, const void *pb);
int nodecmp(const void *pa, const void *pb);
int nodeptrcmp(const void **pa, const void **pb);
void heapify(void *arr, size_t size, size_t bytes, int (*cmp)(const void*, const void*), unsigned int i);
void minheap(void *arr, size_t size, size_t bytes, int (*cmp)(const void*, const void*));
void *extract(void *arr, size_t *size, size_t bytes, int(*cmp)(const void*, const void*));
void insert(void *arr, size_t *size, size_t bytes, int(*cmp)(const void*, const void*), void* new);
void hsort(void *arr, size_t size, size_t bytes, int(*cmp)(const void*, const void*));

void char_dist(FILE* in);
node *newsubtree();
void buildhufftree(freqdict *arr0, size_t *size0, node *arr1, size_t *size1);
void buildht(freqdict *arr0, size_t *size0, node **arr1, size_t *size1);
void encode(node *hufftree, char *code, unsigned int len);
void treecheck(node *hufftree);
void write(FILE *in, char *name);