#include "jankunit.h"

int main(void) {

    start_test_suite("Suite 1");

    start_test("Test 1");
    int x = 5;
    int y = 6;
    EXPECT_EQ(x, y);
    EXPECT_GT(x, y);
    end_test();

    start_test("Test 2");    
    EXPECT_LTE(10, 10);
    EXPECT_STREQ("asd", "dsa");
    end_test();
    
    end_test_suite();

    start_test_suite("Suite 2");
    EXPECT_NE(1, 1);
    end_test_suite();


    return 0;
}
