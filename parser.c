#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

typedef struct Block Block;

struct Block {
    char command;
    Block* block;
    Block* next;
    u_int16_t repeat;
};

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
    
    if(feof(stream)) return block;
    if(command == ']') return block;

    block = malloc(sizeof(Block));
    

    block->command = command;
    block->next = NULL;
    block->block = NULL;
    block->repeat = 1;


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

void free_block(Block* block){
    if(block == NULL) return;
    free(block);
}

void free_blocks(Block* block) {
    if(block == NULL) return;
    free_blocks(block->block);
    free_blocks(block->next);
    free_block(block);
}

void tabs(int depth) {
    for(int i=0; i<depth; i++){
        printf("\t");
    }
}

int repeatable(char c){
    char* ptr = "+-><";
    while(*ptr != 0){
        if (*ptr == c) return 1;
        ptr++;
    }
    return 0;
}

void show_tree_R(Block* block, int depth) {
    if(block == NULL) return;
    tabs(depth);
    printf("Command: %c\n", block->command);
    tabs(depth);
    printf(" count: %u\n",block->repeat); 
    show_tree_R(block->block, depth+1);
    show_tree_R(block->next,depth);
}

void show_tree(Block* block){
    if(block == NULL) return;
    printf("Command: %c\n", block->command);
    if(repeatable(block->command) && block->repeat > 1) printf(" count: %u\n",block->repeat); 
    show_tree_R(block->block, 1);
    show_tree(block->next);
}

void simplify(Block* code) {
    while (code != NULL){
        simplify(code->block);
        if(repeatable(code->command)){
            Block* ptr = code->next;
            while(ptr != NULL && ptr->command == code->command){
                code->next = ptr->next;
                code->repeat++;
                Block* tmp = ptr;
                ptr = ptr->next;
                free_block(tmp);
            }
        }
        code = code->next;
    }
}

#ifndef __TRANSLATOR__


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

#endif
