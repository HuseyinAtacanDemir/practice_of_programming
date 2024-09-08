#include <stdlib.h>
#include "list.h"

ListItem *new_item(void *data)
{
    ListItem *newp;

    newp = (ListItem *) malloc(sizeof(ListItem));
    newp->data = data;
    newp->next = NULL;
    
    return newp;
}

ListItem *add_front(ListItem *listp, ListItem *newp)
{
    if (newp == NULL)
        return NULL;

    newp->next = listp;
    return newp;
}

ListItem *add_end(ListItem *listp, ListItem *newp)
{
    ListItem *p;

    if (listp == NULL || newp == NULL)
        return NULL;

    if (listp == NULL)
        return newp;

    for (p = listp; p->next != NULL; p = p->next)
        ;
    p->next = newp;

    return listp;        
}

ListItem *insert_after(ListItem *listp, ListItem *newp, 
                       void *after_data, int (*cmp)(void*, void*))
{
    ListItem *p;

    if (listp == NULL || newp == NULL)
        return NULL;
    
    if (listp == NULL)
        return newp;

    for (p = listp; p != NULL; p = p->next)
        if ((*cmp)(p->data, after_data) == 0) {
            newp->next = p->next;
            p->next = newp;
            return listp;
        }
    
    return NULL;
}

ListItem *insert_before(ListItem *listp, ListItem *newp, 
                        void *before_data, int (*cmp)(void*, void*))
{
    ListItem *p, *prev;

    if (listp == NULL || newp == NULL)
        return NULL;

    if (listp == NULL)
        return newp;

    prev = NULL;
    for (p = listp; p != NULL; p = p->next) {
        if ((*cmp)(p->data, listp) == 0)
            break;
        prev = p;
    }

    if (p == NULL)
        return NULL;
    
    newp->next = p;
    if (prev != NULL)
        prev->next = newp;

    return listp;
}

ListItem *copy(ListItem *listp)
{
    ListItem *listp_copy, *p, *prev;

    if (listp == NULL)
        return NULL;
 
    listp_copy = prev = NULL;
    for ( ; listp != NULL; listp = listp->next) {
        p = new_item(listp->data);

        if (prev != NULL)
            prev->next = p;

        if (listp_copy == NULL)
            listp_copy = p;

        prev = p;
    }
    return listp_copy;
}

ListItem *reverse(ListItem *listp)
{
    ListItem *p, *prev;
    if (listp == NULL)
        return NULL;

    prev = NULL;
    p = listp;
    while (listp != NULL) {
        listp = listp->next;
        p->next = prev;
        prev = p;
        p = listp;
    }

    return prev;
}

// split_internal: splits the list pointed to by **listpt,
ListItem *split_internal(ListItem *listp, int len)
{
    ListItem *second;
    int i;
    for (i = 1; listp != NULL && i < len; i++, listp = listp->next) 
        ;
    if (listp == NULL)
        return NULL;
    second = listp->next;
    listp->next = NULL;
    return second;
}

ListItem *merge_internal(ListItem *left, ListItem *right, ListItem *tail, int (*cmp)(void*, void*), int order)
{
    ListItem *cur;
    cur = tail;
    while (left != NULL && right != NULL) {
        if ((*cmp)(right->data, left->data) == order) {
            cur->next = left;
            cur       = left;
            left      = left->next;
        } else {
            cur->next = right;
            cur       = right;
            right     = right->next;
        }
    }
    cur->next = (left != NULL) ? left : right;
    while (cur->next != NULL)
        cur = cur->next;
    return cur;
}

ListItem *sort(ListItem *listp, int (*cmp)(void*, void*), int order)
{
  ListItem *tmp_head, *cur, *left, *right, *tail;
  int list_len, partition_size;

  if (listp == NULL)
      return NULL;
  if (listp->next == NULL)
      return listp;

  list_len        = get_length(listp);
  tmp_head        = new_item(NULL);
  tmp_head->next  = listp;

  for (partition_size = 1; partition_size < list_len; partition_size *= 2) {
      cur = tmp_head->next;
      tail = tmp_head;
      while (cur != NULL) {
          left = cur;
          right = split_internal(left, partition_size);
          cur   = split_internal(right, partition_size);
          tail  = merge_internal(left, right, tail, cmp, order); 
      }
  }

  cur = tmp_head->next;
  free(tmp_head);
  return cur;
}

