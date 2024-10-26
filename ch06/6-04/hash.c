#include "hash.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "eprintf.h"
#include "list.h"

#define DEF_N_BUCKET      32
#define DEF_LOAD_FACTOR   0.75
#define DEF_GROWTH_FACTOR 2
#define DEF_MULTIPLIER    33
#define PRINT_WIDTH       120

int             compare_keys    (Hashmap *hmap, void *data1, void *data2);
unsigned int    hash            (Hashmap *hmap, void *data);
void            resize          (Hashmap *hmap);

Hashmap *init_hmap(void *(*get_key_field)(void *data), int key_size, 
                    int is_key_string, int n_bucket, double load_factor, 
                    int growth_factor, int multiplier)
{
    Hashmap *hmap;

    hmap                = (Hashmap *) emalloc(sizeof(Hashmap));
    hmap->n_bucket      = n_bucket > 0 ? n_bucket : DEF_N_BUCKET;
    hmap->n_bucket_used = 0; 
    hmap->table         = (ListItem **) emalloc(
                                        sizeof(ListItem*) * (hmap->n_bucket));
    hmap->n_elems       = 0;
    hmap->load_factor   = load_factor > 0.0 ? load_factor 
                                            : DEF_LOAD_FACTOR;
    hmap->growth_factor = growth_factor > 0 ? growth_factor 
                                            : DEF_GROWTH_FACTOR;
    hmap->multiplier    = multiplier > 0 ? multiplier : DEF_MULTIPLIER;
    hmap->get_key_field = get_key_field;
    hmap->key_size      = key_size > 0 ? key_size : 0; 
    hmap->is_key_string = is_key_string;

    return hmap;
}

HashmapItem *find(Hashmap *hmap, void *data, int create, void *value)
{
    ListItem      *li;
    unsigned int  h;
    
    h = hash(hmap, data);
    for (li = hmap->table[h]; li != NULL; li = li->next)
        if (compare_keys(hmap, data, ((HashmapItem *) li->data)->data) == 0)
            return li->data;
                
    if (create)
        return insert(hmap, data, value);

    return NULL;
}

HashmapItem *insert(Hashmap *hmap, void *data, void *value)
{
    HashmapItem   *hmi;
    ListItem      *li, *next;
    unsigned int  h;
    double        avg_load;

    avg_load = ((double) hmap->n_elems) / ((double) hmap->n_bucket);
    
    if (avg_load > hmap->load_factor)
        resize(hmap);

    hmi         = (HashmapItem *) emalloc(sizeof(HashmapItem));
    hmi->data   = data;
    hmi->value  = value;

    li        = (ListItem *) emalloc(sizeof(ListItem));
    li->data  = hmi;
    li->next  = NULL;

    h               = hash(hmap, data);
    next            = (ListItem *) hmap->table[h];
    li->next        = next;
    hmap->table[h]  = li;

    hmap->n_elems++;
    if (next == NULL)
        hmap->n_bucket_used++;

    return hmi;
}

HashmapItem *del_hmi(Hashmap *hmap, void *data)
{
    unsigned int  h;
    HashmapItem   *hmi;
    ListItem      *li, *prev, *next;

    prev = NULL;
    h = hash(hmap, data);
    for (li = hmap->table[h]; li != NULL; li = li->next) {
        if (compare_keys(hmap, data, ((HashmapItem *) li->data)->data) != 0) {
            prev = li;
            continue;
        }

        hmi   = li->data;
        next  = li->next;

        if (prev == NULL) {
            hmap->table[h] = NULL;
            hmap->n_bucket_used--;
        } else {
            prev->next  = next;
        }     
        
        hmap->n_elems--; 
        free(li);
        li = NULL;
        return hmi;
    }
    return NULL; 
}


// Hashmap compare_keys: compare the key members of data1 and data2. Key member
//                        of the unknown struct that the void pointers point to
//                        is dereferenced/extracted using the get_key_field
//                        fn provided by the user to the Hashmap during init
int compare_keys(Hashmap *hmap, void *data1, void *data2)
{
    int i;
    char *key1, *key2;

    key1 = (char *) hmap->get_key_field(data1);
    key2 = (char *) hmap->get_key_field(data2);

    if (hmap->is_key_string)
        return (hmap->key_size ? strcmp(key1, key2) 
                              : memcmp(key1, key2, hmap->key_size));

    for (i = 0; i < hmap->key_size; i++, key1++, key2++)
        if (*key1 > *key2)
          return 1;
        else if (*key1 < *key2)
          return -1;
    
    return 0;
}

