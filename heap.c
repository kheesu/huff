#include "huff.h"

//Swap two generic variable of same type
void swap(void * a, void* b, size_t bytes) {
    char *temp = calloc(sizeof(char), bytes);

    memcpy(temp, a, bytes);
    memcpy(a, b, bytes);
    memcpy(b, temp, bytes);

    free(temp);
}

//Compares two variables of type freqdict
//Returns 1 if a.freq is greater, returns 0 if b.freq is greater or equal
int freqcmp(const void *pa, const void *pb) {           
    const freqdict* a = pa;
    const freqdict* b = pb;
    return a->freq > b->freq ? 1 : 0;
}

//Compares two variables of type node
//Returns 1 if a.key is greater, 0 if b.freq is greater or equal
int nodecmp(const void *pa, const void *pb) {
    const node *a = pa;
    const node *b = pb;

    return a->key > b->key ? 1 : 0;
}

//Compares two pointers that point to *node
int nodeptrcmp(const void *pa, const void *pb) {
    const node *a = *(const node**)pa;
    const node *b = *(const node**)pb;

    return a->key > b->key ? 1 : 0;
}

//Generic heapify function that heapifies array pointed by *arr using comparative function cmp
void heapify(void *arr, size_t size, size_t bytes, int (*cmp)(const void*, const void*), unsigned int i) {
    unsigned int r = 2 * i + 1, l = r++;
    unsigned int min = i;
    
    void* lp = (char*)(l * bytes + arr);
    void* rp = (char*)(r * bytes + arr);
    void* ip = (char*)(i * bytes + arr);
    void* mp = (char*)( min * bytes + arr);

    
    if(l < size && cmp(mp, lp)) {
        min = l;
        mp = (char*)(arr + (min * bytes));
    }
    if(r < size && cmp(mp, rp)) {
        min = r;
        mp = (char*)(arr + (min * bytes));
    }
    
    if(min != i) {
        swap(ip, mp, bytes);
        heapify(arr, size, bytes, *cmp, min);
    }
    
}

//Creates minheap out of array pointed by *arr
void minheap(void *arr, size_t size, size_t bytes, int (*cmp)(const void*, const void*)) {
    for(int i = size / 2 - 1; i >= 0; i--) {
        heapify(arr, size, bytes, *cmp, i);
    }
}

//Swaps position of smallest member with the last member, restores heap and returns smallest member
void *extract(void *arr, size_t *size, size_t bytes, int(*cmp)(const void*, const void*)) {
    swap(arr, arr + (bytes * ((*size)---1)), bytes);
    minheap(arr, *size, bytes, *cmp);

    return arr + (bytes * ((*size)));
}

//Insert member into array pointed by *arr and restore heap
void insert(void *arr, size_t *size, size_t bytes, int(*cmp)(const void*, const void*), void* new) {
    void* endptr = arr + (bytes * *(size));
    *(size) += 1;
    memcpy(endptr, new, bytes);
    minheap(arr, *size, bytes, *cmp);
}

//Generic minimum heapsort, unused in program but might as well
void hsort(void *arr, size_t size, size_t bytes, int(*cmp)(const void*, const void*)) {
    size_t i = size;
    while(i > 0) {
        extract(arr, &i, bytes, cmp);
    }
}