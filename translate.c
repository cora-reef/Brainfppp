#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


#define __TRANSLATOR__
#include "parser.c"




void write_header(FILE* out_file) {   
    fputs("#define CELL_COUNT 30000\n",out_file);
    fputs("#include <stdio.h>\n",out_file);
    fputs("#include <stdlib.h>\n\n",out_file);
    fputs("int main(void) {\n",out_file);
    fputs("\tchar cells[CELL_COUNT];\n",out_file);
    fputs("\tfor(size_t i=0; i<CELL_COUNT; i++) cells[i] = 0;\n",out_file);
    fputs("\tchar* curr = cells;\n\n",out_file);    
}
 
void ftabs(size_t depth,FILE* stream){
    for(size_t i=0; i<depth; i++){
        fputc('\t',stream);
    }
}

void write(Block* block, FILE* out_file, size_t depth){
    if (block == NULL) {
        ftabs(depth-1, out_file);
        fputs("}\n",out_file);
        return;
    }
    ftabs(depth,out_file);
    switch (block->command) {
        case '+':
            fputs("*curr += 1;\n",out_file);
            break;
        case '-':
            fputs("*curr -= 1;\n",out_file);
            break;
        case '[':
            fputs("while(*curr != 0) {\n",out_file);
            break;
        case '>':
            fputs("curr = cells + ((curr - cells + 1) \% CELL_COUNT);\n",out_file);
            break;
        case '<':
            fputs("curr = cells + ((curr - cells + CELL_COUNT -1) \% CELL_COUNT);\n",out_file);
            break;
        case '.':
            fputs("putc(*curr,stdout);\n",out_file);
            break;
        case ',':
            fputs("*curr = getc(stdin);\n",out_file);
            break;
        case 0:
            fputs("return EXIT_SUCCESS;\n}",out_file);
            return;
        default:
            perror("ERROR: invalid command");
    }
    if(block->block != NULL) write(block->block,out_file,depth+1);
    write(block->next,out_file,depth);
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

    FILE* in_file = fopen(argC[1],"r");
    
    if (in_file == NULL) {
        fprintf(stderr,"Error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    Block* code = parse_code(in_file);
    fclose(in_file);
    
    char* out_file_name = "out.c";
    if (argV == 3) {
        out_file_name = argC[2];
    }
    FILE* out_file = fopen(out_file_name,"w");

    write_header(out_file);
    write(code, out_file,1);

    return EXIT_SUCCESS;
}


