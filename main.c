#include "huff.h"

//Globals
unsigned int freqtable[MAXCHAR] = {0};                  //Table that stores frequency of each characters
char *encodetable[MAXCHAR] = {0};                       //Table that stores the huffman code for each character in string form

//Used with the debug function treecheck()
//Disabled for release
//unsigned long long int number = 0;            

//Measured file size of original file before compression
unsigned long long fsize = 0;                           
//Variable to store compressed data size to display compression ratio
unsigned long long csize = 0; 

int main(int argc, char** argv) {
//Could use argp.h to get better arguments parsing but decided to minimize external library use

//Exit if no arguments are provieded
    if(argc == 1) {             
        fprintf(stderr, "shc: Provide input file as argument\n");
        exit(0);
    }

    //If the compression flag -c is provided
    if(!strcmp("-c", argv[1])) {
        //If too few arguments are provided
        if(argc < 3) {
            fprintf(stderr, "shc: Provide input file as argument\n");
            exit(0);
        }
        //If too many arguments are provided
        else if(argc > 5) {
            fprintf(stderr, "shc: Too many arguments\n");
            exit(0);
        }

        FILE* text;
        errno = 0;
        //Open filename provided as argument in binary mode
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
        node *tree = tbltoht();
        encode(tree, code, len);

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
        cleantree(tree);
    }
    //If decompression flag -d is provided
    else if(!strcmp("-d", argv[1])) {
        //If too few arguments are provided
        if(argc < 3) {
            printf("shc: Provide input file as argument\n");
            exit(0);
        }
        //If too many arguments are provided
        else if(argc > 4) {
            printf("shc: Too many arguments\n");
            exit(0);
        }

        errno = 0;
        FILE *comp = fopen(argv[2], "rb");
        if(errno != 0) {
            FREADERR();
        }

        //If stdout flag -stdout is given write to stdout
        //Taking advantage of short circuiting to prevent a segfault when accessing argv[3]
        if(argc == 4 && !strcmp("-stdout", argv[3])) {                      
            decomp(comp, NULL);
        }
        //If no extract filename is given save to file with the same name with .shc file extension removed
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
        //Write the decompressed file to provided filename
        else {
            decomp(comp, argv[3]); 
        }
        fclose(comp);
    }
    //If stdin compression flag - is provided
    else if(!strcmp("-", argv[1])) {
        //If incorrect number of arguments are given
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
        node *tree = tbltoht();
        encode(tree, code, len);
        comp(tempfile, argv[2]);
        cleantree(tree);
    }
    //If tree printing flag -f is provided
    else if(!strcmp("-t", argv[1])) {
        //If incorrect number of arguments are provided
        if(argc != 3) {
            fprintf(stderr, "shc: Incorrect argument format\n");
            exit(-1);
        }
        
        errno = 0;
        FILE *input = fopen(argv[2], "rb");
        if(errno != 0) {
            FREADERR();
        }

        drawhufftree(input);

        fclose(input);
    }
    else if(!strcmp("-q", argv[1])) {

        long temp = -1;
        char *err;
        errno = 0;

        if(argc < 3 || argc > 4) {
            fprintf(stderr, "shc: Incorrect argument format\n");
            exit(-1);
        }

        if(argc == 3) {
            fprintf(stdout, "Enter query in decimal : ");

            char input[256];
            if(fgets(input, 256, stdin) != NULL) {
                
                temp = strtol(input, &err, 10);
                if(errno == ERANGE) {
                    fprintf(stderr, "shc: Invalid input\n");
                    exit(-1);
                }
            }
        }
        else if(argc == 4) {
            temp = strtol(argv[3], &err, 10);
            if(temp < 0 || temp > 256 || errno == ERANGE) {
                fprintf(stderr, "shc: Invalid input\n");
                exit(-1);
            }
        }

        errno = 0;
        FILE *input = fopen(argv[2], "rb");
        if(errno != 0) {
            FREADERR();
        }

        unsigned char identifier[5] = {0};
        fread(identifier, sizeof(unsigned char), 5, input);
        if(identifier[0] != 7 && identifier[1] != 'S' && identifier[2] != 'H' && identifier[3] != 'C') {
            fprintf(stderr, "shc: Incorrect file format\n");
            exit(-1);
        }

        int maxchar;
        if(identifier[4] == 0) maxchar = 256;
        else maxchar = 128;

        fread(&fsize, sizeof(unsigned long long), 1, input);

        fread(freqtable, sizeof(unsigned int), maxchar, input);

        node *tree = tbltoht();

        char *code = calloc(MAXHCODE, sizeof(char));     
        if(code == NULL) {
            MEMALLOCERR();
        }

        if(!searchhuffcode(tree, code, 0, (unsigned char)temp)) {
            fprintf(stdout, "shc: No result found\n");
        }

        cleantree(tree);
    }
    //Anything else is invalid operation
    else {
        fprintf(stderr, "shc: Invalid operation: \"%s\"\n", argv[1]);
    }

    return 0;
}
