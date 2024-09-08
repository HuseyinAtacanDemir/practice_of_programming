#ifndef LIST_H
#define LIST_H

typedef struct ListItem ListItem;
struct ListItem {
    void      *data;
    ListItem  *next;
};

ListItem    *new_item       (void *data);

ListItem    *add_front      (ListItem *listp, ListItem *newp);
ListItem    *add_end        (ListItem *listp, ListItem *newp);
ListItem    *insert_after   (ListItem *listp, ListItem *newp, 
                              void *after_data, int (*cmp)(void*, void*));
ListItem    *insert_before  (ListItem *listp, ListItem *newp, 
                              void *before_data, int (*cmp)(void*, void*));

ListItem    *copy           (ListItem *listp);
ListItem    *reverse        (ListItem *listp);
ListItem    *sort           (ListItem *listp, int (*cmp)(void*, void*), 
                              int order);

ListItem    *merge_sorted   (ListItem *listp1, ListItem *listp2,
                              int (*cmp)(void*, void*), int order);
ListItem    *split          (ListItem *listp1, void *split_before_data,
                              int (*cmp)(void*, void*));
ListItem    *split_at_index (ListItem *listp1, int index);

ListItem    *lookup         (ListItem *listp, void *data, 
                              int (*cmp)(void*, void*));
    
ListItem    *del_item       (ListItem *listp, void *del_data,
                              int (*cmp)(void*, void*));

int         index_of        (ListItem *listp, void *data, 
                              int (*cmp)(void*, void*));
int         get_length      (ListItem *listp);

void        for_each        (ListItem *listp, void (*fn)(ListItem*, void*), 
                              void *arg);

void        free_all        (ListItem *listp);

#endif
