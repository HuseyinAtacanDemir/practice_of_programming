#ifndef HASH_H
#define HASH_H

#include "list.h"
#include <stdarg.h>

enum { NOT_CREATE, CREATE };

typedef struct Hashmap Hashmap;
struct Hashmap {
    int     n_bucket;
    int     n_bucket_used;
    int     n_elems;
    int     growth_factor;
    int     multiplier;
    double  load_factor;

    ListItem  **table;

    void      *(*get_key_field)(void *data);
    int       key_size;
    int       is_key_string;
};

typedef struct HashmapItem HashmapItem;
struct HashmapItem {
    void  *data;
    void  *value;
};

Hashmap       *init_hmap    (void *(*get_key_field)(void *data), int key_size, 
                              int is_key_string, int n_bucket, 
                              double load_factor, int growth_factor, 
                              int multiplier);

HashmapItem   *find         (Hashmap *hmap, void *data, int create, void *value);
HashmapItem   *insert       (Hashmap *hmap, void *data, void *value);
HashmapItem   *del_hmi      (Hashmap *hmap, void *data);

//void          print_hmap    (Hashmap *hmap);
void          destroy_hmap  (Hashmap *hmap);
void          iterate_hmis  (Hashmap *hmap, void (*fn)(HashmapItem *hmi, int idx, va_list), ...);

#endif
