#include <stdio.h>

#define PASS()             \
    do                     \
    {                      \
        printf("PASS:\n"); \
    } while (0)

#define FAIL()                     \
    do                             \
    {                              \
        printf("FAILED EXPECT\n"); \
    } while (0)

#define FAIL_ASSERT()              \
    do                             \
    {                              \
        printf("FAILED ASSERT\n"); \
    } while (0)

// Comparison macros
#define EQ_COMMON (_a_ == _b_)
#define NEQ_COMMON (_a_ != _b_)
#define GT_COMMON (_a_ > _b_)
#define GTE_COMMON (_a_ >= _b_)
#define LT_COMMON (_a_ < _b_)
#define LTE_COMMON (_a_ <= _b_)

// General comparison macro
#define COMPARE_COMMON(type, comparison, val1, val2, fail_action, fail_message) \
    do                                                                          \
    {                                                                           \
        type _a_ = (val1);                                                      \
        type _b_ = (val2);                                                      \
        if (!(comparison))                                                      \
        {                                                                       \
            printf(fail_message, _b_, _a_, #val1, #val2);                       \
            fail_action;                                                        \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            PASS();                                                             \
        }                                                                       \
    } while (0)

// Equality comparisons
#define EXPECT_EQ(val1, val2)         COMPARE_COMMON(int, EQ_COMMON, val1, val2, FAIL(), "Expected %d, but got %d, %s == %s\n")
#define ASSERT_EQ(val1, val2)         COMPARE_COMMON(int, EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d, but got %d, %s == %s\n")
#define EXPECT_EQ_LONG(val1, val2)    COMPARE_COMMON(long, EQ_COMMON, val1, val2, FAIL(), "Expected %ld, but got %ld, %s == %s\n")
#define ASSERT_EQ_LONG(val1, val2)    COMPARE_COMMON(long, EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %ld, but got %ld, %s == %s\n")
#define EXPECT_EQ_FLOAT(val1, val2)   COMPARE_COMMON(float, EQ_COMMON, val1, val2, FAIL(), "Expected %f, but got %f, %s == %s\n")
#define ASSERT_EQ_FLOAT(val1, val2)   COMPARE_COMMON(float, EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %f, but got %f, %s == %s\n")
#define EXPECT_EQ_DOUBLE(val1, val2)  COMPARE_COMMON(double, EQ_COMMON, val1, val2, FAIL(), "Expected %lf, but got %lf, %s == %s\n")
#define ASSERT_EQ_DOUBLE(val1, val2)  COMPARE_COMMON(double, EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %lf, but got %lf, %s == %s\n")
#define EXPECT_EQ_CHAR(val1, val2)    COMPARE_COMMON(char, EQ_COMMON, val1, val2, FAIL(), "Expected %c, but got %c, %s == %s\n")
#define ASSERT_EQ_CHAR(val1, val2)    COMPARE_COMMON(char, EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %c, but got %c, %s == %s\n")
#define EXPECT_EQ_SHORT(val1, val2)   COMPARE_COMMON(short, EQ_COMMON, val1, val2, FAIL(), "Expected %d, but got %d, %s == %s\n")
#define ASSERT_EQ_SHORT(val1, val2)   COMPARE_COMMON(short, EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d, but got %d, %s == %s\n")
#define EXPECT_EQ_PTR(val1, val2)     COMPARE_COMMON(void*, EQ_COMMON, val1, val2, FAIL(), "Expected %p, but got %p, %s == %s\n")
#define ASSERT_EQ_PTR(val1, val2)     COMPARE_COMMON(void*, EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %p, but got %p, %s == %s\n")

// Inequality comparisons
#define EXPECT_NEQ(val1, val2)        COMPARE_COMMON(int, NEQ_COMMON, val1, val2, FAIL(), "Expected %d != %d, but got %d == %d, %s != %s\n")
#define ASSERT_NEQ(val1, val2)        COMPARE_COMMON(int, NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d != %d, but got %d == %d, %s != %s\n")
#define EXPECT_NEQ_LONG(val1, val2)   COMPARE_COMMON(long, NEQ_COMMON, val1, val2, FAIL(), "Expected %ld != %ld, but got %ld == %ld, %s != %s\n")
#define ASSERT_NEQ_LONG(val1, val2)   COMPARE_COMMON(long, NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %ld != %ld, but got %ld == %ld, %s != %s\n")
#define EXPECT_NEQ_FLOAT(val1, val2)  COMPARE_COMMON(float, NEQ_COMMON, val1, val2, FAIL(), "Expected %f != %f, but got %f == %f, %s != %s\n")
#define ASSERT_NEQ_FLOAT(val1, val2)  COMPARE_COMMON(float, NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %f != %f, but got %f == %f, %s != %s\n")
#define EXPECT_NEQ_DOUBLE(val1, val2) COMPARE_COMMON(double, NEQ_COMMON, val1, val2, FAIL(), "Expected %lf != %lf, but got %lf == %lf, %s != %s\n")
#define ASSERT_NEQ_DOUBLE(val1, val2) COMPARE_COMMON(double, NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %lf != %lf, but got %lf == %lf, %s != %s\n")
#define EXPECT_NEQ_CHAR(val1, val2)   COMPARE_COMMON(char, NEQ_COMMON, val1, val2, FAIL(), "Expected %c != %c, but got %c == %c, %s != %s\n")
#define ASSERT_NEQ_CHAR(val1, val2)   COMPARE_COMMON(char, NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %c != %c, but got %c == %c, %s != %s\n")
#define EXPECT_NEQ_SHORT(val1, val2)  COMPARE_COMMON(short, NEQ_COMMON, val1, val2, FAIL(), "Expected %d != %d, but got %d == %d, %s != %s\n")
#define ASSERT_NEQ_SHORT(val1, val2)  COMPARE_COMMON(short, NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d != %d, but got %d == %d, %s != %s\n")
#define EXPECT_NEQ_PTR(val1, val2)    COMPARE_COMMON(void*, NEQ_COMMON, val1, val2, FAIL(), "Expected %p != %p, but got %p == %p, %s != %s\n")
#define ASSERT_NEQ_PTR(val1, val2)    COMPARE_COMMON(void*, NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %p != %p, but got %p == %p, %s != %s\n")

// Greater-than comparisons
#define EXPECT_GT(val1, val2)         COMPARE_COMMON(int, GT_COMMON, val1, val2, FAIL(), "Expected %d > %d, but got %d <= %d, %s > %s\n")
#define ASSERT_GT(val1, val2)         COMPARE_COMMON(int, GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d > %d, but got %d <= %d, %s > %s\n")
#define EXPECT_GT_LONG(val1, val2)    COMPARE_COMMON(long, GT_COMMON, val1, val2, FAIL(), "Expected %ld > %ld, but got %ld <= %ld, %s > %s\n")
#define ASSERT_GT_LONG(val1, val2)    COMPARE_COMMON(long, GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %ld > %ld, but got %ld <= %ld, %s > %s\n")
#define EXPECT_GT_FLOAT(val1, val2)   COMPARE_COMMON(float, GT_COMMON, val1, val2, FAIL(), "Expected %f > %f, but got %f <= %f, %s > %s\n")
#define ASSERT_GT_FLOAT(val1, val2)   COMPARE_COMMON(float, GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %f > %f, but got %f <= %f, %s > %s\n")
#define EXPECT_GT_DOUBLE(val1, val2)  COMPARE_COMMON(double, GT_COMMON, val1, val2, FAIL(), "Expected %lf > %lf, but got %lf <= %lf, %s > %s\n")
#define ASSERT_GT_DOUBLE(val1, val2)  COMPARE_COMMON(double, GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %lf > %lf, but got %lf <= %lf, %s > %s\n")
#define EXPECT_GT_CHAR(val1, val2)    COMPARE_COMMON(char, GT_COMMON, val1, val2, FAIL(), "Expected %c > %c, but got %c <= %c, %s > %s\n")
#define ASSERT_GT_CHAR(val1, val2)    COMPARE_COMMON(char, GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %c > %c, but got %c <= %c, %s > %s\n")
#define EXPECT_GT_SHORT(val1, val2)   COMPARE_COMMON(short, GT_COMMON, val1, val2, FAIL(), "Expected %d > %d, but got %d <= %d, %s > %s\n")
#define ASSERT_GT_SHORT(val1, val2)   COMPARE_COMMON(short, GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d > %d, but got %d <= %d, %s > %s\n")
#define EXPECT_GT_PTR(val1, val2)     COMPARE_COMMON(void*, GT_COMMON, val1, val2, FAIL(), "Expected %p > %p, but got %p <= %p, %s > %s\n")
#define ASSERT_GT_PTR(val1, val2)     COMPARE_COMMON(void*, GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %p > %p, but got %p <= %p, %s > %s\n")

// Greater-than-or-equal comparisons
#define EXPECT_GTE(val1, val2)        COMPARE_COMMON(int, GTE_COMMON, val1, val2, FAIL(), "Expected %d >= %d, but got %d < %d, %s >= %s\n")
#define ASSERT_GTE(val1, val2)        COMPARE_COMMON(int, GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d >= %d, but got %d < %d, %s >= %s\n")
#define EXPECT_GTE_LONG(val1, val2)   COMPARE_COMMON(long, GTE_COMMON, val1, val2, FAIL(), "Expected %ld >= %ld, but got %ld < %ld, %s >= %s\n")
#define ASSERT_GTE_LONG(val1, val2)   COMPARE_COMMON(long, GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %ld >= %ld, but got %ld < %ld, %s >= %s\n")
#define EXPECT_GTE_FLOAT(val1, val2)  COMPARE_COMMON(float, GTE_COMMON, val1, val2, FAIL(), "Expected %f >= %f, but got %f < %f, %s >= %s\n")
#define ASSERT_GTE_FLOAT(val1, val2)  COMPARE_COMMON(float, GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %f >= %f, but got %f < %f, %s >= %s\n")
#define EXPECT_GTE_DOUBLE(val1, val2) COMPARE_COMMON(double, GTE_COMMON, val1, val2, FAIL(), "Expected %lf >= %lf, but got %lf < %lf, %s >= %s\n")
#define ASSERT_GTE_DOUBLE(val1, val2) COMPARE_COMMON(double, GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %lf >= %lf, but got %lf < %lf, %s >= %s\n")
#define EXPECT_GTE_CHAR(val1, val2)   COMPARE_COMMON(char, GTE_COMMON, val1, val2, FAIL(), "Expected %c >= %c, but got %c < %c, %s >= %s\n")
#define ASSERT_GTE_CHAR(val1, val2)   COMPARE_COMMON(char, GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %c >= %c, but got %c < %c, %s >= %s\n")
#define EXPECT_GTE_SHORT(val1, val2)  COMPARE_COMMON(short, GTE_COMMON, val1, val2, FAIL(), "Expected %d >= %d, but got %d < %d, %s >= %s\n")
#define ASSERT_GTE_SHORT(val1, val2)  COMPARE_COMMON(short, GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d >= %d, but got %d < %d, %s >= %s\n")
#define EXPECT_GTE_PTR(val1, val2)    COMPARE_COMMON(void*, GTE_COMMON, val1, val2, FAIL(), "Expected %p >= %p, but got %p < %p, %s >= %s\n")
#define ASSERT_GTE_PTR(val1, val2)    COMPARE_COMMON(void*, GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %p >= %p, but got %p < %p, %s >= %s\n")

// Less-than comparisons
#define EXPECT_LT(val1, val2)         COMPARE_COMMON(int, LT_COMMON, val1, val2, FAIL(), "Expected %d < %d, but got %d >= %d, %s < %s\n")
#define ASSERT_LT(val1, val2)         COMPARE_COMMON(int, LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d < %d, but got %d >= %d, %s < %s\n")
#define EXPECT_LT_LONG(val1, val2)    COMPARE_COMMON(long, LT_COMMON, val1, val2, FAIL(), "Expected %ld < %ld, but got %ld >= %ld, %s < %s\n")
#define ASSERT_LT_LONG(val1, val2)    COMPARE_COMMON(long, LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %ld < %ld, but got %ld >= %ld, %s < %s\n")
#define EXPECT_LT_FLOAT(val1, val2)   COMPARE_COMMON(float, LT_COMMON, val1, val2, FAIL(), "Expected %f < %f, but got %f >= %f, %s < %s\n")
#define ASSERT_LT_FLOAT(val1, val2)   COMPARE_COMMON(float, LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %f < %f, but got %f >= %f, %s < %s\n")
#define EXPECT_LT_DOUBLE(val1, val2)  COMPARE_COMMON(double, LT_COMMON, val1, val2, FAIL(), "Expected %lf < %lf, but got %lf >= %lf, %s < %s\n")
#define ASSERT_LT_DOUBLE(val1, val2)  COMPARE_COMMON(double, LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %lf < %lf, but got %lf >= %lf, %s < %s\n")
#define EXPECT_LT_CHAR(val1, val2)    COMPARE_COMMON(char, LT_COMMON, val1, val2, FAIL(), "Expected %c < %c, but got %c >= %c, %s < %s\n")
#define ASSERT_LT_CHAR(val1, val2)    COMPARE_COMMON(char, LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %c < %c, but got %c >= %c, %s < %s\n")
#define EXPECT_LT_SHORT(val1, val2)   COMPARE_COMMON(short, LT_COMMON, val1, val2, FAIL(), "Expected %d < %d, but got %d >= %d, %s < %s\n")
#define ASSERT_LT_SHORT(val1, val2)   COMPARE_COMMON(short, LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d < %d, but got %d >= %d, %s < %s\n")
#define EXPECT_LT_PTR(val1, val2)     COMPARE_COMMON(void*, LT_COMMON, val1, val2, FAIL(), "Expected %p < %p, but got %p >= %p, %s < %s\n")
#define ASSERT_LT_PTR(val1, val2)     COMPARE_COMMON(void*, LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %p < %p, but got %p >= %p, %s < %s\n")

// Less-than-or-equal comparisons
#define EXPECT_LTE(val1, val2)        COMPARE_COMMON(int, LTE_COMMON, val1, val2, FAIL(), "Expected %d <= %d, but got %d > %d, %s <= %s\n")
#define ASSERT_LTE(val1, val2)        COMPARE_COMMON(int, LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d <= %d, but got %d > %d, %s <= %s\n")
#define EXPECT_LTE_LONG(val1, val2)   COMPARE_COMMON(long, LTE_COMMON, val1, val2, FAIL(), "Expected %ld <= %ld, but got %ld > %ld, %s <= %s\n")
#define ASSERT_LTE_LONG(val1, val2)   COMPARE_COMMON(long, LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %ld <= %ld, but got %ld > %ld, %s <= %s\n")
#define EXPECT_LTE_FLOAT(val1, val2)  COMPARE_COMMON(float, LTE_COMMON, val1, val2, FAIL(), "Expected %f <= %f, but got %f > %f, %s <= %s\n")
#define ASSERT_LTE_FLOAT(val1, val2)  COMPARE_COMMON(float, LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %f <= %f, but got %f > %f, %s <= %s\n")
#define EXPECT_LTE_DOUBLE(val1, val2) COMPARE_COMMON(double, LTE_COMMON, val1, val2, FAIL(), "Expected %lf <= %lf, but got %lf > %lf, %s <= %s\n")
#define ASSERT_LTE_DOUBLE(val1, val2) COMPARE_COMMON(double, LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %lf <= %lf, but got %lf > %lf, %s <= %s\n")
#define EXPECT_LTE_CHAR(val1, val2)   COMPARE_COMMON(char, LTE_COMMON, val1, val2, FAIL(), "Expected %c <= %c, but got %c > %c, %s <= %s\n")
#define ASSERT_LTE_CHAR(val1, val2)   COMPARE_COMMON(char, LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %c <= %c, but got %c > %c, %s <= %s\n")
#define EXPECT_LTE_SHORT(val1, val2)  COMPARE_COMMON(short, LTE_COMMON, val1, val2, FAIL(), "Expected %d <= %d, but got %d > %d, %s <= %s\n")
#define ASSERT_LTE_SHORT(val1, val2)  COMPARE_COMMON(short, LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d <= %d, but got %d > %d, %s <= %s\n")
#define EXPECT_LTE_PTR(val1, val2)    COMPARE_COMMON(void*, LTE_COMMON, val1, val2, FAIL(), "Expected %p <= %p, but got %p > %p, %s <= %s\n")
#define ASSERT_LTE_PTR(val1, val2)    COMPARE_COMMON(void*, LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %p <= %p, but got %p > %p, %s <= %s\n")


int main(void)
{
    int a, b;
    double c, d;
    float e, f;
    long g, h;

    a = 5;
    b = 6;

    c = 5.5;
    d = 6.5;
    EXPECT_EQ(a, b);
    EXPECT_EQ_DOUBLE(c, d);
    ASSERT_LTE_PTR(a,b);
}