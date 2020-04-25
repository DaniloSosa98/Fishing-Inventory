#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define _DEFAULT_SOURCE

#include "inventory.h"
#include "trimit.h"

int tokCount = 0;

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

part_t * lookup_part(part_t * pp, char * id){
    while(pp != NULL){
        if(strcmp(pp->id, id) == 0){
            return pp;
        }
        pp = pp->next;
    }
    return NULL;
}

assembly_t * lookup_assembly(assembly_t * ap, char * id){
    while(ap != NULL){
        if(strcmp(ap->id, id) == 0){
            return ap;
        }
        ap = ap->next;
    }
    return NULL;
}

item_t * lookup_item(item_t * ip, char * id){
    while(ip != NULL){
        if(strcmp(ip->id, id) == 0){
            return ip;
        }
        ip = ip->next;
    }
    return NULL;
}

void add_part(inventory_t * invp, char * id){
    part_t * temp = invp->part_list;
    for (int i = 1; i<=invp->part_count; i++){
        if(strcmp(id, temp->id) == 0){
            printf("!!! %s: duplicate part ID\n", id);
            return;
        }
        temp = temp->next;
    }

    part_t * p = malloc(sizeof(part_t));

    strcpy(p->id, id);
    p->next = invp->part_list;
    invp->part_list = p;
    invp->part_count++;
}

void add_assembly(inventory_t * invp, char * id, int capacity, items_needed_t * items){
    assembly_t * temp = invp->assembly_list;
    for (int i = 1; i<=invp->assembly_count; i++){
        if(strcmp(id, temp->id) == 0){
            printf("Assembly already exists\n");
            return;
        }
        temp = temp->next;
    }

    if(strlen(id) <= 11 && id[0] == 'A' && capacity >=0 ){
        assembly_t * a = malloc(sizeof(assembly_t));

        strcpy(a->id, id);
        a->capacity = capacity;
        a->on_hand = 0;
        a->items = items;
        a->next = invp->assembly_list;
        invp->assembly_list = a;
        invp->assembly_count++;
        a->items = items;
    }else{
        printf("ID is invalid\n");
    }
}

void add_item(items_needed_t * items, char * id, int quantity){
    item_t * item = lookup_item(items->item_list, id);
    if(item!=NULL){
        item->quantity += quantity;
    }else{
        item_t * i = malloc(sizeof(item_t));
        strcpy(i->id, id);
        i->quantity = quantity;
        i->next = items->item_list;
        items->item_list = i;
        items->item_count++;
    }
}

/*
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
*/
void make(inventory_t * invp, char * id, int n, items_needed_t * parts){
    if (n <= 0){
        return;
    }
    assembly_t * a = lookup_assembly(invp->assembly_list, id);
    if(a == NULL){
        printf("Assembly does not exists\n");
        return;
    }
    printf(">>> make %d units of assembly %s\n", n, a->id);
    item_t * item = a->items->item_list;
    for (int i = 0; i < a->items->item_count; i++){
        if(lookup_part(invp->part_list, item->id) != NULL){
            add_item(parts, item->id, n*item->quantity);
        }else{
            get(invp, item->id, n-item->quantity, parts);
        }
        item = item->next;
    }
    
}

void get(inventory_t * invp, char * id, int n, items_needed_t * parts){
    if (n <= 0){
        return;
    }
    assembly_t * a = lookup_assembly(invp->assembly_list, id);
    if(a == NULL){
        printf("Assembly does not exists\n");
        return;
    }
    if ((a->on_hand - n) >= 0){
        a->on_hand -= n;
    }else{
        make(invp, id, n - a->on_hand, parts);
        a->on_hand = 0;
    }
}

