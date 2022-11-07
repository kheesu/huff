#include "huff.h"

void swap(void * a, void* b, size_t bytes) {
    char *temp = calloc(sizeof(char), bytes);

    memcpy(temp, a, bytes);
    memcpy(a, b, bytes);
    memcpy(b, temp, bytes);

    free(temp);
}

int freqcmp(const void *pa, const void *pb) {           //Returns 1 if a.freq is greater, returns 0 if b.freq is greater or equal
    const freqdict* a = pa;
    const freqdict* b = pb;
    return a->freq > b->freq ? 1 : 0;
}

int nodecmp(const void *pa, const void *pb) {
    const node *a = pa;
    const node *b = pb;

    return a->key > b->key ? 1 : 0;
}

int nodeptrcmp(const void **pa, const void **pb) {
    const node *a = *pa;
    const node *b = *pb;

    return a->key > b->key ? 1 : 0;
}

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

void ptrheapify(void *arr, size_t size, size_t bytes, int (*cmp)(const void**, const void**), unsigned int i) {
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
        ptrheapify(arr, size, bytes, *cmp, min);
    }
    
}

void minheap(void *arr, size_t size, size_t bytes, int (*cmp)(const void*, const void*)) {
    for(int i = size / 2 - 1; i >= 0; i--) {
        heapify(arr, size, bytes, *cmp, i);
    }
}

void ptrminheap(void *arr, size_t size, size_t bytes, int (*cmp)(const void**, const void**)) {
    for(int i = size / 2 - 1; i >= 0; i--) {
        ptrheapify(arr, size, bytes, *cmp, i);
    }
}

void *extract(void *arr, size_t *size, size_t bytes, int(*cmp)(const void*, const void*)) {
    swap(arr, arr + (bytes * ((*size)---1)), bytes);
    minheap(arr, *size, bytes, *cmp);

    return arr + (bytes * ((*size)));
}

void *ptrextract(void *arr, size_t *size, size_t bytes, int(*cmp)(const void**, const void**)) {
    swap(arr, arr + (bytes * ((*size)---1)), bytes);
    ptrminheap(arr, *size, bytes, *cmp);

    return arr + (bytes * ((*size)));
}

void insert(void *arr, size_t *size, size_t bytes, int(*cmp)(const void*, const void*), void* new) {
    void* endptr = arr + (bytes * *(size));
    *(size) += 1;
    memcpy(endptr, new, bytes);
    minheap(arr, *size, bytes, *cmp);
}

void ptrinsert(void *arr, size_t *size, size_t bytes, int(*cmp)(const void**, const void**), void* new) {
    void* endptr = arr + (bytes * *(size));
    *(size) += 1;
    memcpy(endptr, new, bytes);
    ptrminheap(arr, *size, bytes, *cmp);
}

void hsort(void *arr, size_t size, size_t bytes, int(*cmp)(const void*, const void*)) {
    size_t i = size;
    while(i > 0) {
        extract(arr, &i, bytes, cmp);
    }
}