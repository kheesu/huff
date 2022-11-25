#include "huff.h"
extern unsigned int freqtable[MAXCHAR];
extern char *encodetable[MAXCHAR];
extern unsigned long long int number;
extern unsigned long long fsize;
extern unsigned long long csize;

//Finds out distribution of characters in file
void char_dist(FILE* in) {
    size_t size;

    char* buff = calloc(sizeof(char), BUFFSIZE);
    if(buff == NULL) {
        MEMALLOCERR();
    }

    do{
        size = fread(buff, sizeof(char), BUFFSIZE, in);
        fsize += size;

        if(fsize == 0) {
            fprintf(stderr, "shc: Empty file detected: Compression not neccessary\n");
            exit(0);
        }

        if(ferror(in)) {
            FREADERR();
        } 
        for(int i = 0; i < size; i++) {
            if((unsigned char)buff[i] > MAXCHAR) {
                fprintf(stderr, "shc: Non ASCII input detected\n");
                exit(-1);
            }
            freqtable[(unsigned char)buff[i]]++;
        }
    }
    while(size == BUFFSIZE);
    free(buff);
}

//Finds out distribution of characters of file input through stdin
void stdin_char_dist(FILE *temp) {
    size_t size;

    char* buff = calloc(sizeof(char), BUFFSIZE);
    if(buff == NULL) {
        MEMALLOCERR();
    }

    do{
        size = fread(buff, sizeof(char), BUFFSIZE, stdin);
        fwrite(buff, sizeof(char), size, temp);
        fsize += size;

        if(fsize == 0) {
            fprintf(stderr, "shc: Empty input detected: Compression not neccessary\n");
            exit(0);
        }
        for(int i = 0; i < size; i++) {
            if((unsigned char)buff[i] > MAXCHAR) {
                fprintf(stderr, "shc: Non ASCII input detected\n");
                exit(-1);
            }
            freqtable[(unsigned char)buff[i]]++;
        }
    }
    while(size == BUFFSIZE);
    rewind(temp);
    free(buff);
}

//Creates new subtree with both left and right children and returns pointer to parent
node *newsubtree() {

    //Create parent node
    node *p = calloc(1, sizeof(node));
    if(p == NULL) {
        MEMALLOCERR();
    }
    //Create first child and manually set left and right pointer to null
    node *c0 = calloc(1, sizeof(node));
    if(c0 == NULL) {
        MEMALLOCERR();
    }
    c0->left = NULL;
    c0->right = NULL;

    //Create second child and manually set left and right pointer to null
    node *c1 = calloc(1, sizeof(node));
    if(c1 == NULL) {
        MEMALLOCERR();
    }
    c1->left = NULL;
    c1->right = NULL;

    //Assign children to parent
    p->left = c0;
    p->right = c1;

    return p;
}

//Builds huffman tree of arr0 using auxiliary array arr1
void buildht(freqdict *arr0, size_t *size0, node **arr1, size_t *size1) {
    freqdict *fdtemp0, *fdtemp1;
    node **ntemp0, **ntemp1;
    
    while(1) {
        //If arr0 is empty
        if(*size0 == 0) {
            //If arr0 is empty and arr1 has only one element (huffman tree is finished) return
            if(*size1 == 1) return;

            //Otherwise extract two members from arr1 and adds them to a new subtree and reinsert

            ntemp0 = extract(arr1, size1, sizeof(node*), *nodeptrcmp);
            ntemp1 = extract(arr1, size1, sizeof(node*), *nodeptrcmp);

            node *new = calloc(1, sizeof(node));

            new->left = *ntemp0;
            new->right = *ntemp1;
            new->key = new->left->key + new->right->key;

            insert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
        }
        //If arr1 is empty (when going through the loop the first time) extract two members from arr0 and attach to a tree and insert to arr1
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

            insert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
        }
        //If both arr0 and arr1 are populated
        else {
            //Compares the front of both arr0 and arr1 and create new subtree with the smallest member as children
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

                        insert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
                    }
                    else {
                        ntemp0 = extract(arr1, size1, sizeof(node*), *nodeptrcmp);

                        node *new = newsubtree();
                        free(new->right);

                        new->left->ele = fdtemp0->ele;
                        new->left->isleaf = 1;
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
                    new->left->isleaf = 1;
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
                        new->right->isleaf = 1;
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
                    new->right->isleaf = 1;
                    new->right->key = fdtemp0->freq;
                    new->left = *ntemp0;
                    new->key = new->right->key + new->left->key;

                    insert(arr1, size1, sizeof(node*), *nodeptrcmp, &new);
                }
            }
        }
    }
    
}

