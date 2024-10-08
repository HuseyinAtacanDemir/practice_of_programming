#include "jankunit.h"

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
            for (int i = 0; i < N; i++) {
                TEST("is %d not equal %d", i, N+1){
                    EXPECT_NE(11, i);
                }
            }
        }
    }
    return 0;
}
