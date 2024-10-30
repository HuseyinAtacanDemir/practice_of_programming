#include "jankunit.h"
#include "eprintf.h"
#include "qsort.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct Person Person;
struct Person {
    char  *first_name;
    char  *last_name;
    int   age;
};

int char_cmp          (void*, void*);
int int_cmp           (void*, void*);
int float_cmp         (void*, void*);
int double_cmp        (void*, void*);
int long_cmp          (void*, void*);
int person_p_fn_cmp   (void*, void*);
int person_p_ln_cmp   (void*, void*);
int person_p_age_cmp  (void*, void*);
int person_pp_fn_cmp  (void*, void*);
int person_pp_ln_cmp  (void*, void*);
int person_pp_age_cmp (void*, void*);

int *intdup(int len, ...);
double *double_dup(int len, ...);
float *float_dup(int len, ...);
long *long_dup(int len, ...);

int main(void)
{
    init_ctx();
    TEST_PROGRAM("QSORT_GENERIC UNIT TESTS") {
        TEST_SUITE("SORTS CHAR arrays") {
            struct {
                char *test_name;
                char *arr;
                int len;
                char *exp_arr;
                int exp_len;
                int order;
            } tests[] = {
                {"NULL char arr", NULL, 1, NULL, 0, QSORT_ASC},
                {"ONE elem char arr", 
                  estrdup("1"), 1, 
                  "1", 1, QSORT_ASC},
                {"TWO elem char arr", 
                  estrdup("51"), 2, 
                  "15", 2, QSORT_ASC},
                {"20 elem char arr", 
                  estrdup("gjskgyuxwucnmgjflkys"), 20, 
                  "cfgggjjkklmnssuuwxyy", 20, QSORT_ASC},
                {"20 elem char arr DESC", 
                  estrdup("gjskgyuxwucnmgjflkys"), 20, 
                  "yyxwuussnmlkkjjgggfc", 20, QSORT_DESC},
                {NULL, NULL, 0, NULL, 0, 0}
            };

            for (int i = 0; tests[i].test_name; i++) {
                TEST(tests[i].test_name) {
                    FORK() {
                        char *arr = tests[i].arr;
                        int len = tests[i].len;

                        qsort_generic(arr, len, sizeof(char), char_cmp, tests[i].order);

                        char *exp_arr = tests[i].exp_arr;
                        int  exp_len = tests[i].exp_len;
                        for (int j = 0; j < exp_len; j++)
                            EXPECT_EQ_CHAR(arr[j], exp_arr[j]);
    
                        if (exp_arr == NULL)
                            EXPECT_EQ_PTR(arr, exp_arr);
                    }
                    ASSERT_EXIT_CODE_EQ(EXIT_SUCCESS);
                    ASSERT_SIGNAL_CODE_EQ(NOT_SIGNALED);
                    EXPECT_OUT_EQ(NULL);
                    EXPECT_ERR_EQ(NULL);
                }
                if (tests[i].arr != NULL)
                    free(tests[i].arr);
            }
        }
        TEST_SUITE("SORTS INTEGER arrays") {
            struct {
                char *test_name;
                int *arr;
                int len;
                int *exp_arr;
                int exp_len;
                int order;
            } tests[] = {
                {"NULL int arr", 
                  NULL, 1, 
                  NULL, 0, QSORT_ASC},
                {"ONE elem int arr", 
                  intdup(1, 1), 1, 
                  intdup(1, 1), 1, QSORT_ASC},
                {"TWO elem int arr", 
                  intdup(2, 5, 1), 2, 
                  intdup(2, 1, 5), 2, QSORT_ASC},
                {"20 elem int arr", 
                  intdup(20,
                    20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1), 
                  20, 
                  intdup(20,
                    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20), 
                  20
                , QSORT_ASC},
                {"20 elem int arr DESC", 
                  intdup(20,
                    20,18,19,17,14,15,16,13,12,11,10,7,8,9,6,5,4,2,3,1), 
                  20, 
                  intdup(20,
                    20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1), 
                  20
                , QSORT_DESC},
                {NULL, NULL, 0, NULL, 0}
            };
            for (int i = 0; tests[i].test_name; i++) {
                TEST(tests[i].test_name) {
                    FORK() {
                        int *arr = tests[i].arr;
                        int len = tests[i].len;

                        qsort_generic(arr, len, sizeof(int), int_cmp, tests[i].order);

                        int *exp_arr = tests[i].exp_arr;
                        int exp_len = tests[i].exp_len;
                        for (int j = 0; j < exp_len; j++)
                            EXPECT_EQ(arr[j], exp_arr[j]);
    
                        if (exp_arr == NULL)
                            EXPECT_EQ_PTR(arr, exp_arr);
                    }
                    ASSERT_EXIT_CODE_EQ(EXIT_SUCCESS);
                    ASSERT_SIGNAL_CODE_EQ(NOT_SIGNALED);
                    EXPECT_OUT_EQ(NULL);
                    EXPECT_ERR_EQ(NULL);
                }
            }
        }
        TEST_SUITE("SORTS DOUBLE arrays") {
            struct {
                char *test_name;
                double *arr;
                int len;
                double *exp_arr;
                int exp_len;
                int order;
            } tests[] = {
                {"NULL double arr", 
                  NULL, 1, 
                  NULL, 0, QSORT_ASC},
                {"ONE elem double arr", 
                  double_dup(1, 1.5), 1, 
                  double_dup(1, 1.5), 1, QSORT_ASC},
                {"TWO elem double arr", 
                  double_dup(2, 5.5, 1.2), 2, 
                  double_dup(2, 1.2, 5.5), 2, QSORT_ASC},
                {"20 elem double arr", 
                  double_dup(20,
                    20.0,19.0,18.0,17.0,16.0,15.0,14.0,13.0,12.0,11.0,
                    10.0,9.0,8.0,7.0,6.0,5.0,4.0,3.0,2.0,1.0), 
                  20, 
                  double_dup(20,
                    1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,
                    10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,18.0,19.0,20.0), 
                  20
                , QSORT_ASC},
                {"20 elem double arr", 
                  double_dup(20,
                    20.0,18.0,19.0,17.0,16.0,12.0,14.0,13.0,15.0,10.0,
                    11.0,9.0,8.0,7.0,4.0,5.0,6.0,3.0,2.0,1.0), 
                  20, 
                  double_dup(20,
                    20.0,19.0,18.0,17.0,16.0,15.0,14.0,13.0,12.0,11.0,
                    10.0,9.0,8.0,7.0,6.0,5.0,4.0,3.0,2.0,1.0), 
                  20
                , QSORT_DESC},
                {NULL, NULL, 0, NULL, 0}
            };
            for (int i = 0; tests[i].test_name; i++) {
                TEST(tests[i].test_name) {
                    FORK() {
                        double *arr = tests[i].arr;
                        int len = tests[i].len;

                        qsort_generic(arr, len, sizeof(double), double_cmp, tests[i].order);

                        double *exp_arr = tests[i].exp_arr;
                        int exp_len = tests[i].exp_len;
                        for (int j = 0; j < exp_len; j++)
                            EXPECT_EQ_DOUBLE(arr[j], exp_arr[j]);
    
                        if (exp_arr == NULL)
                            EXPECT_EQ_PTR(arr, exp_arr);
                    }
                    ASSERT_EXIT_CODE_EQ(EXIT_SUCCESS);
                    ASSERT_SIGNAL_CODE_EQ(NOT_SIGNALED);
                    EXPECT_OUT_EQ(NULL);
                    EXPECT_ERR_EQ(NULL);
                }
            }
        
        }
        TEST_SUITE("SORTS FLOAT arrays") {
            struct {
                char *test_name;
                float *arr;
                int len;
                float *exp_arr;
                int exp_len;
            } tests[] = {
                {"NULL float arr", 
                  NULL, 1, 
                  NULL, 0},
                {"ONE elem float arr", 
                  float_dup(1, 1.5), 1, 
                  float_dup(1, 1.5), 1},
                {"TWO elem float arr", 
                  float_dup(2, 5.5, 1.2), 2, 
                  float_dup(2, 1.2, 5.5), 2},
                {"20 elem float arr", 
                  float_dup(20,
                    20.0,19.0,18.0,17.0,16.0,15.0,14.0,13.0,12.0,11.0,
                    10.0,9.0,8.0,7.0,6.0,5.0,4.0,3.0,2.0,1.0), 
                  20, 
                  float_dup(20,
                    1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,
                    10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,18.0,19.0,20.0), 
                  20
                },
                {NULL, NULL, 0, NULL, 0}
            };
            for (int i = 0; tests[i].test_name; i++) {
                TEST(tests[i].test_name) {
                    FORK() {
                        float *arr = tests[i].arr;
                        int len = tests[i].len;

                        qsort_generic(arr, len, sizeof(float), float_cmp, QSORT_ASC);

                        float *exp_arr = tests[i].exp_arr;
                        int exp_len = tests[i].exp_len;
                        for (int j = 0; j < exp_len; j++)
                            EXPECT_EQ_FLOAT(arr[j], exp_arr[j]);
    
                        if (exp_arr == NULL)
                            EXPECT_EQ_PTR(arr, exp_arr);
                    }
                    ASSERT_EXIT_CODE_EQ(EXIT_SUCCESS);
                    ASSERT_SIGNAL_CODE_EQ(NOT_SIGNALED);
                    EXPECT_OUT_EQ(NULL);
                    EXPECT_ERR_EQ(NULL);
                }
            }
        }
        TEST_SUITE("SORTS LONG arrays") {
            struct {
                char *test_name;
                long *arr;
                int len;
                long *exp_arr;
                int exp_len;
            } tests[] = {
                {"NULL long arr", 
                  NULL, 1, 
                  NULL, 0},
                {"ONE elem long arr", 
                  long_dup(1, 1), 1, 
                  long_dup(1, 1), 1},
                {"TWO elem long arr", 
                  long_dup(2, 5, 1), 2, 
                  long_dup(2, 1, 5), 2},
                {"20 elem long arr", 
                  long_dup(20,
                    20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1), 
                  20, 
                  long_dup(20,
                    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20), 
                  20
                },
                {NULL, NULL, 0, NULL, 0}
            };
            for (int i = 0; tests[i].test_name; i++) {
                TEST(tests[i].test_name) {
                    FORK() {
                        long *arr = tests[i].arr;
                        int len = tests[i].len;

                        qsort_generic(arr, len, sizeof(long), long_cmp, QSORT_ASC);

                        long *exp_arr = tests[i].exp_arr;
                        int exp_len = tests[i].exp_len;
                        for (int j = 0; j < exp_len; j++)
                            EXPECT_EQ_LONG(arr[j], exp_arr[j]);
    
                        if (exp_arr == NULL)
                            EXPECT_EQ_PTR(arr, exp_arr);
                    }
                    ASSERT_EXIT_CODE_EQ(EXIT_SUCCESS);
                    ASSERT_SIGNAL_CODE_EQ(NOT_SIGNALED);
                    EXPECT_OUT_EQ(NULL);
                    EXPECT_ERR_EQ(NULL);
                }
            }
        }
        TEST_SUITE("SORTS STRUCT arrays") {
            Person *people = (Person *) emalloc(sizeof(Person) * 5);
            people[0].first_name = estrdup("John"); 
            people[0].last_name = estrdup("McSomething"); 
            people[0].age = 23; 

            people[1].first_name = estrdup("Mark"); 
            people[1].last_name = estrdup("Somethingsey"); 
            people[1].age = 20; 

            people[2].first_name = estrdup("Aaron"); 
            people[2].last_name = estrdup("Somestein"); 
            people[2].age = 13; 

            people[3].first_name = estrdup("Ivan"); 
            people[3].last_name = estrdup("Ivanovich Somethingskiyy"); 
            people[3].age = 3; 

            people[4].first_name = estrdup("Shuji"); 
            people[4].last_name = estrdup("Samatinura"); 
            people[4].age = 21; 

            FORK() {
                qsort_generic(people, 5, sizeof(Person), person_p_fn_cmp, QSORT_ASC);
                EXPECT_EQ_STR(people[0].first_name, "Aaron");
                EXPECT_EQ_STR(people[1].first_name, "Ivan");
                EXPECT_EQ_STR(people[2].first_name, "John");
                EXPECT_EQ_STR(people[3].first_name, "Mark");
                EXPECT_EQ_STR(people[4].first_name, "Shuji");
            }
            
            ASSERT_EXIT_CODE_EQ(EXIT_SUCCESS);
            ASSERT_SIGNAL_CODE_EQ(NOT_SIGNALED);
            EXPECT_OUT_EQ(NULL);
            EXPECT_ERR_EQ(NULL);

            FORK() {
                qsort_generic(people, 5, sizeof(Person), person_p_ln_cmp, QSORT_ASC);
                EXPECT_EQ_STR(people[0].last_name, "Ivanovich Somethingskiyy");
                EXPECT_EQ_STR(people[1].last_name, "McSomething");
                EXPECT_EQ_STR(people[2].last_name, "Samatinura");
                EXPECT_EQ_STR(people[3].last_name, "Somestein");
                EXPECT_EQ_STR(people[4].last_name, "Somethingsey");
            }
            
            ASSERT_EXIT_CODE_EQ(EXIT_SUCCESS);
            ASSERT_SIGNAL_CODE_EQ(NOT_SIGNALED);
            EXPECT_OUT_EQ(NULL);
            EXPECT_ERR_EQ(NULL);

            FORK() {
                qsort_generic(people, 5, sizeof(Person), person_p_age_cmp, QSORT_ASC);
                EXPECT_EQ(people[0].age, 3);
                EXPECT_EQ(people[1].age, 13);
                EXPECT_EQ(people[2].age, 20);
                EXPECT_EQ(people[3].age, 21);
                EXPECT_EQ(people[4].age, 23);
            }
            
            ASSERT_EXIT_CODE_EQ(EXIT_SUCCESS);
            ASSERT_SIGNAL_CODE_EQ(NOT_SIGNALED);
            EXPECT_OUT_EQ(NULL);
            EXPECT_ERR_EQ(NULL);
        }
        TEST_SUITE("SORTS STRUCT PTR arrays") {
            Person **people = (Person **) emalloc(sizeof(Person *) * 5);
            for (int i = 0; i < 5; i++) {
                people[i] = (Person *) emalloc(sizeof(Person));
            }
            people[0]->first_name = estrdup("John"); 
            people[0]->last_name = estrdup("McSomething"); 
            people[0]->age = 23; 

            people[1]->first_name = estrdup("Mark"); 
            people[1]->last_name = estrdup("Somethingsey"); 
            people[1]->age = 20; 

            people[2]->first_name = estrdup("Aaron"); 
            people[2]->last_name = estrdup("Somestein"); 
            people[2]->age = 13; 

            people[3]->first_name = estrdup("Ivan"); 
            people[3]->last_name = estrdup("Ivanovich Somethingskiyy"); 
            people[3]->age = 3; 

            people[4]->first_name = estrdup("Shuji"); 
            people[4]->last_name = estrdup("Samatinura"); 
            people[4]->age = 21; 

            FORK() {
                qsort_generic(people, 5, sizeof(Person *), person_pp_fn_cmp, QSORT_ASC);
                EXPECT_EQ_STR(people[0]->first_name, "Aaron");
                EXPECT_EQ_STR(people[1]->first_name, "Ivan");
                EXPECT_EQ_STR(people[2]->first_name, "John");
                EXPECT_EQ_STR(people[3]->first_name, "Mark");
                EXPECT_EQ_STR(people[4]->first_name, "Shuji");
            } 
            ASSERT_EXIT_CODE_EQ(EXIT_SUCCESS);
            ASSERT_SIGNAL_CODE_EQ(NOT_SIGNALED);
            EXPECT_OUT_EQ(NULL);
            EXPECT_ERR_EQ(NULL);

            FORK() {
                qsort_generic(people, 5, sizeof(Person *), person_pp_ln_cmp, QSORT_ASC);
                EXPECT_EQ_STR(people[0]->last_name, "Ivanovich Somethingskiyy");
                EXPECT_EQ_STR(people[1]->last_name, "McSomething");
                EXPECT_EQ_STR(people[2]->last_name, "Samatinura");
                EXPECT_EQ_STR(people[3]->last_name, "Somestein");
                EXPECT_EQ_STR(people[4]->last_name, "Somethingsey");
            } 
            ASSERT_EXIT_CODE_EQ(EXIT_SUCCESS);
            ASSERT_SIGNAL_CODE_EQ(NOT_SIGNALED);
            EXPECT_OUT_EQ(NULL);
            EXPECT_ERR_EQ(NULL);

            FORK() {
                qsort_generic(people, 5, sizeof(Person *), person_pp_age_cmp, QSORT_ASC);
                EXPECT_EQ(people[0]->age, 3);
                EXPECT_EQ(people[1]->age, 13);
                EXPECT_EQ(people[2]->age, 20);
                EXPECT_EQ(people[3]->age, 21);
                EXPECT_EQ(people[4]->age, 23);
            }
            
            ASSERT_EXIT_CODE_EQ(EXIT_SUCCESS);
            ASSERT_SIGNAL_CODE_EQ(NOT_SIGNALED);
            EXPECT_OUT_EQ(NULL);
            EXPECT_ERR_EQ(NULL);
        }
    }
    return 0;
}

