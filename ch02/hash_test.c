#include <stdio.h>
#include <stdlib.h>
#include "hash.h"


#define POPULATION    100
#define MAX_NAME_LEN  10
#define SSN_LEN       10
#define SSN_MIN       1000000000

#define rand_letter()               ( rand() % (('Z'-'A') + 1) + 'A' )
#define rand_digit()                ( rand() % 10 )
#define rand_name_len()             ( (rand() % (MAX_NAME_LEN-1)) + 1)
#define cur_letter_offset(i, j, k)  ( (i * MAX_NAME_LEN * 2) + (j * MAX_NAME_LEN) + k )

#define IS_KEY_STRING   0
#define N_BUCKET        -1
#define LOAD_FACTOR     -1.0
#define GROWTH_FACTOR   -1

typedef struct Person Person;
struct Person {
    long   ssn;
    char  *fname;
    char  *lname;
};

void *get_key_field(void *data);
char *random_name();
long random_ssn();
Person **create_people();


int main(int argc, char **argv)
{
    Hashmap *hmap;
    Person  **people;
    int     i;

    hmap    = init_hmap(get_key_field, sizeof(long), IS_KEY_STRING, N_BUCKET, LOAD_FACTOR, GROWTH_FACTOR);
    people  = create_people();

    for (i = 0; i < POPULATION; i++)
        insert(hmap, people[i], NULL);

}

void test_insertion(Person **people, Hashmap *hmap)
{
    printf("ssn: %ld, fname: %s, lname: %s\n", people[0]->ssn, people[0]->fname, people[0]->lname);

    HashmapItem *hmi = find(hmap, (void *) people[0], 0, NULL);

    printf("ssn: %ld, fname: %s, lname: %s\n", 
            ((Person *) hmi->data)->ssn, ((Person *) hmi->data)->fname, ((Person *) hmi->data)->lname);
}

void test_deletion(Person **people, Hashmap *hmap)
{

    del_hmi(hmap, (void *) people[0]);
    hmi = find(hmap, (void *) people[0], 0, NULL);

    if (hmi != NULL) {
        printf("ssn: %ld, fname: %s, lname: %s\n", 
                ((Person *) hmi->data)->ssn, ((Person *) hmi->data)->fname, ((Person *) hmi->data)->lname);
    } else {
        printf("NOT FOUND\n");
    }

}

void *get_key_field(void *data)
{
    return (void *) &(((Person *) data)->ssn);
}

Person **create_people()
{
    int i;
    Person *person, **people;
    people = (Person **) malloc(sizeof(Person *) * POPULATION);

    for (i = 0; i < POPULATION; i++) {
        person = (Person *) malloc(sizeof(Person));
        person->fname = random_name();
        person->lname = random_name();
        person->ssn   = random_ssn();
  
        people[i] = person;
    }
    return people;
}

char *random_name()
{
    char  *name;
    int   i, name_len;

    name_len = rand_name_len();
    name = (char *) malloc(sizeof(char) * name_len);
        
    for (i = 0; i < name_len-1; i++)
        *(name+i) = rand_letter();
    *(name+i) = '\0';
    return name;
}

long random_ssn()
{
    int i;
    long ssn;
    ssn = 0;
    for (i = 0; i < SSN_LEN || ssn < SSN_MIN; i++)
      ssn = ssn * 10 + rand_digit();
    return ssn;
}

