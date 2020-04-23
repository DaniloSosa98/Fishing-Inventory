#ifndef INVENTORY_H
#define INVENTORY_H

extern char * trim(char *);
extern int getline(char **, size_t *, FILE *);

#define ID_MAX // **FIXME**

// part
struct part {
    char id[ID_MAX+1];        // ID_MAX plus NUL
    struct part * next; // the next part in the list of parts
};

// assembly
struct assembly {
    char id[ID_MAX+1];
    int capacity;
    int on_hand;
    struct items_needed * items; // parts/sub-assemblies needed for this ID
    struct assembly * next;      // the next assembly in the inventory list
};

// inventory item (a part or an assembly)
struct item {
    char id[ID_MAX+1];           // ID_MAX plus NUL
    int quantity;
    struct item * next; // next item in the part/assembly list
};

// the inventory (parts and assemblies)
struct inventory {
    struct part * part_list;         // list of parts by ID
    int part_count;                  // number of distinct parts
    struct assembly * assembly_list; // list of assemblies by ID
    int assembly_count;              // number of distinct assemblies
};

// parts/sub-assemblies needed to make required assemblies
struct items_needed {
    struct item * item_list;
    int item_count;
};

struct req {
    char * req_string;
    void (*req_fn)(struct inventory *);
};

typedef struct inventory inventory_t;
typedef struct items_needed items_needed_t;
typedef struct item item_t;
typedef struct part part_t;
typedef struct assembly assembly_t;

part_t * lookup_part(part_t * pp, char * id);
assembly_t * lookup_assembly(assembly_t * ap, char * id);
item_t * lookup_item(item_t * ip, char * id);

void add_part(inventory_t * invp, char * id);
void add_assembly(inventory_t * invp,
                  char * id,
                  int capacity,
                  items_needed_t * items);
void add_item(items_needed_t * items, char * id, int quantity);

// these are used for sorting purposes
part_t ** to_part_array(int count, part_t * part_list);
assembly_t ** to_assembly_array(int count, assembly_t * assembly_list);
item_t ** to_item_array(int count, item_t * item_list);
int part_compare(const void *, const void *);
int assembly_compare(const void *, const void *);
int item_compare(const void *, const void *);

void make(inventory_t * invp, char * id, int n, items_needed_t * parts);
void get(inventory_t * invp, char * id, int n, items_needed_t * parts);
void print_inventory(inventory_t * invp);
void print_parts(inventory_t * invp);
void print_items_needed(items_needed_t * items);
void free_inventory(inventory_t * invp);

#endif // INVENTORY_H