int char_cmp(void *a, void *b)
{
    return *((char *)a) - *((char *)b);
}

int int_cmp(void *a, void *b)
{
    return *((int *)a) - *((int *)b);
}
int double_cmp(void *a, void *b)
{
    return *((double *)a) - *((double *)b);
}

int float_cmp(void *a, void *b)
{
    return *((float *)a) - *((float *)b);
}

int long_cmp(void *a, void *b)
{
    return *((long *)a) - *((long *)b);
}

int person_p_fn_cmp(void *a, void *b)
{
    Person *person_a = (Person *) a;
    Person *person_b = (Person *) b;

    if (person_a->first_name || person_b->first_name)
        return strcmp(person_a->first_name, person_b->first_name);
    else if (person_a->first_name && !person_b->first_name)
        return 1;
    else if (!person_a->first_name && person_b->first_name)
        return -1;
    else
        return 0;
}

int person_pp_fn_cmp(void *a, void *b)
{
    Person **person_a = (Person **) a;
    Person **person_b = (Person **) b;

    if ((*person_a)->first_name || (*person_b)->first_name)
        return strcmp((*person_a)->first_name, (*person_b)->first_name);
    else if ((*person_a)->first_name && !(*person_b)->first_name)
        return 1;
    else if (!(*person_a)->first_name && (*person_b)->first_name)
        return -1;
    else
        return 0;
}

