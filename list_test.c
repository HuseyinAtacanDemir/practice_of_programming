#include <stdio.h>
#include <stdlib.h>

#include "list.h"

#define LIST_LEN 50

ListItem *create_random_int_list(int len);

void print_list_item(ListItem *listp, void *arg);

int main(int argc, char **argv)
{
    ListItem *list = create_random_int_list(LIST_LEN);

    for_each(list, print_list_item, NULL);  
}

ListItem *create_random_int_list(int len)
{
    int *data;
    ListItem *head, *p;
    
    if (len <= 0)
        return NULL;

    head = NULL;
    for ( ; len > 0; len--) {
        data = malloc(sizeof(int));
        *data = rand();

        if (head == NULL) {
            head = new_item((void *) data);
            p = head;
        } else {
            p->next = new_item((void *) data);
            p = p->next;
        }
    }
    
    return head;
}


void print_list_item(ListItem *listp, void *arg)
{
    int data = *((int *)(listp->data));
    printf("val: %d, next: %p\n", data, listp->next);
}

