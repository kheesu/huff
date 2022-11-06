#include "huff.h"

//Globals
unsigned int freqtable[MAXCHAR] = {0};
char *encodetable[MAXCHAR] = {0};
unsigned long long int number = 0;

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
    fclose(text);
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
    //printf("%d\n", htsize);
    buildht(dict, &dictlen, hufftree, &htsize);
    //printf("%d\n", htsize);

    //printf("%d %d %d\n",hufftree[0]->key, hufftree[0]->left->key, hufftree[0]->right->key);
    //treecheck(*hufftree);
    //return 0;
    char *code = calloc(MAXHCODE, sizeof(char));                    
    unsigned int len = 0;
    encode(*hufftree, code, len);
    //printf("mark\n");

    for(int i = 0; i < MAXCHAR; i++) printf("%d %s\n", i, encodetable[i]);

    text = fopen(argv[1], "rb");                //Probably unsafe
    write(text, "compressed");
    fclose(text);
    return 0;
}

void char_dist(FILE* in) {                   //Finds out distribution of characters in file
    size_t size;

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

void buildhufftree(freqdict *arr0, size_t *size0, node *arr1, size_t *size1) {
    while(1) {
        if(*size0 > 0 && *size1 > 0) {
            node *p = newsubtree();
            unsigned int newfreq = 0;

            freqdict *temp0;
            node *temp1;

            if(arr0[0].freq <= arr1[0].key) {
                temp0 = extract(arr0, size0, sizeof(freqdict), *freqcmp);
                p->left->key = temp0->freq;
                p->left->ele = temp0->ele;
                newfreq += temp0->freq;
            }
            else {
                free(p->left);
                temp1 = extract(arr1, size1, sizeof(node), *nodecmp);

                p->left = temp1;
                newfreq += temp1->key;
            }

            if(*size0 > 0 && *size1 > 0) {
                if(arr0[0].freq <= arr1[0].key) {
                    temp0 = extract(arr0, size0, sizeof(freqdict), *freqcmp);
                    p->right->key = temp0->freq;
                    p->right->ele = temp0->ele;
                    newfreq += temp0->freq;
                }
                else {
                    free(p->right);
                    temp1 = extract(arr1, size1, sizeof(node), *nodecmp);

                    p->right = temp1;
                    newfreq += temp1->key;
                }
            }
            else {
                free(p->right);
                temp1 = extract(arr1, size1, sizeof(node), *nodecmp);

                p->right = temp1;
                newfreq += temp1->key;
            }

            p->key = newfreq;
            insert(arr1, size1, sizeof(node), *nodecmp, p);
            
        }
        else if(*size0 == 0) {
            if(*size1 == 1) break;

            node *p = calloc(1, sizeof(node));
            if(p == NULL) {
                MEMALLOCERR();
            }
            
            unsigned int newfreq = 0;

            node* temp = extract(arr1, size1, sizeof(node), *nodecmp);
            p->left = temp;
            newfreq += temp->key;

            temp = extract(arr1, size1, sizeof(node), *nodecmp);
            p->right = temp;
            newfreq += temp->key;

            p->key = newfreq;
            
            insert(arr1, size1, sizeof(node), *nodecmp, p);
        }
        else if(*size1 == 0) {
            node *p = newsubtree();
            
            unsigned int newfreq = 0;

            freqdict *temp = extract(arr0, size0, sizeof(freqdict), *freqcmp);
            p->left->key = temp->freq;
            p->left->ele = temp->ele;
            newfreq += temp->freq;

            temp = extract(arr0, size0, sizeof(freqdict), *freqcmp);
            p->right->key = temp->freq;
            p->right->ele = temp->ele;
            newfreq += temp->freq;

            p->key = newfreq;

            insert(arr1, size1, sizeof(node), *nodecmp, p);
        }
    }
}

void buildht(freqdict *arr0, size_t *size0, node **arr1, size_t *size1) {
    freqdict *fdtemp0, *fdtemp1;
    node **ntemp0, **ntemp1;
    
    while(1) {
        if(*size0 == 0) {
            if(*size1 == 1) return;

            ntemp0 = extract(arr1, size1, sizeof(node*), *nodeptrcmp);
            ntemp1 = extract(arr1, size1, sizeof(node*), *nodeptrcmp);

            node *new = calloc(1, sizeof(node));

            new->left = *ntemp0;
            new->right = *ntemp1;
            new->key = new->left->key + new->right->key;

            insert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
        }
        else if(*size1 == 0) {
            fdtemp0 = extract(arr0, size0, sizeof(freqdict), *freqcmp);
            fdtemp1 = extract(arr0, size0, sizeof(freqdict), *freqcmp);

            node *new = newsubtree();

            new->left->ele = fdtemp0->ele;
            new->left->key = fdtemp0->freq;
            new->right->ele = fdtemp1->ele;
            new->right->key = fdtemp1->freq;
            new->key = fdtemp0->freq + fdtemp1->freq;

            insert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
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
                        new->left->key = fdtemp0->freq;
                        new->right->ele = fdtemp1->ele;
                        new->right->key = fdtemp1->freq;
                        new->key = fdtemp0->freq + fdtemp1->freq;

                        insert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
                    }
                    else {
                        ntemp0 = extract(arr1, size1, sizeof(node*), *nodeptrcmp);

                        node *new = newsubtree();
                        free(new->right);

                        new->left->ele = fdtemp0->ele;
                        new->left->key = fdtemp0->freq;
                        new->right = *ntemp0;
                        new->key = fdtemp0->freq + new->right->key;

                        insert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
                    }
                }
                else {
                    ntemp0 = extract(arr1, size1, sizeof(node*), *nodeptrcmp);

                    node *new = newsubtree();
                    free(new->right);

                    new->left->ele = fdtemp0->ele;
                    new->left->key = fdtemp0->freq;
                    new->right = *ntemp0;
                    new->key = new->left->key + new->right->key;

                    insert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
                }
            }
            else {
                ntemp0 = extract(arr1, size1, sizeof(node*), *nodeptrcmp);

                if(*size1 > 0) {
                    if(arr0[0].freq <= arr1[0]->key) {
                        fdtemp0 = extract(arr0, size0, sizeof(freqdict), *freqcmp);

                        node *new = newsubtree();
                        free(new->left);

                        new->right->ele = fdtemp0->ele;
                        new->right->key = fdtemp0->freq;
                        new->left = *ntemp0;
                        new->key = fdtemp0->freq + new->left->key;

                        insert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
                    }
                    else {
                        ntemp1 = extract(arr1, size1, sizeof(node*), *nodeptrcmp);

                        node *new = calloc(1, sizeof(node));
                        if(new == NULL) {
                            MEMALLOCERR();
                        }

                        new->left = *ntemp0;
                        new->right = *ntemp1;
                        new->key = new->left->key + new->right->key;

                        insert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
                    }
                }
                else {
                    fdtemp0 = extract(arr0, size0, sizeof(freqdict), *freqcmp);

                    node *new = newsubtree();
                    free(new->left);

                    new->right->ele = fdtemp0->ele;
                    new->right->key = fdtemp0->freq;
                    new->left = *ntemp0;
                    new->key = new->right->key + new->left->key;

                    insert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
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
    if(hufftree->ele) {
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
        printf("%d\n",number++);
        
        return;
    }
    treecheck(hufftree->left);
    treecheck(hufftree->right);
}

void write(FILE *in, char *name) {
    errno = 0;
    FILE *out = fopen(name, "wb");
    if(errno != 0) {
        FREADERR();
    }

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
            for(int i = 0; code[i]; i++) {
                if(code[i] == '1')
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
    if(outbyte)
        fputc(outbyte, out);
    
    fclose(out);
    free(buff);
}