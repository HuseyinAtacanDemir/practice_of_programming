#include <stdlib.h>
#include "hash.h"
#include "list.h"

#define DEF_N_BUCKET      32
#define DEF_LOAD_FACTOR   4
#define DEF_GROWTH_FACTOR 2
#define MULTIPLIER        31

Hashmap *init_hmap(void *(*get_key_field)(void *data), int is_key_string,
                   int key_size, int n_bucket, int load_factor)
{
    Hashmap *hmap;

    hmap                = (Hashmap *) malloc(sizeof(Hashmap));
    hmap->n_bucket      = n_bucket > 0 ? n_bucket : DEF_N_HASH;
    hmap->n_bucket_used = 0; 
    hmap->n_elems       = 0;
    hmap->load_factor   = load_factor > 0 ? load_factor : DEF_LOAD_FACTOR;
    hmap->growth_factor = growth_factor > 0 ? growth_factor : DEF_GROWTH_FACTOR;
    hmap->table         = (ListItem **) malloc(sizeof(ListItem*) * (hmap->n_bucket));
    
    hmap->get_key_field = get_key_field;
    hmap->is_key_string = is_key_string;
    hmap->key_size      = key_size > 0 ? key_size : 0; 

    return hmap;
}

HashmapItem *lookup(Hashmap *hmap, void *data, int create, void *value)
{
    ListItem      *li;
    unsigned int  hash;

    hash  = hash(hmap, data);
    li    = (ListItem *) (hmap->table)[hash];

    for ( ; li != NULL; li = li->next)

    if (create >= 0)
        return insert(hmap, data, value);

    return NULL;
}

HashmapItem *insert(Hashmap *hmap, void *data, void *value)
{
    HashMapItem   *hmi;
    ListItem      *li, *p, **table;
    unsigned int  hash;
    int           avg_load;

    avg_load = (hmap->n_elems) / (hmap->n_bucket_used);
    
    if (avg_load  > hmap->load_factor)
        resize(hmap);

    hmi         = (HashmapItem *) calloc(sizeof(HashmapItem));
    hmi->data   = data;
    hmi->value  = value;

    li          = (ListItem *) calloc(sizeof(ListItem));
    li->data    = hmi;
    li->next    = NULL;

    hash        = hash(hmap, data);
    table       = (hmap->table);
    p           = (ListItem *) table[hash];
    li->next    = p;
    table[hash] = li;

    return hmi;
}

HashmapItem * remove()
{
}

unsigned int hash(Hashmap *hmap, void *data)
{
    char *c;
    void *key;
    unsigned int h, i, iteration_limit;

    key = hmap->get_key_field(data);

    if (is_key_string > 0)
        iteration_limit = strlen( (char *) key);
    else
        iteration_limit = hmap->key_size;

    for (i = 0; i < iteration_limit; i++) {
        c = ((char *) key) + i;
        h = MULTIPLIER * h + *c;
    }
    return (h % (hmap->n_bucket));
}

void *resize(Hashmap *hmap)
{
    int           old_n_bucket, i;
    unsigned int  hash;
    HashmapItem   *hmi;
    ListItem      **new_table, **old_table, *p, *next;

    old_n_bucket        = hmap->n_bucket;
    hmap->n_bucket     *= hmap->growth_factor;
    hmap->n_bucket_used = 0;
    
    new_table = (ListItem **) calloc(sizeof(ListItem*) * hmap->n_bucket);
    old_table = (ListItem **) hmap->table;

    for (i = 0; i < old_n_bucket; i++) {
        if ((p = (ListItem *) old_table[i]) == NULL)
            continue;

        for ( ; p != NULL; p = p->next) {
            *hmi = (HashmapItem *) p->data;
            hash = hash(hmap, hmi->data);
            next = NULL;
            
            if (new_table[hash] == NULL)
                hmap->n_buckets_used++;
            else
                next = new_table[hash]->next;

            old_table[i] = p->next;
            p->next = next
            new_table[hash] = p;
        }
    }
    
    free(hmap->table);
    hmap->table = new_table;
}

void free_all(Hashmap *hmap)
{
    ListItem **table;
    
    table = hmap->table;
    free(table);
    free(hmap);
}