int person_p_ln_cmp(void *a, void *b)
{
    Person *person_a = (Person *) a;
    Person *person_b = (Person *) b;

    if (person_a->last_name || person_b->last_name)
        return strcmp(person_a->last_name, person_b->last_name);
    else if (person_a->last_name && !person_b->last_name)
        return 1;
    else if (!person_a->last_name && person_b->last_name)
        return -1;
    else
        return 0;
}

int person_pp_ln_cmp(void *a, void *b)
{
    Person **person_a = (Person **) a;
    Person **person_b = (Person **) b;

    if ((*person_a)->last_name || (*person_b)->last_name)
        return strcmp((*person_a)->last_name, (*person_b)->last_name);
    else if ((*person_a)->last_name && !(*person_b)->last_name)
        return 1;
    else if (!(*person_a)->last_name && (*person_b)->last_name)
        return -1;
    else
        return 0;
}

int person_p_age_cmp(void *a, void *b)
{
    Person *person_a = (Person *) a;
    Person *person_b = (Person *) b;

    return person_a->age - person_b->age;
}

int person_pp_age_cmp(void *a, void *b)
{
    Person **person_a = (Person **) a;
    Person **person_b = (Person **) b;

    return ((*person_a)->age) - ((*person_b)->age);
}

int *intdup(int len, ...) {
    int *dest = (int *) emalloc(sizeof(int) * len);

    va_list args;
    va_start(args, len);
    for (int i = 0; i < len; i++) {
        int value = va_arg(args, int);
        dest[i] = value;
    }
    va_end(args);
    return dest;
}

double *double_dup(int len, ...) {
    double *dest = (double *) emalloc(sizeof(double) * len);

    va_list args;
    va_start(args, len);
    for (int i = 0; i < len; i++) {
        double value = va_arg(args, double);
        dest[i] = value;
    }
    va_end(args);
    return dest;
}

float *float_dup(int len, ...) {
    float *dest = (float *) emalloc(sizeof(float) * len);

    va_list args;
    va_start(args, len);
    for (int i = 0; i < len; i++) {
        float value = (float) va_arg(args, double);
        dest[i] = value;
    }
    va_end(args);
    return dest;
}

long *long_dup(int len, ...) {
    long *dest = (long *) emalloc(sizeof(long) * len);

    va_list args;
    va_start(args, len);
    for (int i = 0; i < len; i++) {
        long value = va_arg(args, long);
        dest[i] = value;
    }
    va_end(args);
    return dest;
}