void print_inventory(inventory_t * invp){
    printf("Assembly inventory:\n");
    printf("-------------------\n");

    if(invp->assembly_count == 0){
        printf("EMPTY INVENTORY\n");
        return;
    }
    printf("Assembly ID Capacity On Hand\n");
    printf("=========== ======== =======\n");
    assembly_t * a = invp->assembly_list;
    for (int i = 0; i < invp->assembly_count; i++){
        int onHand = a->on_hand;
        int capacity = a->capacity;
        if(onHand < ((float)capacity/2)){
            printf("%-11s %8d %7d*\n", a->id, capacity, onHand);
        }else{
            printf("%-11s %8d %7d\n", a->id, capacity, onHand);
        }
        a = a->next;
    }
    
}

void print_inventoryID(inventory_t * invp, char * id){
    assembly_t * a = lookup_assembly(invp->assembly_list, id);
    printf("Assembly ID:  %s\n", a->id);
    printf("bin capacity: %d\n", a->capacity);
    printf("on-hand:      %d\n", a->on_hand);
    if(a->items->item_count > 0){
        printf("Parts list:\n");
        printf("-----------\n");
        printf("Part ID     quantity\n");
        printf("=========== ========\n");
        item_t * item = a->items->item_list;
        for (int i = 0; i < a->items->item_count; i++){
            printf("%-11s %8d\n", item->id, item->quantity);
            item = item->next;
        }
        
        
    }
}

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

void print_items_needed(items_needed_t * items){
    if(items->item_count == 0){
        return;
    }
    printf("Parts needed:\n");
    printf("-------------\n");
    printf("Part ID     quantity\n");
    printf("=========== ========\n");
    item_t * item = items->item_list;
    for (int i = 0; i < items->item_count; i++){
        printf("%-11s %8d\n", item->id, item->quantity);
        item = item->next;
    }
    
}

void free_inventory(inventory_t * invp){
    //free parts list
    free(invp->part_list);
    //free assembly list and its parts list
    assembly_t * a = invp->assembly_list;
    for (int i = 0; i < invp->assembly_count; i++){
        free(a->items);
        a = a->next;
    }
    free(invp->assembly_list);
    free(invp);
    invp = inv_init();
}

void empty(inventory_t * invp, char * id){
    assembly_t * ap = lookup_assembly(invp->assembly_list, id);
    if(ap == NULL){
        printf("!!! %s: assembly ID is not in the inventory\n", id);
        return;
    }

    ap->on_hand = 0;
}