/* merge_sorted: merge two sorted lists
   returns the head to the merged list
   returns unsorted result if given unsorted lists
   ordering should match the ordering of input lists

   args:
   ListItem *listp1:
    head of list1
   ListItem *listp2:
    head of list2
   int (*cmp)(void* data1, void* data2): 
    compares ListItem.data and
    returns 1 if data1>data2, -1 if data1<data2, 0 if data1 == data2
   int order:
    1 for Ascending
    -1 for Descending
*/
ListItem *merge_sorted(ListItem *listp1, ListItem *listp2, 
                       int (*cmp)(void*, void*), int order)
{
    ListItem *head, *p;

    if (listp1 == NULL && listp2 == NULL)
        return NULL;
    else if (listp1 == NULL)
        return listp2;
    else if (listp2 == NULL)
        return listp1;

    if ((*cmp)(listp2->data, listp1->data) == order) {
        head = listp1;
        listp1 = listp1->next;
    } else {
        head = listp2;
        listp2 = listp2->next;
    }

    for (p = head; listp1 != NULL && listp2 != NULL; p = p->next)
        if ((*cmp)(listp2->data, listp1->data) == order) {
            p->next  = listp1;
            listp1      = listp1->next;
        } else {
            p->next  = listp2;
            listp2      = listp2->next;
        }
    
    if (listp1 != NULL)
        p->next = listp1;
    if (listp2 != NULL)
        p->next = listp2;
    return head;
}

// split: splits list at first occurance of the list item
//        with the given data; Makes that list item the 
//        Head of the new list. If no item with given data
//        is found returns NULL. 
//        Ptr to the list in the input args keeps pointing 
//        to the head of original list.
ListItem *split(ListItem *listp1, void *split_before_data, 
                int (*cmp)(void*, void*))
{
    ListItem *p, *prev;
    if (split_before_data == NULL || listp1 == NULL)
        return NULL;
    
    prev = NULL;
    for (p = listp1; p != NULL; p = p->next) {
        if ((*cmp)(p->data, split_before_data) == 0)
            break;
        prev = p;
    }

    if (p == NULL)
        return NULL;

    if (prev != NULL)
        prev->next = NULL;

    return p;
}

// split_at_index: splits given list at index, returns head
//                 to the splitted list whose head is the item
//                 previously present at "index" index of listp.
//                 severs the link from listp to new list.
ListItem *split_at_index(ListItem *listp, int index)
{
    ListItem *p, *prev;
    
    prev = NULL;
    for (p = listp; p != NULL && index > 0; p = p->next, index--)
        prev = p;
    
    if (p == NULL && prev == NULL)
        return NULL;
    else if (p == NULL)
        return listp;
    else if (prev == NULL)
        return listp;

    prev->next = NULL;
    return p;
}

ListItem *del_item(ListItem *listp, void *data, 
                   int (*cmp)(void*, void*))
{
    ListItem *p, *prev;

    if (listp == NULL || data == NULL)
        return NULL;

    prev = NULL;
    for (p = listp; p != NULL; p = p->next) {
        if ((*cmp)(p->data, data) == 0)
            break;

        prev = p;
    }

    if (p == NULL)
        return NULL;
    
    if (prev == NULL)
        listp = p->next;
    else
        prev->next = p->next;
    free(p);
    
    return listp;
}

ListItem *lookup(ListItem *listp, void *data, 
                 int (*cmp)(void*, void*))
{
    if (listp == NULL || data == NULL)
        return NULL;

    for ( ; listp != NULL; listp = listp->next)
        if ((*cmp)(listp->data, data) == 0)
            return listp;

    return NULL;
}

int index_of(ListItem *listp, void *data, int (*cmp)(void*, void*))
{
    int index;
    if (listp == NULL || data == NULL)
        return -1;

    for (index = 0; listp != NULL; listp = listp->next, index++)
        if ((*cmp)(listp->data, data) == 0)
            break;

    if(listp == NULL)
        return -1;

    return index;
}

int get_length(ListItem *listp)
{
    int count;
    if (listp == NULL)
        return 0;

    for (count = 0; listp != NULL; listp = listp->next)
        count++;
    return count;
}

void for_each(ListItem *listp, void (*fn)(ListItem*, void*), void *arg)
{
    for ( ; listp != NULL; listp = listp->next)
        (*fn)(listp, arg); 
}

void free_all(ListItem *listp)
{
    ListItem *next;
    for ( ; listp != NULL; listp = next) {
        next = listp->next;
        free(listp);
    }
}

