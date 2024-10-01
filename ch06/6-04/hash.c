#include <stdlib.h>
#include <string.h>
#include "hash.h"
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

    hmap                = (Hashmap *) malloc(sizeof(Hashmap));
    hmap->n_bucket      = n_bucket > 0 ? n_bucket : DEF_N_BUCKET;
    hmap->n_bucket_used = 0; 
    hmap->n_elems       = 0;
    hmap->load_factor   = load_factor > 0.0 ? load_factor : DEF_LOAD_FACTOR;
    hmap->growth_factor = growth_factor > 0 ? growth_factor : DEF_GROWTH_FACTOR;
    hmap->multiplier    = multiplier > 0 ? multiplier : DEF_MULTIPLIER;

    hmap->table         = (ListItem **) malloc(sizeof(ListItem*) * (hmap->n_bucket));
    
    hmap->get_key_field = get_key_field;
    hmap->key_size      = key_size > 0 ? key_size : 0; 
    hmap->is_key_string = is_key_string;

    return hmap;
}

HashmapItem *find(Hashmap *hmap, void *data, int create, void *value)
{
    ListItem      *li;
    unsigned int  h;
    
    h   = hash(hmap, data);
    for (li = hmap->table[h]; li != NULL; li = li->next)
        if (compare_keys(hmap, data, ((HashmapItem *) li->data)->data) == 0)
            return li->data;
                
    if (create > 0)
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

    hmi         = (HashmapItem *) calloc(1, sizeof(HashmapItem));
    hmi->data   = data;
    hmi->value  = value;

    li        = (ListItem *) calloc(1, sizeof(ListItem));
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
          return hmi;
    }
    return NULL; 
}

//void print_hmap(Hashmap *hmap)
//{
//    int i, char_index[4];
//    unsigned int h;
//    ListItem *li;
//    char *printed_line;
//    
//    char horz_separator[15] = "--------------";
//    char arrow[5] = "--->";
//    char vert_separator[2] = "|";
//    
//    for (i = 0; i < hmap->n_bucket; i++) {
//        printed_line = (char *) malloc(sizeof(char) * (PRINT_WIDTH+1) * 4);
//
//        char_index = {0, 0, 0, 0};
//        strncpy(printed_line, horz_separator, PRINT_WIDTH);
//        char_index[0] += strlen(horz_separator);
//
//        for (li = hmap->table[i]; li != NULL; li = li->next) {
//            h = hash(hmap, ((HashmapItem *)li->data)->data);
//            
//        }
//
//        printf(printed_line);
//    }
//
//}

// TODO clean all HMAP related DSTs
void free_map(Hashmap *hmap)
{
    ListItem **table;
    
    table = hmap->table;
    free(table);
    free(hmap);
}

int compare_keys(Hashmap *hmap, void *data1, void *data2)
{
    int i;
    char *key1, *key2;

    key1 = (char *) hmap->get_key_field(data1);
    key2 = (char *) hmap->get_key_field(data2);

    for (i = 0; i < hmap->key_size; i++, key1++, key2++)
        if (*key1 > *key2)
          return 1;
        else if (*key1 < *key2)
          return -1;
    
    return 0;
}

// Hashmap hash: create a hash based on a key member of an unknown structure
//                based on the keysize (bytes) provided by the user, and the 
//                get_key_field fn provided by the user
unsigned int hash(Hashmap *hmap, void *data)
{
    char *key;
    unsigned int h, i, iteration_limit;

    key = (char *) hmap->get_key_field(data);

    iteration_limit = hmap->key_size;
    if (hmap->is_key_string > 0)
        iteration_limit = strlen(key);
    
    h = 0;
    for (i = 0; i < iteration_limit; i++)
        h = hmap->multiplier * h + *(key+i);
    
    return (h % (hmap->n_bucket));
}

// Hashmap resize: resizes existing hmap by multiplying bucket count by growth factor
void resize(Hashmap *hmap)
{
    int           n_bucket_old, i;
    unsigned int  h;
    HashmapItem   *hmi;
    ListItem      *li, *next_new, **tbl_new, **tbl_old;

    n_bucket_old          = hmap->n_bucket;
    hmap->n_bucket_used   = 0;
    hmap->n_bucket        = n_bucket_old * hmap->growth_factor;
    
    tbl_new = (ListItem **) calloc(hmap->n_bucket, sizeof(ListItem*));
    tbl_old = (ListItem **) hmap->table;
    hmap->table = tbl_new;

    for (i = 0; i < n_bucket_old; i++) {                    // go through mapped buckets
        for (li = tbl_old[i]; li != NULL; li = tbl_old[i]) {  // for each mapped bucket, pluck first from old table, substitute next in
            hmi = (HashmapItem *) li->data;
            h   = hash(hmap, hmi->data);

            if ((next_new = tbl_new[h]) == NULL)            // if tbl_new corresponding bucket empty
                hmap->n_bucket_used++;

            tbl_old[i]  = li->next;
            li->next    = next_new;
            tbl_new[h]  = li;
        }
    }
    
    free(tbl_old);
}