// Hashmap hash: create a hash based on a key member of an unknown structure
//                based on the keysize (bytes) provided by the user, and the 
//                get_key_field fn provided by the user, so effectively,
//                we hash everything as if they are chars. If the key is a str,
//                we hash assuming the dereferenced key will be a str.
//                is this approach stupid? 
unsigned int hash(Hashmap *hmap, void *data)
{
    char *key;
    unsigned int h, i, iteration_limit;

    // keys are treated as char arrays of size key_size...
    key = (char *) hmap->get_key_field(data);
    iteration_limit = hmap->key_size;

    // ..unless the key itself is a string, 
    //in which case we can assume \0 termination
    if (hmap->is_key_string > 0)
        iteration_limit = strlen(key);
   
    // hashing as if we are hasing a string, regardless of underlying key type 
    // is this stupid?
    h = 0;
    for (i = 0; i < iteration_limit; i++)
        h = hmap->multiplier * h + *(key+i);
    
    return (h % (hmap->n_bucket));
}

// Hashmap resize: resizes existing hmap by multiplying bucket count by growth 
//                  factor. Move all data from old hmap->table to a newly
//                  allocated bigger hmap->table. Free the old table.
void resize(Hashmap *hmap)
{
    int           n_bucket_old, i;
    unsigned int  h;
    HashmapItem   *hmi;
    ListItem      *li, *new_next_li, **tbl_new, **tbl_old;

    // store old hmap n_bucket, update hmap metadata to be a bigger empty hmap
    n_bucket_old          = hmap->n_bucket;
    hmap->n_bucket        = n_bucket_old * hmap->growth_factor;
    hmap->n_bucket_used   = 0;
    
    // store old hmap table, alloc new bigger hmap table, update hmap metadata
    tbl_old     = (ListItem **) hmap->table;
    tbl_new     = (ListItem **) emalloc(sizeof(ListItem*) * hmap->n_bucket);
    hmap->table = tbl_new;
    
    // loop over buckets of tbl_old. If bucket was mapped: 
    //    pluck that bucket's head li; 
    //    substitute li->next to be tbl_old's i'th bucket's head li;
    //    hash plucked li's data; 
    //    and set li the head of appropriate bucket in tbl_new
    for (i = 0; i < n_bucket_old; i++) {
        li = tbl_old[i];
        while (li) {  // i.e. if tbl_old bukcet i is not empty
            hmi = (HashmapItem *) li->data;
            h   = hash(hmap, hmi->data);
            
            // if tbl_new[i] (aka i'th bucket) is empty
            if ((new_next_li = tbl_new[h]) == NULL) 
                hmap->n_bucket_used++;

            // update tbl_old's i'th bucket's head li, 
            // set li to be tbl_new's h'th bucket's head with new_next_li
            tbl_old[i]  = li->next;
            li->next    = new_next_li;
            tbl_new[h]  = li;

            // li = what used to be li->next in tbl_old, for iteration
            li = tbl_old[i];
        }
    }
    
    free(tbl_old);
    tbl_old = NULL;
}

// TODO clean all HMAP related DSTs
void destroy_hmap(Hashmap *hmap)
{
    /*
    pseudocode: 
        loop over all mapped buckets (non null buckets)
            iterate over li, li->next ... till NULL
                for each li, free the HashMapItem
                free the li
        free (ListItem **) hmap->table
        free (Hashmap *) hmap
    */
}

void iterate_hmis(Hashmap *hmap, void (*fn)(HashmapItem *hmi, int idx, va_list args), ...)
{
    va_list args;

    va_start(args, fn);
    int j = 0;
    for (int i = 0 ; i < hmap->n_bucket; i++)
        for (ListItem *li = hmap->table[i]; li ; li = li->next) {
            fn((HashmapItem *) li->data, j, args);
            j++;
        }
    va_end(args);
}

