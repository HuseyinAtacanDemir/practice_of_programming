#include "list.h"

#ifndef HASH_H
#define HASH_H
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

Hashmap       *init_hmap    (void *(*get_key_field)(void *data), int key_size, int is_key_string,
                              int n_bucket, double load_factor, int growth_factor, int multiplier);

HashmapItem   *find         (Hashmap *hmap, void *data, int create, void *value);
HashmapItem   *insert       (Hashmap *hmap, void *data, void *value);
HashmapItem   *del_hmi      (Hashmap *hmap, void *data);

//void          print_hmap    (Hashmap *hmap);
void          free_map      (Hashmap *hmap);

#endif
