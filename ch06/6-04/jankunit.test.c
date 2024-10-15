#include "jankunit.h"
#include "eprintf.h"

#include <stdlib.h>

char *InvPosNumErr = "Invalid Positive Numbers: %d %d\n";
char *SumInfoOut = "a: %d, b: %d\n sum: %d\n";

int some_fn(int a, int b)
{
    if (!a || !b)
        eprintf(InvPosNumErr, a, b);
    printf(SumInfoOut, a, b, (a+b));
    return a+b;
}

int main(void) 
{
    init_ctx(); 
    TEST_PROGRAM("Jank Unit Test Library", "")
    {
        TEST_SUITE("Dumb Suite %s", "super dumb")
        {
            int start = 0;
            int end = 100;
            TEST("is x equal y from %d to %d", start, end)
            {
                for (int x = 0, y = 0; x <= end; x++, y++)
                    EXPECT_EQ(x, y);
            }
            int val = 10;
            char *str = "asd";
            TEST("is %d lte %d and %s equal to %s", val, val, str, str)
            { 
                EXPECT_LTE(val, val);
                EXPECT_STREQ(str, str);
            }    
        }

        TEST_SUITE("Dumb Suite %d", 2)
        {
            int N = 10;
            int i = 0;
            TEST("is %d not equal %d", i, N+1)
            {
                for ( ; i < N; i++) {
                    ASSERT_NEQ(5, i);
                    EXPECT_NEQ(11, i);
                }
            }
            //TEST("Proposed hypothetical FORK with utilities")
            //{
                //int N = 11;
                //for(int i = 0; i < N; i++) {
                    //int a, b, result;
                    //FORK()
                    //{
                        //EXPECT_EQ(result, (a+b));
                        //exit(EXIT_SUCCESS);
                    //}
               //
                    //EXPECT_OUT_EQ(SumInfoOut, a, b);
                    //EXPECT_ERR_EQ("");
//
                    //ASSERT_EXIT_EQ(EXIT_SUCCESS);
                    //ASSERT_SIG_EQ(0);                
                //}
            //}
        }
    }
    return 0;
}
