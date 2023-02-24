#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

typedef struct Block Block;

struct Block {
    char command;
    Block* block;
    Block* next;
};

//  +++[->++<]
//  Code
//      Block
//          char +
//          block NULL
//          next ...
//      Block
//          char +
//          block NULL
//          next ...
//      Block
//          char +
//          block NULL
//          next ...
//      Block
//          char [
//          block ... 
//              Block 
//                  char -
//                  block NULL
//                  next ...
//              Block
//                  char >
//                  block NULL
//                  next ...
//              Block
//                  char +
//                  block NULL
//                  next ...
//              Block
//                  char +
//                  block NULL
//                  next ...
//              Block
//                  char <
//                  block NULL
//                  next NULL
//          next NULL
//
short valid_char(char c) {
    char* ptr = "[]+-,.><";
    while(1) {
        if(*ptr == 0) return 0;
        if(*ptr == c) return 1;
        ptr++;
    }
}

char next_char(FILE* stream, size_t* bytes_read) {
    *bytes_read = 0;
    char c = fgetc(stream);
    if (c != EOF) *bytes_read = 1;
    while(!valid_char(c) && c != EOF){
        c = fgetc(stream);
        *bytes_read += 1;
    }
    return c;
}

Block* parse_block(FILE* stream);
Block* parse_code(FILE* stream);


Block* parse_block(FILE* stream) {
    Block* block = NULL;
    size_t bytes_read;
    char command = next_char(stream, &bytes_read);
    
    if(bytes_read == 0) return block;
    if(command == ']') return block;

    block = malloc(sizeof(Block));
    

    block->command = command;
    block->next = NULL;
    block->block = NULL;


    if(command == '[') {
        block->block = parse_code(stream);
    }

    return block;

}

Block* parse_code(FILE* stream) {
    
    Block* start = parse_block(stream);
    Block* current = start;

    while(current != NULL) {
        current->next = parse_block(stream);
        current = current->next;
    }
    return start;
}

void free_blocks(Block* block) {
    if(block == NULL) return;
    free_blocks(block->block);
    free_blocks(block->next);
    free(block);
}

void tabs(int depth) {
    for(int i=0; i<depth; i++){
        printf("\t");
    }
}

void show_tree_R(Block* block, int depth) {
    if(block == NULL) return;
    tabs(depth);
    printf("Command: %c\n", block->command);
    show_tree_R(block->block, depth+1);
    show_tree_R(block->next,depth);
}

void show_tree(Block* block){
    if(block == NULL) return;
    printf("Command: %c\n", block->command);
    show_tree_R(block->block, 1);
    show_tree(block->next);
}

int main(int argV, char** argC) {
    /*
        No Fle Specificed
    */
    if (argV == 1){
        perror("Error: No File Specificed");
        fprintf(stderr,"%s <file_name>\n",*argC);
        return EXIT_FAILURE;
    }

    FILE* file = fopen(argC[1],"r");
    
    if (file == NULL) {
        fprintf(stderr,"Error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    Block* code = parse_code(file);
    show_tree(code);
    return EXIT_SUCCESS;
}

