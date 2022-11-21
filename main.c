#include "huff.h"

//Globals
unsigned int freqtable[MAXCHAR] = {0};                  //Table that stores frequency of each characters
char *encodetable[MAXCHAR] = {0};                       //Table that stores the huffman code for each character in string form
//unsigned long long int number = 0;            
//Used with the debug function treecheck()
//Disabled for release
unsigned long long fsize = 0;                           //Measured file size of original file before compression

int main(int argc, char** argv) {
//Could use argp.h to get better arguments parsing but decided to minimize external library use
    if(argc == 1) {
        fprintf(stderr, "shc: Provide input file as argument\n");
        exit(0);
    }

    if(!strcmp("-c", argv[1])) {
        if(argc < 3) {
            fprintf(stderr, "shc: Provide input file as argument\n");
            exit(0);
        }
        else if(argc > 5) {
            fprintf(stderr, "shc: Too many arguments\n");
            exit(0);
        }

        FILE* text;
        errno = 0;
        text = fopen(argv[2], "rb");
        if(errno != 0) {
            FREADERR();
        }

        char_dist(text);
        fclose(text);

        char *code = calloc(MAXHCODE, sizeof(char));     
        if(code == NULL) {
            MEMALLOCERR();
        }               
        unsigned int len = 0;
        encode(tbltoht(), code, len);

        if(argc == 3) {
            char *outname = calloc(strlen(argv[2]) + 5, sizeof(char));
            if(outname == NULL)  {
                MEMALLOCERR();
            }
            strcpy(outname, argv[2]);
            strcat(outname, ".shc");

            errno = 0;
            text = fopen(argv[2], "rb");
            if(errno != 0) {
                FREADERR();
            }

            comp(text, outname);
            free(outname);
        }
        else {
            errno = 0;
            text = fopen(argv[2], "rb");
            if(errno != 0) {
                FREADERR();
            }
            comp(text, argv[3]);
        }

        fclose(text);

    }
    else if(!strcmp("-d", argv[1])) {
        if(argc < 3) {
            printf("shc: Provide input file as argument\n");
            exit(0);
        }
        else if(argc > 4) {
            printf("shc: Too many arguments\n");
            exit(0);
        }

        errno = 0;
        FILE *comp = fopen(argv[2], "rb");
        if(errno != 0) {
            FREADERR();
        }
        
        if(argc == 4 && !strcmp("-stdout", argv[3])) {                      //Taking advantage of short circuiting to prevent a segfault when accessing argv[3]
            decomp(comp, NULL);
        }

        else if(argc == 3) {
            char *fname = calloc(strlen(argv[2]) + 1, sizeof(char));
            if(fname == NULL) {
                MEMALLOCERR();
            }
            strncpy(fname, argv[2], strlen(argv[2]));
            
            int lastc = 0;
            for(int i = 0; fname[i]; i++) {
                if(fname[i] == '.') lastc = i;
            }
            fname[lastc] = '\0';
            if(lastc == 0 || strcmp(fname + lastc + 1, "shc") != 0) {
                printf("shc: Unsupported file extension\n");
                exit(0);
            }
            
            decomp(comp, fname);
            free(fname);
        }
        else {
            decomp(comp, argv[3]); 
        }
        fclose(comp);
    }
    else if(!strcmp("-", argv[1])) {
        if(argc != 3) {
            fprintf(stderr, "shc: Incorrect arguments\n");
            exit(-1);
        }

        FILE *tempfile = tmpfile();
        if(tempfile == NULL) {
            fprintf(stderr, "shc: Failed to create temporary file\n");
            exit(-1);
        }

        stdin_char_dist(tempfile);

        char *code = calloc(MAXHCODE, sizeof(char));     
        if(code == NULL) {
            MEMALLOCERR();
        }               
        unsigned int len = 0;
        encode(tbltoht(), code, len);

        comp(tempfile, argv[2]);
    }
    else {
        printf("shc: Invalid operation: \"%s\"\n", argv[1]);
    }

    return 0;
}
