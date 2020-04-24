#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define _DEFAULT_SOURCE

#include "inventory.h"
#include "trimit.h"

inventory_t * inv_init() {
	inventory_t * invp = malloc(sizeof(inventory_t));
	invp->part_list = malloc(sizeof(part_t));
	invp->part_count = 0; 
	invp->assembly_list = malloc(sizeof(assembly_t));
	invp->assembly_count = 0;
	return(invp);
}

void help(){
    printf("Requests:\n");
    printf("    addPart\n");
    printf("    addAssembly ID capacity [x1 n1 [x2 n2 ...]]\n");
    printf("    fulfillOrder [x1 n1 [x2 n2 ...]]\n");
    printf("    stock ID n\n");
    printf("    restock [ID]\n");
    printf("    empty ID\n");
    printf("    inventory [ID]\n");
    printf("    parts\n");
    printf("    help\n");
    printf("    clear\n");
    printf("    quit\n");
}

/*part_t * lookup_part(part_t * pp, char * id){

}

assembly_t * lookup_assembly(assembly_t * ap, char * id){

}

item_t * lookup_item(item_t * ip, char * id){

}*/

void add_part(inventory_t * invp, char * id){
    part_t * temp = invp->part_list;
    for (int i = 1; i<=invp->part_count; i++){
        if(strcmp(id, temp->id) == 0){
            printf("Part already exists\n");
            return;
        }
        temp = temp->next;
    }

    if(strlen(id) <= 11 && id[0] == 'P'){
        part_t * p = malloc(sizeof(part_t));

        strcpy(p->id, id);
        p->next = invp->part_list;
        invp->part_list = p;
        invp->part_count++;
    }else{
        printf("ID is invalid\n");
    }
}
/*
void add_assembly(inventory_t * invp, char * id, int capacity, items_needed_t * items){

}

void add_item(items_needed_t * items, char * id, int quantity){

}

// these are used for sorting purposes
part_t ** to_part_array(int count, part_t * part_list){
    
}

assembly_t ** to_assembly_array(int count, assembly_t * assembly_list){

}

item_t ** to_item_array(int count, item_t * item_list){
    
}

int part_compare(const void *, const void *);
int assembly_compare(const void *, const void *);
int item_compare(const void *, const void *);

void make(inventory_t * invp, char * id, int n, items_needed_t * parts){

}

void get(inventory_t * invp, char * id, int n, items_needed_t * parts){

}

void print_inventory(inventory_t * invp){

}
*/
void print_parts(inventory_t * invp){
    if(invp->part_count > 0){
        printf("Part inventory:\n");
        printf("---------------\n");
        printf("Part ID\n");
        printf("===========\n");
        part_t * temp = invp->part_list;
        for (int i = 1; i<=invp->part_count; i++){
            printf("%s\n", temp->id);
            temp = temp->next;
        }
        
    }else{
        printf("Part inventory:\n");
        printf("---------------\n");
        printf("NO PARTS\n");
    }
}
/*
void print_items_needed(items_needed_t * items){

}

void free_inventory(inventory_t * invp){

}*/

void processRequest(char * command, char ** toToken, inventory_t * invp){
    
    
    (void) invp;
    (void) toToken;

    if (strcmp(command, "addPart") == 0){
        add_part(invp, toToken[1]);
    }else if (strcmp(command, "addAssembly") == 0){
        printf("addAssembly\n");
    }else if (strcmp(command, "fulfillOrder") == 0){
        printf("fulfillOrder\n");
    }else if (strcmp(command, "stock") == 0){
        printf("stock\n");
    }else if (strcmp(command, "restock") == 0){
        printf("restock\n");
    }else if (strcmp(command, "empty") == 0){
        printf("empty\n");
    }else if (strcmp(command, "inventory") == 0){
        printf("inventory\n");
    }else if (strcmp(command, "parts") == 0){
        print_parts(invp);
    }else if (strcmp(command, "help") == 0){
        help();
    }else if (strcmp(command, "clear") == 0){
        printf("clear\n");
    }else if (strcmp(command, "quit") == 0){
        exit(0);
    }else{
        printf("Command does not exists\n");
    }
}

int main(int argc, char *argv[]){

    FILE* fp;

    if(argc == 1){
        fp = stdin;
    }else if(argc == 2){
        fp = fopen( argv[1], "r");
        if(!fp){
            perror(argv[1]);
            return EXIT_FAILURE;
        }
    }else{
        fprintf( stderr, "usage: ./inventory [file]\n");
        return EXIT_FAILURE;
    }
    inventory_t * invp = inv_init();
    char * line = NULL;
    size_t size = 0;
    

    //main loop running while there is a line in either stdin or a file
    while(getline(&line, &size, fp) != -1 ){
        //if the line doesn't starts with whitespace, new line or '#'
        if (line[0] != '\n' && line[0] != '#'){
            //check if there is '#' in the current line and change to \0
            char * ptr;
            ptr = strchr(line, '#');
            if(ptr != NULL){
                *ptr = '\0';
            }

            line = trim(line);
            printf("+ %s\n", line);

            char * token = strtok(line, " ");

            char ** toToken = (char **) calloc(120, sizeof(char *));
            for (size_t i = 0; token != NULL; i++){
                toToken[i] = token;
                token = strtok(NULL, " ");
            }

            char * command = toToken[0];

            processRequest(command, toToken, invp);
        }
    }
}
