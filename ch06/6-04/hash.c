#include "hash.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "eprintf.h"
#include "list.h"

void      resize          (Hashmap *hmap);
int       compare_keys    (Hashmap *hmap, void *data1, void *data2);
unsigned  hash            (Hashmap *hmap, void *data);

Hashmap *init_hmap(void *(*get_key)(Hashmap *hmap, void *data), int keysize, 
                    int is_key_str, int n_bucket, int x_load, int x_growth, 
                    int multiplier)
{
    Hashmap *hmap;

    hmap            = (Hashmap *)emalloc(sizeof(Hashmap));

    hmap->n_bucket  = n_bucket > 0 ? n_bucket : DEF_N_BUCKET;
    hmap->table     = (ListItem **)emalloc(sizeof(ListItem*)*(hmap->n_bucket));
    hmap->n_used    = 0; 
    hmap->n_items   = 0;

    hmap->x_load      = x_load > 0.0    ? x_load      : DEF_LOAD_FACTOR;
    hmap->x_growth    = x_growth > 0    ? x_growth    : DEF_GROWTH_FACTOR;
    hmap->multiplier  = multiplier > 0  ? multiplier  : DEF_MULTIPLIER;

    hmap->get_key     = get_key;
    hmap->keysize     = keysize > 0 ? keysize : 0; 
    hmap->is_key_str  = is_key_str;

    return hmap;
}

Item *find(Hashmap *hmap, void *data, int create, void *value)
{
    ListItem  *li;
    unsigned  h;
    
    //  calculate the hash for the data, iterate over all mapped items in the
    //  bucket with the corresponding match, compare the data to the data
    //  of the items found in the bucket
    h = hash(hmap, data);
    for (li = hmap->table[h]; li != NULL; li = li->next)
        if (compare_keys(hmap, data, ((Item *)li->data)->data) == 0)
            return li->data;
                
    //  if not found and a create option is selected, insert a new mapped item
    if (create)
        return insert(hmap, data, value);

    return NULL;
}

Item *insert(Hashmap *hmap, void *data, void *value)
{
    Item      *item;
    ListItem  *li, *next;
    unsigned  h;
    int       load;

    //  calculate current load by dividing n_items by n_buckets, 
    //  multiply resulting percentage by 100, cast it to in
    load = (int) ( ((double) hmap->n_items / (double) hmap->n_bucket) * 100.0);
    
    if (load > hmap->x_load)
        resize(hmap);

    //  allocate memory for the new mapped item and the wrapper ListItem
    item        = (Item *)emalloc(sizeof(Item));
    item->data  = data;
    item->value = value;

    li        = (ListItem *)emalloc(sizeof(ListItem));
    li->data  = item;
    li->next  = NULL;

    //  calculate hash for the data of the item and insert it to corresponding
    //  bucket of hmap using the ListItem wrapper
    h               = hash(hmap, data);
    next            = (ListItem *) hmap->table[h];
    li->next        = next;
    hmap->table[h]  = li;

    //  n_items ot hmap increases in any case, n_used only increases if
    //  ListItem wrapper was inserted into a previously empty bucket
    hmap->n_items++;
    if (next == NULL)
        hmap->n_used++;

    return item;
}

void del_hmap_item(Hashmap *hmap, void *data)
{
    ListItem  *li, *prev, *next;
    unsigned  h;

    // iterate over the items in the bucket corresponding to the hash
    prev = NULL;
    h = hash(hmap, data);
    for (li = hmap->table[h]; li != NULL; prev = li, li = li->next) {
        if (compare_keys(hmap, data, ((Item *) li->data)->data) == 0) {
            next = li->next;
            li->next = NULL;

            if (prev)           // found somewhere other than bucket list head
                prev->next = next;
            else if (next)      // found in bucket list head, other items exist
                hmap->table[h] = next;
            else {              // found in bucket list head, was the only item
                hmap->table[h] = NULL;
                hmap->n_used--;
            }

            hmap->n_items--;
            
            free(li->data);
            li->data = NULL;

            free(li);
            li = NULL;

            return;
        }
    } 
    return;
}


// Hashmap compare_keys: compare the key members of data1 and data2. Key member
//                        of the unknown struct that the void pointers point to
//                        is dereferenced/extracted using the get_key
//                        fn provided by the user to the Hashmap during init
int compare_keys(Hashmap *hmap, void *data1, void *data2)
{
    int   i;
    char  *key1, *key2;

    key1 = (char *) hmap->get_key(hmap, data1);
    key2 = (char *) hmap->get_key(hmap, data2);

    if (hmap->is_key_str)
        return (hmap->keysize == 0 ? strcmp(key1, key2) 
                              : memcmp(key1, key2, hmap->keysize));

    for (i = 0; i < hmap->keysize; i++, key1++, key2++)
        if (*key1 > *key2)
          return 1;
        else if (*key1 < *key2)
          return -1;
    
    return 0;
}

// Hashmap hash: create a hash based on a key member of an unknown structure
//                based on the keysize (bytes) provided by the user, and the 
//                get_key fn provided by the user, so effectively,
//                we hash everything as if they are chars. If the key is a str,
//                we hash assuming the dereferenced key will be a str.
//                is this approach stupid? 
unsigned int hash(Hashmap *hmap, void *data)
{
    char *key;
    unsigned int h, i, iteration_limit;

    // keys are treated as char arrays of size keysize...
    key = (char *) hmap->get_key(hmap, data);
    iteration_limit = hmap->keysize;

    // ..unless the key itself is a string, 
    //in which case we can assume \0 termination
    if (hmap->is_key_str > 0)
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
    Item   *item;
    ListItem      *li, *new_next_li, **tbl_new, **tbl_old;

    // store old hmap n_bucket, update hmap metadata to be a bigger empty hmap
    n_bucket_old    = hmap->n_bucket;
    hmap->n_bucket  = n_bucket_old * hmap->x_growth;
    hmap->n_used    = 0;
    
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
            item = (Item *) li->data;
            h   = hash(hmap, item->data);
            
            // if tbl_new[i] (aka i'th bucket) is empty
            if ((new_next_li = tbl_new[h]) == NULL) 
                hmap->n_used++;

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

void iterate_map(Hashmap *hmap, void (*fn)(Item*, int, va_list), ...)
{
    va_list args;

    va_start(args, fn);
    int j = 0;
    for (int i = 0 ; i < hmap->n_bucket; i++)
        for (ListItem *li = hmap->table[i]; li ; li = li->next) {
            fn((Item *) li->data, j, args);
            j++;
        }
    va_end(args);
}

