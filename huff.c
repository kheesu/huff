#include "huff.h"
extern unsigned int freqtable[MAXCHAR];
extern char *encodetable[MAXCHAR];
extern unsigned long long int number;
extern unsigned long long fsize;

void char_dist(FILE* in) {                   //Finds out distribution of characters in file
    size_t size;

    char* buff = calloc(sizeof(char), BUFFSIZE);
    if(buff == NULL) {
        MEMALLOCERR();
    }

    do{
        size = fread(buff, sizeof(char), BUFFSIZE, in);
        fsize += size;
        if(ferror(in)) {
            FREADERR();
        } 
        for(int i = 0; i < size; i++) {
            if((unsigned char)buff[i] > MAXCHAR) {
                fprintf(stderr, "Non ASCII input detected. Exiting...\n");
                exit(-1);
            }
            freqtable[(unsigned char)buff[i]]++;
        }
    }
    while(size == BUFFSIZE);
    free(buff);
}

node *newsubtree() {
    node *p = calloc(1, sizeof(node));
    if(p == NULL) {
        MEMALLOCERR();
    }
    node *c0 = calloc(1, sizeof(node));
    if(c0 == NULL) {
        MEMALLOCERR();
    }
    c0->left = NULL;
    c0->right = NULL;

    node *c1 = calloc(1, sizeof(node));
    if(c1 == NULL) {
        MEMALLOCERR();
    }
    c1->left = NULL;
    c1->right = NULL;

    p->left = c0;
    p->right = c1;

    return p;
}

void buildht(freqdict *arr0, size_t *size0, node **arr1, size_t *size1) {
    freqdict *fdtemp0, *fdtemp1;
    node **ntemp0, **ntemp1;
    
    while(1) {
        if(*size0 == 0) {
            if(*size1 == 1) return;

            ntemp0 = ptrextract(arr1, size1, sizeof(node*), *nodeptrcmp);
            ntemp1 = ptrextract(arr1, size1, sizeof(node*), *nodeptrcmp);

            node *new = calloc(1, sizeof(node));

            new->left = *ntemp0;
            new->right = *ntemp1;
            new->key = new->left->key + new->right->key;

            ptrinsert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
        }
        else if(*size1 == 0) {
            fdtemp0 = extract(arr0, size0, sizeof(freqdict), *freqcmp);
            fdtemp1 = extract(arr0, size0, sizeof(freqdict), *freqcmp);

            node *new = newsubtree();

            new->left->ele = fdtemp0->ele;
            new->left->isleaf = 1;
            new->left->key = fdtemp0->freq;
            new->right->ele = fdtemp1->ele;
            new->right->isleaf = 1;
            new->right->key = fdtemp1->freq;
            new->key = fdtemp0->freq + fdtemp1->freq;

            ptrinsert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
        }
        else {
            //for(int i = 0; i < *size1; i++) printf("%d %d\n", arr1[i]->key, arr1[i]->ele);
            if(arr0[0].freq <= arr1[0]->key) {
                fdtemp0 = extract(arr0, size0, sizeof(freqdict), *freqcmp);

                if(*size0 > 0) {
                    if(arr0[0].freq <= arr1[0]->key) {
                        fdtemp1 = extract(arr0, size0, sizeof(freqdict), *freqcmp);

                        node *new = newsubtree();

                        new->left->ele = fdtemp0->ele;
                        new->left->isleaf = 1;
                        new->left->key = fdtemp0->freq;
                        new->right->ele = fdtemp1->ele;
                        new->right->isleaf = 1;
                        new->right->key = fdtemp1->freq;
                        new->key = fdtemp0->freq + fdtemp1->freq;

                        ptrinsert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
                    }
                    else {
                        ntemp0 = ptrextract(arr1, size1, sizeof(node*), *nodeptrcmp);

                        node *new = newsubtree();
                        free(new->right);

                        new->left->ele = fdtemp0->ele;
                        new->left->isleaf = 1;
                        new->left->key = fdtemp0->freq;
                        new->right = *ntemp0;
                        new->key = fdtemp0->freq + new->right->key;

                        ptrinsert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
                    }
                }
                else {
                    ntemp0 = ptrextract(arr1, size1, sizeof(node*), *nodeptrcmp);

                    node *new = newsubtree();
                    free(new->right);

                    new->left->ele = fdtemp0->ele;
                    new->left->isleaf = 1;
                    new->left->key = fdtemp0->freq;
                    new->right = *ntemp0;
                    new->key = new->left->key + new->right->key;

                    ptrinsert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
                }
            }
            else {
                ntemp0 = ptrextract(arr1, size1, sizeof(node*), *nodeptrcmp);

                if(*size1 > 0) {
                    if(arr0[0].freq <= arr1[0]->key) {
                        fdtemp0 = extract(arr0, size0, sizeof(freqdict), *freqcmp);

                        node *new = newsubtree();
                        free(new->left);

                        new->right->ele = fdtemp0->ele;
                        new->right->isleaf = 1;
                        new->right->key = fdtemp0->freq;
                        new->left = *ntemp0;
                        new->key = fdtemp0->freq + new->left->key;

                        ptrinsert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
                    }
                    else {
                        ntemp1 = ptrextract(arr1, size1, sizeof(node*), *nodeptrcmp);

                        node *new = calloc(1, sizeof(node));
                        if(new == NULL) {
                            MEMALLOCERR();
                        }

                        new->left = *ntemp0;
                        new->right = *ntemp1;
                        new->key = new->left->key + new->right->key;

                        ptrinsert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
                    }
                }
                else {
                    fdtemp0 = extract(arr0, size0, sizeof(freqdict), *freqcmp);

                    node *new = newsubtree();
                    free(new->left);

                    new->right->ele = fdtemp0->ele;
                    new->right->isleaf = 1;
                    new->right->key = fdtemp0->freq;
                    new->left = *ntemp0;
                    new->key = new->right->key + new->left->key;

                    ptrinsert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
                }
            }
        }
    }
    
}

