/*



*/

#ifndef HASH_H
#define HASH_H

#include "list.h"
#include <stdarg.h>

#define DEF_N_BUCKET      32
#define DEF_LOAD_FACTOR   75
#define DEF_GROWTH_FACTOR 2
#define DEF_MULTIPLIER    33
#define PRINT_WIDTH       120

enum { NOT_CREATE, CREATE };

typedef struct Hashmap Hashmap;
struct Hashmap {
    ListItem  **table;

    int   n_bucket;   // current number of buckets in hmap
    int   n_used;     // current number of buckets used in hmap
    int   n_items;    // current # of Item's mapped in hmap
    int   x_growth;   // n_bucket *= x_growth, once x_load is reached
    int   x_load;     // trigger a resize, eg 75 -> resize when %75 full
    int   multiplier; // hash multiplier, default 33

    // return a void * to the "key" field of the mapped data structure, 
    // this will be the value that will be hashed 
    void  *(*get_key)(void *data);  
    int   keysize; // size, in bytes, of key returned by get_key
    int   is_key_string;  // true if "key" is a null term str 
};

typedef struct Item Item;
struct Item {
    void  *data;
    void  *value;
};

// initialize the hmap, get_key and keysize are not optional
Hashmap *init_hmap  (void *(*get_key)(void *data), int keysize, int is_key_str, 
                      int n_bucket, int x_load, int x_growth, int multiplier);

Item *find        (Hashmap *hmap, void *data, int create, void *value);
Item *insert      (Hashmap *hmap, void *data, void *value);
void *del_item    (Hashmap *hmap, void *data);
void destroy_hmap (Hashmap *hmap);
void iterate_map  (Hashmap *hmap, void (*fn)(Item *, int idx, va_list), ...);

#endif
