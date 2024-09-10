#include "list.h"

#ifndef HASH_H
#define HASH_H
typedef struct Hashmap Hashmap;
struct Hashmap {
    int   n_bucket;
    int   n_bucket_used;
    int   n_elems;
    int   load_factor;
    int   growth_factor;

    ListItem  **table;

    void      *(*get_key_field)(void *data);
    int       is_key_string;
    int       key_size;
};

typedef struct HashmapItem HashmapItem;
struct HashmapItem {
    void  *data;
    void  *value;
};

Hashmap       *init_hmap    (void *(*get_key_field)(void *data) 
                              int is_key_string, int key_size, int n_bucket, int load_factor);

HashmapItem   *lookup       (Hashmap *hmap, void *data, int create, void *value);
HashmapItem   *insert       (Hashmap *hmap, void *data, void *value);
HashmapItem   *remove       (Hashmap *hmap, void *data);

void          free_all     (Hashmap *hmap);

#endif