//Create huffman code recursively by traversing the tree and appending 0 and 1 for left and right child
void encode(node *hufftree, char *code, unsigned int len) {
    //Abort if current code goes over MAXHCODE
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
/*
//Debug function that... checks trees
void treecheck(node *hufftree) {
    if(hufftree == NULL) return;
    if(hufftree->ele) {
        printf("%llu\n",number++);
        
        return;
    }
    treecheck(hufftree->left);
    treecheck(hufftree->right);
}
*/

//Compress file
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
                    csize++;
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

    //Print compression info to stderr
    fprintf(stderr, "\
    Before compression: %lu \n\
    After compression: %lu \n\
    Compression ratio : %.2f\n", fsize, csize, ((double)fsize / (double)csize));
}

//Decompresses file and writes to file *name
//If *name is NULL, write to stdout
void decomp(FILE *in, char *name) {
    FILE *out;

    //Checks if name is null pointer, if so set out to stdout
    if(name) {                              
        errno = 0;
        out = fopen(name, "wb");
        if(errno != 0) {
            FREADERR();
        }
    }
    else {
        //To ensure compatibility with Windows. However, untested if it works because couldn't find proper tar binary for Windows
        #ifdef _WIN32
        _setmode(_fileno(stdout), _O_BINARY);
        #endif
        out = stdout;
    }
    
    //Check if file header is correct
    unsigned char identifier[5] = {0};
    fread(identifier, sizeof(unsigned char), 5, in);
    if(identifier[0] != 7 && identifier[1] != 'S' && identifier[2] != 'H' && identifier[3] != 'C') {
        fprintf(stderr, "shc: Incorrect file format\n");
        exit(-1);
    }

    int maxchar;
    if(identifier[4] == 0) maxchar = 256;
    else maxchar = 128;

    //Read original file size
    fread(&fsize, sizeof(unsigned long long), 1, in);

    //Read frequency table used to generate huffman code
    fread(freqtable, sizeof(unsigned int), maxchar, in);

    //Create huffman tree from table
    node *hufftree = tbltoht();
    size_t size;

    unsigned char* buff = calloc(sizeof(char), BUFFSIZE);
    if(buff == NULL) {
        MEMALLOCERR();
    }

    unsigned char c;
    node *hp = hufftree;

    //Read data in chunks using buffer and decode
    do{
        size = fread(buff, sizeof(char), BUFFSIZE, in);
        if(ferror(in)) {
            FREADERR();
        } 
        for(int i = 0; i < size; i++) {
            c = buff[i];

            //For each byte in file, check the most significant bit by performing AND operation with 0b10000000 (128) and shift left by 1 bit
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

//Create huffman tree from frequency table
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
    
    //Create dictionary of used chararcters
    for(int i = 0, j = 0; i < MAXCHAR; i++) {                           
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

//primary fuction
void drawhufftree(FILE *in)
{
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

    node *tree = tbltoht();

    // should check if we don't exceed this somehow..
    char path[255] = {};
    int inputdepth = 5;
    printf("input the depth of the tree : ");
    scanf("%d", &inputdepth);
    if(inputdepth == -1) inputdepth = __INT32_MAX__;
    
    //initial depth is 0
    treeprint(tree, 0, path, 0, inputdepth);
}

void treeprint(node *hufftree, int depth, char *path, int right, int inputdepth)
{
    // stopping condition
    if (inputdepth == depth){
        return;
    }
    if (hufftree == NULL){
        return;
    }

    // increase spacing
    depth++;

    // start with right node
    treeprint(hufftree->right, depth, path, 1, inputdepth);

    if(depth > 1)
    {
        // set | draw map
        path[depth-2] = 0;

        if(right)
            path[depth-2] = 1;
    }

    if(hufftree->left)
        path[depth-1] = 1;

    // print root after spacing
    printf("\n");

    for(int i=0; i<depth-1; i++)
    {
        if(i == depth-2)
            printf("+");
        else if(path[i])
            printf("|");
        else
            printf(" ");

        for(int j=1; j<SPACE; j++)
            if(i < depth-2)
                printf(" ");
            else
                printf("-");
    }

    if(hufftree->ele > 21 && hufftree->ele < 127)
        printf("%d(%c)\n", hufftree->key, hufftree->ele);
    
    else
        printf("%d<%d>\n", hufftree->key, hufftree->ele);

    // vertical spacers below
    for(int i=0; i<depth; i++)
    {
        if(path[i])
            printf("|");
        else
            printf(" ");

        for(int j=1; j<SPACE; j++)
            printf(" ");
    }

    // go to left node
    treeprint(hufftree->left, depth, path, 0, inputdepth);
}