void processRequest(char * command, char ** toToken, inventory_t * invp){

    if (strcmp(command, "addPart") == 0){
        char * id = toToken[1];
        if(strlen(id) > 11){
            printf("!!! %s: part ID too long\n", id);
            return;
        }
        if(id[0] != 'P'){
            printf("!!! %s: part ID must start with 'P'\n", id);
            return;
        }
        add_part(invp, toToken[1]);

    }else if (strcmp(command, "addAssembly") == 0){
        char * id = toToken[1];
        int capacity = atoi(toToken[2]);
        if(capacity<0){
            printf("Capacity can't be negative\n");
            return;
        }
        items_needed_t * items = malloc(sizeof(items_needed_t));
        items->item_list = malloc(sizeof(item_t));
        items->item_count = 0;
        for (int i = 3; i < tokCount; i+=2){
            char * id = toToken[i];
            int quantity = atoi(toToken[i+1]);
            if((lookup_assembly(invp->assembly_list, id) != NULL || lookup_part(invp->part_list, id) != NULL ) && quantity>0){
                add_item(items, id, quantity);
            }else{
                printf("A parts needed condition was violated\n");
                return;
            }
        }

        add_assembly(invp, id, capacity, items);

    }else if (strcmp(command, "fulfillOrder") == 0){
        
        items_needed_t * in = malloc(sizeof(items_needed_t));
        in->item_list = malloc(sizeof(item_t));
        in->item_count = 0;
        
        for (int i = 1; i < tokCount; i+=2){
            char * id = toToken[i];
            int quantity = atoi(toToken[i+1]);
            if(lookup_assembly(invp->assembly_list, id) == NULL){
                printf("!!! %s: assembly ID is not in the inventory -- order canceled\n", id);
                return;
            }
            if(quantity<=0){
                printf("!!! %d: illegal order quantity for ID %s -- order canceled\n", quantity, id);
                return;
            }
            add_item(in, id, quantity);
        }
        items_needed_t * parts = malloc(sizeof(items_needed_t));
        parts->item_list = malloc(sizeof(item_t));
        parts->item_count = 0;
        item_t * current = in->item_list;
        for (int i = 0; i < in->item_count; i++){
            char * id = current->id;
            int quantity = current->quantity;
            get(invp, id, quantity, parts);
            current = current->next;
        }
        free(in);
        print_items_needed(parts);
        free(parts);

    }else if (strcmp(command, "stock") == 0){
        char* id = toToken[1];
        int n = atoi(toToken[2]);

        if(n<=0){
            printf("!!! %d: illegal quantity for ID %s\n", n, id);
            return;
        }

        assembly_t * a = lookup_assembly(invp->assembly_list, id);
        if(a==NULL){
            printf("!!! %s: assembly ID is not in the inventory\n", id);
            return;
        }

        if(a->on_hand == a->capacity || a->capacity == 0){
            printf("!!! %s: assembly is at max capacity\n", id);
            return;
        }

        items_needed_t * parts = malloc(sizeof(items_needed_t));
        parts->item_list = malloc(sizeof(item_t));
        parts->item_count = 0;
        
        if (a->on_hand + n > a->capacity){
            n = a->capacity - a->on_hand;
            make(invp, id, n, parts);
            a->on_hand =+ n;
        }
        else{
            make(invp, id, n, parts);
            a->on_hand =+ n;
        }

        print_items_needed(parts);
        free(parts);
        

    }else if (strcmp(command, "restock") == 0){
        
        items_needed_t * parts = malloc(sizeof(items_needed_t));
        parts->item_list = malloc(sizeof(item_t));
        parts->item_count = 0;

        if(tokCount>1){
            char * id = toToken[1];
            assembly_t * as = lookup_assembly(invp->assembly_list, id);
            if(as == NULL){
                printf("!!! %s: assembly ID is not in the inventory\n", id);
                return;
            }
            if(as->on_hand < ((float)as->capacity/2)){
                    int n = as->capacity - as->on_hand;
                    printf(">>> restocking assembly %s with %d items\n", as->id, n);
                    make(invp, as->id, n, parts);
                    as->on_hand = as->capacity;
            }

        }else{
            assembly_t * a = invp->assembly_list;
            for (int i = 0; i < invp->assembly_count; i++){
                if(a->on_hand < ((float)a->capacity/2)){
                    int n = a->capacity - a->on_hand;
                    printf(">>> restocking assembly %s with %d items\n", a->id, n);
                    make(invp, a->id, n, parts);
                    a->on_hand = a->capacity;
                }
                a = a->next;
            }
            
        }

        print_items_needed(parts);
        free(parts);

    }else if (strcmp(command, "empty") == 0){
        char * id = toToken[1];
        if(id[0] != 'A'){
            printf("!!! %s: ID not an assembly\n", id);
            return;
        }
        empty(invp, toToken[1]);
    }else if (strcmp(command, "inventory") == 0){
        if(tokCount>1){
            if(lookup_assembly(invp->assembly_list, toToken[1]) != NULL){
                print_inventoryID(invp, toToken[1]);
            }else{
                printf("Assembly does no exists\n");
            }
            
        }else{
            print_inventory(invp);
        }
    }else if (strcmp(command, "parts") == 0){
        print_parts(invp);
    }else if (strcmp(command, "help") == 0){
        help();
    }else if (strcmp(command, "clear") == 0){
        free_inventory(invp);
    }else if (strcmp(command, "quit") == 0){
        exit(0);
    }else{
        printf("!!! %s: unknown command\n", command);
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
            tokCount = 0;
            for (size_t i = 0; token != NULL; i++){
                toToken[i] = token;
                token = strtok(NULL, " ");
                tokCount++;
            }
            char * command = toToken[0];

            processRequest(command, toToken, invp);
        }
    }
}
