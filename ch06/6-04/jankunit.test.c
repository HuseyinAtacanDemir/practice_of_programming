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

int main(void) {
    TEST_PROGRAM("Jank Unit Test Library", ""){
        TEST_SUITE("Dumb Suite %s", "super dumb"){
            int start = 0;
            int end = 100;
            TEST("is x equal y from %d to %d", start, end){
                for (int x = 0, y = 0; x <= end; x++, y++)
                    EXPECT_EQ(x, y);
            }
            int val = 10;
            char *str = "asd";
            TEST("is %d lte %d and %s equal to %s", val, val, str, str){    
                EXPECT_LTE(val, val);
                EXPECT_STREQ(str, str);
            }    
        }

        TEST_SUITE("Dumb Suite %d", 2) {
            int N = 10;
            int i = 0;
            TEST("is %d not equal %d", i, N+1){
                for ( ; i < N; i++) {
                    ASSERT_NE(5, i);
                    EXPECT_NE(11, i);
                }
            }

            //TEST("is exit success"){
                //int a, b;
                //for (a = b = 0; a < 10; a++, b++) {
                    //FORK(some_fn, a, b);
                    //if (!a || !b) {
                        //EXPECT_STREQ(ERR, InvPosNum, a, b);
                        //EXPECT_STREQ(OUT, "");
                        //EXPECT_EQ(EXIT_CODE, EXIT_FAILURE);
                    //} else {
                        //EXPECT_STREQ(ERR, "");
                        //EXPECT_STREQ(OUT, SumInfoOut, a, b, (a+b));
                        //EXPECT_EQ(EXIT_CODE, EXIT_SUCCESS);
                    //}
                //}
            //}
//
            ////OR
//
            //TEST("Should forked tests be like this?"){
                //SHARED("iii", a, b, result, 5, 5, 0 );
                //FORK("i", some_fn, result, a, b);
//
                //ASSERT_EQ(EXIT_CODE, 0);
                //ASSERT_EQ(SIGNAL, 0);
//
                //EXPECT_STREQ(OUT, exp_out);
                //EXPECT_STREQ(ERR, exp_err);
//
                //EXPECT_EQ(result, (a+b));
            //}
//
            ////Or
//
            //TEST("I think it should be like this"){
                //int a, b, result;
                //a = b = 5;
                //FORK(some_fn, result, a, b);
//
                //ASSERT_EXIT_EQ(0);
                //ASSERT_SIG_EQ(0);
//
                //EXPECT_EQ(result, (a+b));
//
                //EXPECT_OUT_EQ(SumInfoOut, a, b, (a+b));
                //EXPECT_ERR_EQ("");
            //}

// With this last case, I would need to be omniscient to know
// exactly what the fn being tested is going to return, especially if
// the return types have pointers in them, dynamically allocated data...
// I cannot just make a shared memory pointer point to the private mem
// space of a child process and expect any comparison to work in the parent
// after the process exits. Therefore whatever checks the user implements,
// meaning checks other than OUT ERR EXIT SIG comparisons, will have to
// be made in the scope of the FORK, meaning shmem will only be used for
// synchronizing the TEST/SUITE/PROG state after the child exits

// so we demand knowledge of the internals of FORK from the user.
// because it will be the users responsibility to test the non-shared
// resources withint he FORK block

// In that case, let's go all the way and only make EXIT and SIG tests
// be conventional just outside following the FORK.

// Thus we should handle the OUT and ERR buf checks within the FORK scope,
// So we need to implement the piping logic within FORK

// Since this will be complicated, perhaps instead of wrapping every aspect
// of testing exiting/printing functions in macros, we might just give
// utility functions to the user
// So a hypothetical flow of using FORK on an exiting/printing function
// could be like:

            TEST("Proposed hypothetical FORK with utilities"){
                int N = 11;
                for(int i = 0; i < N; i++){
                    int a, b, result;
                    FORK(fn, result, a, b){
                        EXPECT_EQ(result, (a+b));
                        EXPECT_OUT_EQ(SumInfoOut, a, b);
                        EXPECT_ERR_EQ("");
                        exit(EXIT_SUCCESS);
                    } FORK_END;
               
                    ASSERT_EXIT_EQ(EXIT_SUCCESS);
                    ASSERT_SIG_EQ(0);                
                }
            }

// Perhaps this whole thing should be scratched and we should move towards
// a MOCKING related solution instead. I want this to be plug and play,
// however, so that the USER won't need to alter anything in their code
// to test their code. I want the USER to be able to just include
// their relevant header in the test file and just call any function
// without worrying about other things, but then again, managing shmem
// and considering the implications of FORKING end up being other things
// to worry about.
        }
    }
    return 0;
}
