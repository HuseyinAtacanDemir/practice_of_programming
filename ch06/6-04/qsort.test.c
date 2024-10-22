#include "jankunit.h"
#include "qsort.h"
#include "shmalloc.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct Person Person;
struct Person {
    char  *first_name;
    char  *last_name;
    int   age;
};

int char_cmp        (const void*, const void*);
int int_cmp         (const void*, const void*);
int float_cmp       (const void*, const void*);
int double_cmp      (const void*, const void*);
int long_cmp        (const void*, const void*);
int person_fn_cmp   (const void*, const void*);
int person_ln_cmp   (const void*, const void*);
int person_age_cmp  (const void*, const void*);

int *shintdup(int len, ...);

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
            } tests[] = {
                {"NULL char arr", NULL, 1, NULL, 0},
                {"ONE elem char arr", eshstrdup("1"), 1, "1", 1},
                {"TWO elem char arr", eshstrdup("51"), 2, "15", 2},
                {"20 elem char arr", eshstrdup("gjskgyuxwucnmgjflkys"), 20, "cfgggjjkklmnssuuwxyy", 20},
                {NULL, NULL, 0, NULL, 0}
            };

            for (int i = 0; tests[i].test_name; i++) {
                TEST(tests[i].test_name) {
                    char *arr = tests[i].arr;
                    int len = tests[i].len;
                    FORK() {
                        qsort_generic(arr, len, sizeof(char), char_cmp);
                    }
                    ASSERT_EXIT_CODE_EQ(EXIT_SUCCESS);
                    ASSERT_SIGNAL_CODE_EQ(NOT_SIGNALED);
                    EXPECT_OUT_EQ(NULL);
                    EXPECT_ERR_EQ(NULL);
                    char *exp_arr = tests[i].exp_arr;
                    int  exp_len = tests[i].exp_len;
                    for (int j = 0; j < exp_len; j++)
                        EXPECT_EQ_CHAR(arr[j], exp_arr[j]);

                    if (exp_arr == NULL)
                        EXPECT_EQ_PTR(arr, exp_arr);
                }
            }
        }
        TEST_SUITE("SORTS INTEGER arrays") {
            struct {
                char *test_name;
                int *arr;
                int len;
                int *exp_arr;
                int exp_len;
            } tests[] = {
                {"NULL int arr", NULL, 1, NULL, 0},
                {"ONE elem int arr", shintdup((int[]){1}, 1), 1, shintdup((int[]){1}, 1), 1},
                {"TWO elem int arr", shintdup((int[]){5, 1}, 2), 2, shintdup((int[]){1, 5}, 2), 2},
                {"20 elem int arr", shintdup((int[]){20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1}, 20), 20, shintdup((int[]){1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20}, 20), 20},
                {NULL, NULL, 0, NULL, 0}
            };
            for (int i = 0; tests[i].test_name; i++) {
                TEST(tests[i].test_name) {
                    int *arr = tests[i].arr;
                    int len = tests[i].len;
                    FORK() {
                        qsort_generic(arr, len, sizeof(int), int_cmp);
                    }
                    ASSERT_EXIT_CODE_EQ(EXIT_SUCCESS);
                    ASSERT_SIGNAL_CODE_EQ(NOT_SIGNALED);
                    EXPECT_OUT_EQ(NULL);
                    EXPECT_ERR_EQ(NULL);
                    int *exp_arr = tests[i].exp_arr;
                    int exp_len = tests[i].exp_len;
                    for (int j = 0; j < exp_len; j++)
                        EXPECT_EQ(arr[j], exp_arr[j]);

                    if (exp_arr == NULL)
                        EXPECT_EQ_PTR(arr, exp_arr);
                }
            }
        }
        TEST_SUITE("SORTS DOUBLE arrays") {
        
        }
        TEST_SUITE("SORTS FLOAT arrays") {
        
        }
        TEST_SUITE("SORTS LONG arrays") {
        
        }
        TEST_SUITE("SORTS STRUCT arrays") {
        
        }
        TEST_SUITE("SORTS STRUCT PTR arrays") {
        
        }
    }
    shfree_all();
    return 0;
}

int char_cmp(const void *a, const void *b)
{
    return *((char *)a) - *((char *)b);
}

int int_cmp(const void *a, const void *b)
{
    return *((int *)a) - *((int *)b);
}
int double_cmp(const void *a, const void *b)
{
    return *((double *)a) - *((double *)b);
}

int float_cmp(const void *a, const void *b)
{
    return *((float *)a) - *((float *)b);
}

int long_cmp(const void *a, const void *b)
{
    return *((long *)a) - *((long *)b);
}

int person_fn_cmp(const void *a, const void *b)
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

int person_ln_cmp(const void *a, const void *b)
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

int person_age_cmp(const void *a, const void *b)
{
    Person *person_a = (Person *) a;
    Person *person_b = (Person *) b;

    return person_a->age - person_b->age;
}

int *shintdup(int len, ...) {
    int *dest = (int *) eshmalloc(sizeof(int) * len);

    va_list args;
    va_start(args, len);
    for (int i = 0; i < len; i++) {
        int value = va_arg(args, int);
        dest[i] = value;
    }
    va_end(args);
    return dest;
}