void encode(node *hufftree, char *code, unsigned int len) {
    if(len > MAXHCODE) {
        fprintf(stderr, "Code exceeds limit. Aborting...\n");
        exit(-1);
    }
    if(hufftree->isleaf) {
        code[len] = '\0';
        encodetable[hufftree->ele] = code;
        return;
    }

    char *new0 = calloc(MAXHCODE, sizeof(char));
    if(new0 == NULL) {
        MEMALLOCERR();
    }
    char *new1 = calloc(MAXHCODE, sizeof(char));
    if(new1 == NULL) {
        MEMALLOCERR();
    }
    
    memcpy(new0, code, MAXHCODE);
    memcpy(new1, code, MAXHCODE);
    free(code);
    new0[len] = '0';
    encode(hufftree->left, new0, len + 1);
    new1[len] = '1';
    encode(hufftree->right, new1, len + 1);
}

void treecheck(node *hufftree) {
    if(hufftree == NULL) return;
    if(hufftree->ele) {
        printf("%lld\n",number++);
        
        return;
    }
    treecheck(hufftree->left);
    treecheck(hufftree->right);
}

void comp(FILE *in, char *name) {
    
    errno = 0;
    FILE *out = fopen(name, "wb");
    if(errno != 0) {
        FREADERR();
    }

    const unsigned char identifier[5] = {7, 'S', 'H', 'C', (unsigned char)MAXCHAR};
    fwrite(identifier, sizeof(unsigned char), 5, out);
    fwrite(&fsize, sizeof(unsigned long long), 1, out);
    fwrite(freqtable, sizeof(int), MAXCHAR, out);
    size_t size;

    char* buff = calloc(sizeof(char), BUFFSIZE);
    if(buff == NULL) {
        MEMALLOCERR();
    }
        
    int bitpos = 0;
    unsigned char outbyte = 0;
    do{
        size = fread(buff, sizeof(char), BUFFSIZE, in);
        if(ferror(in)) {
            FREADERR();
        } 

        for(int i = 0; i < size; i++) {
            char *code = encodetable[(unsigned char)buff[i]];
            for(int j = 0; code[j]; j++) {
                if(code[j] == '1')
                    outbyte = outbyte|(1<<(7 - bitpos));
                bitpos++;
                if(bitpos == 8) {
                    bitpos = 0;
                    fputc(outbyte, out);
                    outbyte = 0;
                }
            }
        }
    }
    while(size == BUFFSIZE);

    if(bitpos){
        fputc(outbyte, out);
    }

    fclose(out);
    free(buff);
}

void decomp(FILE *in, char *name) {
    errno = 0;
    FILE *out = fopen(name, "wb");
    if(errno != 0) {
        FREADERR();
    }
    unsigned char identifier[5] = {0};
    fread(identifier, sizeof(unsigned char), 5, in);
    if(identifier[0] != 7 && identifier[1] != 'S' && identifier[2] != 'H' && identifier[3] != 'C') {
        fprintf(stderr, "shc: Incorrect file format\n");
        exit(-1);
    }

    int maxchar;
    if(identifier[4] == 0) maxchar = 256;
    else maxchar = 128;

    fread(&fsize, sizeof(unsigned long long), 1, in);

    fread(freqtable, sizeof(unsigned int), maxchar, in);

    node *hufftree = tbltoht();
    size_t size;

    unsigned char* buff = calloc(sizeof(char), BUFFSIZE);
    if(buff == NULL) {
        MEMALLOCERR();
    }

    unsigned char c;
    node *hp = hufftree;
    do{
        size = fread(buff, sizeof(char), BUFFSIZE, in);
        if(ferror(in)) {
            FREADERR();
        } 
        for(int i = 0; i < size; i++) {
            c = buff[i];
            for(int j = 0; j < 8; j++) {
                if(c & 128) hp = hp->right;
                else hp = hp->left;
                
                if(hp->isleaf) {
                    fputc(hp->ele, out);
                    if(!--fsize) break;
                    hp = hufftree;
                }
                c = c<<1;
            }
        }
    }
    while(size == BUFFSIZE);
    free(buff);
}

node *tbltoht() {
    unsigned int unique_characters = 0;
    for(int i = 0; i < MAXCHAR; i++) {
        if(freqtable[i] != 0) unique_characters++;
    }

    freqdict * dict = calloc(unique_characters, sizeof(freqdict));
    if(dict == NULL) {
        MEMALLOCERR();
    }
    size_t dictlen = unique_characters;
    
    for(int i = 0, j = 0; i < MAXCHAR; i++) {                           //Create dictionary of used chararcters
        if(freqtable[i] != 0) {
            dict[j].ele = i;
            dict[j++].freq = freqtable[i];
        }
    }

    minheap(dict, dictlen, sizeof(freqdict), *freqcmp);

    node **hufftree = calloc(unique_characters * 2, sizeof(node*));
    if(hufftree == NULL) {
        MEMALLOCERR();
    }
    size_t htsize = 0;
    buildht(dict, &dictlen, hufftree, &htsize);

    return *hufftree;
}