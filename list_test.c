#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#define MAX_NAME_LEN    10
#define LIST_LEN        50
#define ASC             1
#define DESC            -1

#define rand_letter()               ( rand() % (('Z'-'A') + 1) + 'A' )
#define rand_name_len()             ( (rand() % (MAX_NAME_LEN-1)) + 1)
#define cur_letter_offset(i, j, k)  ( (i * MAX_NAME_LEN * 2) + (j * MAX_NAME_LEN) + k )

typedef struct Person Person;
struct Person {
    char      *f_name;
    char      *l_name;
};

ListItem      *create_random_person_list  (int len);
ListItem      *create_random_int_list     (int len);

void          print_int_list_item         (ListItem *listp, void *arg);
void          print_person_list_item      (ListItem *listp, void *arg);

int           int_comparator              (void *data1, void *data2);
int           person_comparator           (void *data1, void *data2);

int main(int argc, char **argv)
{
    //ListItem *int_list = create_random_int_list(LIST_LEN);
    //for_each(int_list, print_int_list_item, NULL);  

    //int_list = sort(int_list, int_comparator, ASC);
    //for_each(int_list, print_int_list_item, NULL);

    ListItem *person_list = create_random_person_list(LIST_LEN);
    for_each(person_list, print_person_list_item, NULL);

    person_list = sort(person_list, person_comparator, ASC);
    for_each(person_list, print_person_list_item, NULL);
    
    return 0;
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

ListItem *create_random_person_list(int len)
{
    ListItem  *list, *p;
    Person    *persons, *cur_person; 

    char      *names;
    int       i, j, k, name_len;

    persons = malloc(sizeof(Person) * len);
    list    = malloc(sizeof(ListItem) * len);
    names   = malloc(sizeof(char) * MAX_NAME_LEN * len * 2);

    for (i = 0; i < len; i++) {
        for (j = 0; j < 2; j++) {
            name_len = rand_name_len();
            for (k = 0; k < name_len; k++)
                *(names+cur_letter_offset(i, j, k)) = rand_letter();
            *(names+cur_letter_offset(i, j, k)) = '\0';
        }

        cur_person          = persons+i;
        cur_person->f_name  = names+(i*MAX_NAME_LEN*2);
        cur_person->l_name  = names+(i*MAX_NAME_LEN*2) + MAX_NAME_LEN;
        
        p       = list+i;
        p->data = (void *) cur_person;
        p->next = (i != (len-1)) ? list+i+1 : NULL;
    }

    return list;
}

void print_int_list_item(ListItem *listp, void *arg)
{
    int data = *((int *)(listp->data));
    printf("val: %d, next: %p\n", data, listp->next);
}

void print_person_list_item(ListItem *listp, void *arg)
{
    Person *data = (Person *)(listp->data);
    printf("f_name: %s, l_name: %s, next: %p\n", data->f_name, data->l_name, listp->next);
}

int int_comparator(void *data1, void *data2)
{
    int d1 = *((int *) data1);
    int d2 = *((int *) data2);

    if (d1 > d2)
      return 1;
    else if (d1 < d2)
      return -1;
    else
      return 0;
}

int person_comparator(void *person1, void *person2)
{
    int f_name_cmp, l_name_cmp;
    Person *p1, *p2;
    
    p1 = (Person *) person1;
    p2 = (Person *) person2;

    f_name_cmp = strcmp(p1->f_name, p2->f_name);
    
    if (f_name_cmp != 0)
      return (f_name_cmp > 0 ? 1 : -1);
    
    l_name_cmp = strcmp(p1->l_name, p2->l_name);
    return (l_name_cmp > 0 ? 1 : -1);
}
