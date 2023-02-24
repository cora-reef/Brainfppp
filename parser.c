#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct Block Block;

typedef struct Link Link;


struct Link {
    Link* next;
    Block* block;
};

struct Block {
    char character;
    Block* block;
};

//  Code
//      Block
//          Block
//              Char
//              Char
//              Char
//          Char
//      Block
//          Char
//          Char
//          Char

//Block* parse_bloch(file)


int main(int argV, char** argC) {
    /*
        No Fle Specificed
    */
    if (argV == 1){
        perror("Error: No File Specificed");
        fprintf(stderr,"%s <file_name>\n",*argC);
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}

