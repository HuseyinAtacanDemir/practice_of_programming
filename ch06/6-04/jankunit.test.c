#include "jankunit.h"
#include "eprintf.h"
#include "shmalloc.h"

#include <stdlib.h>

char *InvPosNumErr = "Invalid Positive Numbers: %d %d";
char *SumInfoOut = "a: %d, b: %d\n sum: %d\n";

int some_fn(int a, int b)
{
    if (!a || !b)
        eprintf(InvPosNumErr, a, b);
    printf(SumInfoOut, a, b, (a+b));
    return a+b;
}

int side_effect_fn(int num, int *num_p)
{
    if (num == 5) {
        *num_p = 1;
        exit(*num_p);
    } else if (num == 9) {
        int *p = NULL;
        //segv
        int asd = *p;
    } else {
        *num_p = num;
    }
    return 0;
}

int infinite_fn()
{
    int a;
    for (;;)
        a++;

    return 0;
}

int main(void) 
{
    init_ctx(); 
    TEST_PROGRAM("Test the Jank Unit Test Library") {
        
        TEST_SUITE("Dumb Suite %s", "super dumb") {
            int start, end;
            TEST("is x equal y from %d to %d", (start = 0), (end = 10)) {
                for (int x = 0, y = 0; x <= end; x++, y++)
                    EXPECT_EQ(x, y);
            }
  
            int val = 10;
            char *str = "asd";
            TEST("is %d lte %d and %s equal to %s", val, val, str, str) { 
                EXPECT_LTE(val, val);
                EXPECT_EQ_STR(str, str);
            }
        }
        
        TEST_SUITE("Dumb Suite %d", 2) {
            
            int i, N;
            TEST("is %d not equal %d", (i = 0), ((N=10) + 1)) {
                for ( ; i < N; i++)
                    EXPECT_NEQ(11, i);
            }

            TEST("some_fn stdout and stderr should be correct") {
                int N = 10;
                for(int i = 0; i < N; i++) {
                    int a, b, result;
                    a = i;
                    b = i+1;
                    FORK() {
                        result = some_fn(a, b);
                        EXPECT_EQ(result, (a+b));
                    }
                    if (!a || !b) {
                        EXPECT_OUT_EQ(NULL);
                        char *msg = NULL;
                        easeprintf(&msg, InvPosNumErr, a, b); 
                        EXPECT_ERR_EQ(msg);
                        free(msg);
                        msg = NULL;
                    } else {
                        EXPECT_OUT_EQ(SumInfoOut, a, b, (a+b));
                        EXPECT_ERR_EQ(NULL);
                    }
                }
            }
            
            TEST("side_effect_fn should alter variable correctly") {
                int N = 10;
                int *sh_int = (int *) eshmalloc(sizeof(int));
                for (int i = 0; i < N; i++) {
                    FORK() {
                        int result;
                        result = side_effect_fn(i, sh_int);
                        EXPECT_EQ(result, 0);
                        EXPECT_EQ(*sh_int, i);
                    }
                    if (i == 5) {
                        EXPECT_EXIT_CODE_EQ(*sh_int);
                        EXPECT_SIGNAL_CODE_EQ(NOT_SIGNALED);
                        EXPECT_EQ(*sh_int, 1);
                    } else if (i == 9) {
                        EXPECT_SIGNAL_CODE_EQ(SIGSEGV);
                    } else {
                        EXPECT_EXIT_CODE_EQ(EXIT_SUCCESS);
                        EXPECT_SIGNAL_CODE_EQ(NOT_SIGNALED);
                        EXPECT_EQ(*sh_int, i);
                    }
                }
                eshfree(sh_int);
                sh_int = NULL;
                shfree_all();
            }

            TEST("FORK_KILL_AFTER should behave correctly") {
                FORK_KILL_AFTER(1) {
                    infinite_fn();
                    ASSERT_TRUE(0);
                }

                EXPECT_OUT_EQ(NULL);
                EXPECT_ERR_EQ(NULL);
                EXPECT_EXIT_CODE_EQ(NOT_EXITED);
                EXPECT_SIGNAL_CODE_EQ(SIGALRM);
            }
        }
    }
    return 0;
}
