#ifndef JANKUNIT_ASSERT_H
#define JANKUNIT_ASSERT_H

#define PASS() \
    do { \
        if      (GLOBAL_CTX->cur_block[TEST] != NULL) { GLOBAL_CTX->cur_block[TEST]->passed++; GLOBAL_CTX->cur_block[TEST]->total++; } \
        else if (GLOBAL_CTX->cur_block[SUITE] != NULL) { GLOBAL_CTX->cur_block[SUITE]->passed++; GLOBAL_CTX->cur_block[SUITE]->total++; } \
        else if (GLOBAL_CTX->cur_block[PROG] != NULL) { GLOBAL_CTX->cur_block[PROG]->passed++; GLOBAL_CTX->cur_block[PROG]->total++; } \
    } while (0)

#define FAIL() \
    do { \
        if      (GLOBAL_CTX->cur_block[TEST] != NULL) { GLOBAL_CTX->cur_block[TEST]->failed++; GLOBAL_CTX->cur_block[TEST]->total++; } \
        else if (GLOBAL_CTX->cur_block[SUITE] != NULL) { GLOBAL_CTX->cur_block[SUITE]->failed++; GLOBAL_CTX->cur_block[SUITE]->total++; } \
        else if (GLOBAL_CTX->cur_block[PROG] != NULL) { GLOBAL_CTX->cur_block[PROG]->failed++; GLOBAL_CTX->cur_block[PROG]->total++; } \
    } while (0)

#define FAIL_ASSERT() \
    do {  \
        if (GLOBAL_CTX->cur_block[TEST] != NULL) { \
            GLOBAL_CTX->cur_block[TEST]->failed++; \
            GLOBAL_CTX->cur_block[TEST]->failed_assert++; \
            GLOBAL_CTX->cur_block[TEST]->total++;   \
        } else if (GLOBAL_CTX->cur_block[SUITE] != NULL) { \
            GLOBAL_CTX->cur_block[SUITE]->failed++; \
            GLOBAL_CTX->cur_block[SUITE]->failed_assert++; \
            GLOBAL_CTX->cur_block[SUITE]->total++;   \
        } else if (GLOBAL_CTX->cur_block[PROG] != NULL) { \
            GLOBAL_CTX->cur_block[PROG]->failed++; \
            GLOBAL_CTX->cur_block[PROG]->failed_assert++; \
            GLOBAL_CTX->cur_block[PROG]->total++;   \
        } \
    } while (0)

#define CONTINUE_IF_FAILED_ASSERT_IN_SCOPE() \
    if (GLOBAL_CTX->cur_block[TEST] != NULL && GLOBAL_CTX->cur_block[TEST]->failed_assert > 0 && (GLOBAL_CTX->cur_block[TEST]->total++, 1))  \
        continue; \
    else if (GLOBAL_CTX->cur_block[SUITE] != NULL && GLOBAL_CTX->cur_block[SUITE]->failed_assert > 0 && (GLOBAL_CTX->cur_block[SUITE]->total++, 1)) \
        continue; \
    else if (GLOBAL_CTX->cur_block[PROG] != NULL && GLOBAL_CTX->cur_block[PROG]->failed_assert > 0 && (GLOBAL_CTX->cur_block[PROG]->total++, 1)) \
        continue

#define FLUSH_AND_REDIRECT_SYS_IF_IN_FORK_AND_USR_OUT_IS_UNFLUSHED()    \
    flush_usr_pipes_and_dup2_sys_pipes(); \
    GLOBAL_CTX->flushed = FLUSHED

// Comparison macros
#define EQ_COMMON   (_a_ == _b_)
#define NEQ_COMMON  (_a_ != _b_)
#define GT_COMMON   (_a_ > _b_)
#define GTE_COMMON  (_a_ >= _b_)
#define LT_COMMON   (_a_ < _b_)
#define LTE_COMMON  (_a_ <= _b_)

// General comparison macro
#define COMPARE_COMMON(type, comp, val1, val2, fail_action, fail_message) \
    do {  \
        FLUSH_AND_REDIRECT_SYS_IF_IN_FORK_AND_USR_OUT_IS_UNFLUSHED();   \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        type _a_ = (val1);  \
        type _b_ = (val2);  \
        if (!(comp)) {  \
            print_with_indent(fail_message, _a_, _b_, _a_, _b_, #val1, #val2, __FILE__, __LINE__);  \
            fail_action;  \
        } else {  \
            PASS(); \
        } \
    } while (0)

// Equality comparisons
#define EXPECT_EQ(val1, val2)         COMPARE_COMMON(int,     EQ_COMMON, val1, val2, FAIL(), "Expected %d == %d, but got %d != %d, (%s == %s). %s:%d\n")
#define EXPECT_EQ_LONG(val1, val2)    COMPARE_COMMON(long,    EQ_COMMON, val1, val2, FAIL(), "Expected %ld == %ld, but got %ld != %ld, (%s == %s). %s:%d\n")
#define EXPECT_EQ_FLOAT(val1, val2)   COMPARE_COMMON(float,   EQ_COMMON, val1, val2, FAIL(), "Expected %f == %f, but got %f != %f, (%s == %s). %s:%d\n")
#define EXPECT_EQ_DOUBLE(val1, val2)  COMPARE_COMMON(double,  EQ_COMMON, val1, val2, FAIL(), "Expected %lf == %lf, but got %lf != %lf, (%s == %s). %s:%d\n")
#define EXPECT_EQ_CHAR(val1, val2)    COMPARE_COMMON(char,    EQ_COMMON, val1, val2, FAIL(), "Expected %c == %c, but got %c != %c, (%s == %s). %s:%d\n")
#define EXPECT_EQ_SHORT(val1, val2)   COMPARE_COMMON(short,   EQ_COMMON, val1, val2, FAIL(), "Expected %hd == %hd, but got %hd != %hd, (%s == %s). %s:%d\n")
#define EXPECT_EQ_PTR(val1, val2)     COMPARE_COMMON(void *,  EQ_COMMON, val1, val2, FAIL(), "Expected %p == %p, but got %p != %p, (%s == %s). %s:%d\n")

#define EXPECT_NEQ(val1, val2)        COMPARE_COMMON(int,     NEQ_COMMON, val1, val2, FAIL(), "Expected %d != %d, but got %d == %d, (%s != %s). %s:%d\n")
#define EXPECT_NEQ_LONG(val1, val2)   COMPARE_COMMON(long,    NEQ_COMMON, val1, val2, FAIL(), "Expected %ld != %ld, but got %ld == %ld, (%s != %s). %s:%d\n")
#define EXPECT_NEQ_FLOAT(val1, val2)  COMPARE_COMMON(float,   NEQ_COMMON, val1, val2, FAIL(), "Expected %f != %f, but got %f == %f, (%s != %s). %s:%d\n")
#define EXPECT_NEQ_DOUBLE(val1, val2) COMPARE_COMMON(double,  NEQ_COMMON, val1, val2, FAIL(), "Expected %lf != %lf, but got %lf == %lf, (%s != %s). %s:%d\n")
#define EXPECT_NEQ_CHAR(val1, val2)   COMPARE_COMMON(char,    NEQ_COMMON, val1, val2, FAIL(), "Expected %c != %c, but got %c == %c, (%s != %s). %s:%d\n")
#define EXPECT_NEQ_SHORT(val1, val2)  COMPARE_COMMON(short,   NEQ_COMMON, val1, val2, FAIL(), "Expected %hd != %hd, but got %hd == %hd, (%s != %s). %s:%d\n")
#define EXPECT_NEQ_PTR(val1, val2)    COMPARE_COMMON(void *,  NEQ_COMMON, val1, val2, FAIL(), "Expected %p != %p, but got %p == %p, (%s != %s). %s:%d\n")

#define EXPECT_LTE(val1, val2)        COMPARE_COMMON(int,     LTE_COMMON, val1, val2, FAIL(), "Expected %d <= %d, but got %d > %d, (%s <= %s). %s:%d\n")
#define EXPECT_LTE_LONG(val1, val2)   COMPARE_COMMON(long,    LTE_COMMON, val1, val2, FAIL(), "Expected %ld <= %ld, but got %ld > %ld, (%s <= %s). %s:%d\n")
#define EXPECT_LTE_FLOAT(val1, val2)  COMPARE_COMMON(float,   LTE_COMMON, val1, val2, FAIL(), "Expected %f <= %f, but got %f > %f, (%s <= %s). %s:%d\n")
#define EXPECT_LTE_DOUBLE(val1, val2) COMPARE_COMMON(double,  LTE_COMMON, val1, val2, FAIL(), "Expected %lf <= %lf, but got %lf > %lf, (%s <= %s). %s:%d\n")
#define EXPECT_LTE_CHAR(val1, val2)   COMPARE_COMMON(char,    LTE_COMMON, val1, val2, FAIL(), "Expected %c <= %c, but got %c > %c, (%s <= %s). %s:%d\n")
#define EXPECT_LTE_SHORT(val1, val2)  COMPARE_COMMON(short,   LTE_COMMON, val1, val2, FAIL(), "Expected %hd <= %hd, but got %hd > %hd, (%s <= %s). %s:%d\n")
#define EXPECT_LTE_PTR(val1, val2)    COMPARE_COMMON(void *,  LTE_COMMON, val1, val2, FAIL(), "Expected %p <= %p, but got %p > %p, (%s <= %s). %s:%d\n")

#define EXPECT_LT(val1, val2)         COMPARE_COMMON(int,     LT_COMMON, val1, val2, FAIL(), "Expected %d < %d, but got %d >= %d, (%s < %s). %s:%d\n")
#define EXPECT_LT_LONG(val1, val2)    COMPARE_COMMON(long,    LT_COMMON, val1, val2, FAIL(), "Expected %ld < %ld, but got %ld >= %ld, (%s < %s). %s:%d\n")
#define EXPECT_LT_FLOAT(val1, val2)   COMPARE_COMMON(float,   LT_COMMON, val1, val2, FAIL(), "Expected %f < %f, but got %f >= %f, (%s < %s). %s:%d\n")
#define EXPECT_LT_DOUBLE(val1, val2)  COMPARE_COMMON(double,  LT_COMMON, val1, val2, FAIL(), "Expected %lf < %lf, but got %lf >= %lf, (%s < %s). %s:%d\n")
#define EXPECT_LT_CHAR(val1, val2)    COMPARE_COMMON(char,    LT_COMMON, val1, val2, FAIL(), "Expected %c < %c, but got %c >= %c, (%s < %s). %s:%d\n")
#define EXPECT_LT_SHORT(val1, val2)   COMPARE_COMMON(short,   LT_COMMON, val1, val2, FAIL(), "Expected %hd < %hd, but got %hd >= %hd, (%s < %s). %s:%d\n")
#define EXPECT_LT_PTR(val1, val2)     COMPARE_COMMON(void *,  LT_COMMON, val1, val2, FAIL(), "Expected %p < %p, but got %p >= %p, (%s < %s). %s:%d\n")

#define EXPECT_GTE(val1, val2)        COMPARE_COMMON(int,     GTE_COMMON, val1, val2, FAIL(), "Expected %d >= %d, but got %d < %d, (%s >= %s). %s:%d\n")
#define EXPECT_GTE_LONG(val1, val2)   COMPARE_COMMON(long,    GTE_COMMON, val1, val2, FAIL(), "Expected %ld >= %ld, but got %ld < %ld, (%s >= %s). %s:%d\n")
#define EXPECT_GTE_FLOAT(val1, val2)  COMPARE_COMMON(float,   GTE_COMMON, val1, val2, FAIL(), "Expected %f >= %f, but got %f < %f, (%s >= %s). %s:%d\n")
#define EXPECT_GTE_DOUBLE(val1, val2) COMPARE_COMMON(double,  GTE_COMMON, val1, val2, FAIL(), "Expected %lf >= %lf, but got %lf < %lf, (%s >= %s). %s:%d\n")
#define EXPECT_GTE_CHAR(val1, val2)   COMPARE_COMMON(char,    GTE_COMMON, val1, val2, FAIL(), "Expected %c >= %c, but got %c < %c, (%s >= %s). %s:%d\n")
#define EXPECT_GTE_SHORT(val1, val2)  COMPARE_COMMON(short,   GTE_COMMON, val1, val2, FAIL(), "Expected %hd >= %hd, but got %hd < %hd, (%s >= %s). %s:%d\n")
#define EXPECT_GTE_PTR(val1, val2)    COMPARE_COMMON(void *,  GTE_COMMON, val1, val2, FAIL(), "Expected %p >= %p, but got %p < %p, (%s >= %s). %s:%d\n")

#define EXPECT_GT(val1, val2)         COMPARE_COMMON(int,     GT_COMMON, val1, val2, FAIL(), "Expected %d > %d, but got %d <= %d, (%s > %s). %s:%d\n")
#define EXPECT_GT_LONG(val1, val2)    COMPARE_COMMON(long,    GT_COMMON, val1, val2, FAIL(), "Expected %ld > %ld, but got %ld <= %ld, (%s > %s). %s:%d\n")
#define EXPECT_GT_FLOAT(val1, val2)   COMPARE_COMMON(float,   GT_COMMON, val1, val2, FAIL(), "Expected %f > %f, but got %f <= %f, (%s > %s). %s:%d\n")
#define EXPECT_GT_DOUBLE(val1, val2)  COMPARE_COMMON(double,  GT_COMMON, val1, val2, FAIL(), "Expected %lf > %lf, but got %lf <= %lf, (%s > %s). %s:%d\n")
#define EXPECT_GT_CHAR(val1, val2)    COMPARE_COMMON(char,    GT_COMMON, val1, val2, FAIL(), "Expected %c > %c, but got %c <= %c, (%s > %s). %s:%d\n")
#define EXPECT_GT_SHORT(val1, val2)   COMPARE_COMMON(short,   GT_COMMON, val1, val2, FAIL(), "Expected %hd > %hd, but got %hd <= %hd, (%s > %s). %s:%d\n")
#define EXPECT_GT_PTR(val1, val2)     COMPARE_COMMON(void *,  GT_COMMON, val1, val2, FAIL(), "Expected %p > %p, but got %p <= %p, (%s > %s). %s:%d\n")


#define ASSERT_EQ(val1, val2)         COMPARE_COMMON(int,     EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d == %d, but got %d != %d, (%s == %s). %s:%d\n")
#define ASSERT_EQ_LONG(val1, val2)    COMPARE_COMMON(long,    EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %ld == %ld, but got %ld != %ld, (%s == %s). %s:%d\n")
#define ASSERT_EQ_FLOAT(val1, val2)   COMPARE_COMMON(float,   EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %f == %f, but got %f != %f, (%s == %s). %s:%d\n")
#define ASSERT_EQ_DOUBLE(val1, val2)  COMPARE_COMMON(double,  EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %lf == %lf, but got %lf != %lf, (%s == %s). %s:%d\n")
#define ASSERT_EQ_CHAR(val1, val2)    COMPARE_COMMON(char,    EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %c == %c, but got %c != %c, (%s == %s). %s:%d\n")
#define ASSERT_EQ_SHORT(val1, val2)   COMPARE_COMMON(short,   EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %hd == %hd, but got %hd != %hd, (%s == %s). %s:%d\n")
#define ASSERT_EQ_PTR(val1, val2)     COMPARE_COMMON(void *,  EQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %p == %p, but got %p != %p, (%s == %s). %s:%d\n")

#define ASSERT_NEQ(val1, val2)        COMPARE_COMMON(int,     NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d != %d, but got %d == %d, (%s != %s). %s:%d\n")
#define ASSERT_NEQ_LONG(val1, val2)   COMPARE_COMMON(long,    NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %ld != %ld, but got %ld == %ld, (%s != %s). %s:%d\n")
#define ASSERT_NEQ_FLOAT(val1, val2)  COMPARE_COMMON(float,   NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %f != %f, but got %f == %f, (%s != %s). %s:%d\n")
#define ASSERT_NEQ_DOUBLE(val1, val2) COMPARE_COMMON(double,  NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %lf != %lf, but got %lf == %lf, (%s != %s). %s:%d\n")
#define ASSERT_NEQ_CHAR(val1, val2)   COMPARE_COMMON(char,    NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %c != %c, but got %c == %c, (%s != %s). %s:%d\n")
#define ASSERT_NEQ_SHORT(val1, val2)  COMPARE_COMMON(short,   NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %hd != %hd, but got %hd == %hd, (%s != %s). %s:%d\n")
#define ASSERT_NEQ_PTR(val1, val2)    COMPARE_COMMON(void *,  NEQ_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %p != %p, but got %p == %p, (%s != %s). %s:%d\n")

#define ASSERT_GT(val1, val2)         COMPARE_COMMON(int,     GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d > %d, but got %d <= %d, (%s > %s). %s:%d\n")
#define ASSERT_GT_LONG(val1, val2)    COMPARE_COMMON(long,    GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %ld > %ld, but got %ld <= %ld, (%s > %s). %s:%d\n")
#define ASSERT_GT_FLOAT(val1, val2)   COMPARE_COMMON(float,   GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %f > %f, but got %f <= %f, (%s > %s). %s:%d\n")
#define ASSERT_GT_DOUBLE(val1, val2)  COMPARE_COMMON(double,  GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %lf > %lf, but got %lf <= %lf, (%s > %s). %s:%d\n")
#define ASSERT_GT_CHAR(val1, val2)    COMPARE_COMMON(char,    GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %c > %c, but got %c <= %c, (%s > %s). %s:%d\n")
#define ASSERT_GT_SHORT(val1, val2)   COMPARE_COMMON(short,   GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %hd > %hd, but got %hd <= %hd, (%s > %s). %s:%d\n")
#define ASSERT_GT_PTR(val1, val2)     COMPARE_COMMON(void *,  GT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %p > %p, but got %p <= %p, (%s > %s). %s:%d\n")

#define ASSERT_GTE(val1, val2)        COMPARE_COMMON(int,     GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d >= %d, but got %d < %d, (%s >= %s). %s:%d\n")
#define ASSERT_GTE_LONG(val1, val2)   COMPARE_COMMON(long,    GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %ld >= %ld, but got %ld < %ld, (%s >= %s). %s:%d\n")
#define ASSERT_GTE_FLOAT(val1, val2)  COMPARE_COMMON(float,   GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %f >= %f, but got %f < %f, (%s >= %s). %s:%d\n")
#define ASSERT_GTE_DOUBLE(val1, val2) COMPARE_COMMON(double,  GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %lf >= %lf, but got %lf < %lf, (%s >= %s). %s:%d\n")
#define ASSERT_GTE_CHAR(val1, val2)   COMPARE_COMMON(char,    GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %c >= %c, but got %c < %c, (%s >= %s). %s:%d\n")
#define ASSERT_GTE_SHORT(val1, val2)  COMPARE_COMMON(short,   GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %hd >= %hd, but got %hd < %hd, (%s >= %s). %s:%d\n")
#define ASSERT_GTE_PTR(val1, val2)    COMPARE_COMMON(void *,  GTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %p >= %p, but got %p < %p, (%s >= %s). %s:%d\n")

#define ASSERT_LT(val1, val2)         COMPARE_COMMON(int,     LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d < %d, but got %d >= %d, (%s < %s). %s:%d\n")
#define ASSERT_LT_LONG(val1, val2)    COMPARE_COMMON(long,    LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %ld < %ld, but got %ld >= %ld, (%s < %s). %s:%d\n")
#define ASSERT_LT_FLOAT(val1, val2)   COMPARE_COMMON(float,   LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %f < %f, but got %f >= %f, (%s < %s). %s:%d\n")
#define ASSERT_LT_DOUBLE(val1, val2)  COMPARE_COMMON(double,  LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %lf < %lf, but got %lf >= %lf, (%s < %s). %s:%d\n")
#define ASSERT_LT_CHAR(val1, val2)    COMPARE_COMMON(char,    LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %c < %c, but got %c >= %c, (%s < %s). %s:%d\n")
#define ASSERT_LT_SHORT(val1, val2)   COMPARE_COMMON(short,   LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %hd < %hd, but got %hd >= %hd, (%s < %s). %s:%d\n")
#define ASSERT_LT_PTR(val1, val2)     COMPARE_COMMON(void *,  LT_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %p < %p, but got %p >= %p, (%s < %s). %s:%d\n")

#define ASSERT_LTE(val1, val2)        COMPARE_COMMON(int,     LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %d <= %d, but got %d > %d, (%s <= %s). %s:%d\n")
#define ASSERT_LTE_LONG(val1, val2)   COMPARE_COMMON(long,    LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %ld <= %ld, but got %ld > %ld, (%s <= %s). %s:%d\n")
#define ASSERT_LTE_FLOAT(val1, val2)  COMPARE_COMMON(float,   LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %f <= %f, but got %f > %f, (%s <= %s). %s:%d\n")
#define ASSERT_LTE_DOUBLE(val1, val2) COMPARE_COMMON(double,  LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %lf <= %lf, but got %lf > %lf, (%s <= %s). %s:%d\n")
#define ASSERT_LTE_CHAR(val1, val2)   COMPARE_COMMON(char,    LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %c <= %c, but got %c > %c, (%s <= %s). %s:%d\n")
#define ASSERT_LTE_SHORT(val1, val2)  COMPARE_COMMON(short,   LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %hd <= %hd, but got %hd > %hd, (%s <= %s). %s:%d\n")
#define ASSERT_LTE_PTR(val1, val2)    COMPARE_COMMON(void *,  LTE_COMMON, val1, val2, FAIL_ASSERT(), "ASSERT failed: Expected %p <= %p, but got %p > %p, (%s <= %s). %s:%d\n")

// String equality comparison (handles NULL pointers)
#define EXPECT_STREQ(str1, str2) \
    do { \
        FLUSH_AND_REDIRECT_SYS_IF_IN_FORK_AND_USR_OUT_IS_UNFLUSHED();   \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        const char* s1 = (str1); \
        const char* s2 = (str2); \
        if (!((s1 == NULL && s2 == NULL) || (s1 != NULL && s2 != NULL && strcmp(s1, s2) == 0))) { \
            print_with_indent("Expected string '%s' to equal '%s', but got different strings: '%s' and '%s', strcmp((%s), (%s)). %s:%d\n", (s1 ? s1 : "NULL"), (s2 ? s2 : "NULL"), (s1 ? s1 : "NULL"), (s2 ? s2 : "NULL"), #str1, #str2, __FILE__, __LINE__); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_STREQ(str1, str2) \
    do { \
        FLUSH_AND_REDIRECT_SYS_IF_IN_FORK_AND_USR_OUT_IS_UNFLUSHED();   \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        const char* s1 = (str1); \
        const char* s2 = (str2); \
        if (!((s1 == NULL && s2 == NULL) || (s1 != NULL && s2 != NULL && strcmp(s1, s2) == 0))) { \
            print_with_indent("ASSERT failed: Expected string '%s' to equal '%s', but got different strings: '%s' and '%s', strcmp((%s), (%s)). %s:%d\n", (s1 ? s1 : "NULL"), (s2 ? s2 : "NULL"), (s1 ? s1 : "NULL"), (s2 ? s2 : "NULL"), #str1, #str2, __FILE__, __LINE__); \
            FAIL_ASSERT(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define COMMON_FORK_OUTPUT_COMPARISON(comp, stream, ...) \
    do {  \
        char *_buf_ = NULL; \
        easprintf(&_buf_, __VA_ARGS__); \
        comp(_buf_, (GLOBAL_CTX->bufs ? GLOBAL_CTX->bufs->bufs[USR][stream] : NULL)); \
        free(_buf_);  \
    } while (0)

#define EXPECT_OUT_EQ(...) COMMON_FORK_OUTPUT_COMPARISON(EXPECT_STREQ, OUT, __VA_ARGS__)
#define EXPECT_ERR_EQ(...) COMMON_FORK_OUTPUT_COMPARISON(EXPECT_STREQ, ERR, __VA_ARGS__)
#define ASSERT_OUT_EQ(...) COMMON_FORK_OUTPUT_COMPARISON(ASSERT_STREQ, OUT, __VA_ARGS__)
#define ASSERT_ERR_EQ(...) COMMON_FORK_OUTPUT_COMPARISON(ASSERT_STREQ, ERR, __VA_ARGS__)

#define EXPECT_EXIT_CODE_EQ(code)   EXPECT_EQ(code, GLOBAL_CTX->EXIT_CODE)
#define EXPECT_SIGNAL_CODE_EQ(code) EXPECT_EQ(code, GLOBAL_CTX->SIGNAL_CODE)
#define ASSERT_EXIT_CODE_EQ(code)   ASSERT_EQ(code, GLOBAL_CTX->EXIT_CODE)
#define ASSERT_SIGNAL_CODE_EQ(code) ASSERT_EQ(code, GLOBAL_CTX->SIGNAL_CODE)

// Boolean assertions
#define EXPECT_TRUE(expr) \
    do { \
        FLUSH_AND_REDIRECT_SYS_IF_IN_FORK_AND_USR_OUT_IS_UNFLUSHED();   \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        if (!(expr)) { \
            print_with_indent("Expected TRUE, but got FALSE, (%s). %s:%d\n", #expr, __FILE__, __LINE__); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_TRUE(expr) \
    do { \
        FLUSH_AND_REDIRECT_SYS_IF_IN_FORK_AND_USR_OUT_IS_UNFLUSHED();   \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        if (!(expr)) { \
            print_with_indent("ASSERT failed: Expected TRUE, but got FALSE, (%s). %s:%d\n", #expr, __FILE__, __LINE__); \
            FAIL_ASSERT(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define EXPECT_FALSE(expr) \
    do { \
        FLUSH_AND_REDIRECT_SYS_IF_IN_FORK_AND_USR_OUT_IS_UNFLUSHED();   \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        if ((expr)) { \
            print_with_indent("Expected FALSE, but got TRUE, (%s). %s:%d\n", #expr, __FILE__, __LINE__); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_FALSE(expr) \
    do { \
        FLUSH_AND_REDIRECT_SYS_IF_IN_FORK_AND_USR_OUT_IS_UNFLUSHED();   \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        if ((expr)) { \
            print_with_indent("ASSERT failed: Expected FALSE, but got TRUE, (%s). %s:%d\n", #expr, __FILE__, __LINE__); \
            FAIL_ASSERT(); \
        } else { \
            PASS(); \
        } \
    } while (0)

/*
create the pipes
create the GLOBAL BUFS (OUT ERR)
create the GLOBAL STATUS

for the function being tested
within the forked block:
    run the assertions 
        ( what if the forked code exits so no assertions are run?
              In that case, the checks provded within the fork scope won't be run,
              and the user won't be notified the true number of total test cases,
              since the check macros (expect/assert_*) all update the counts of their
              scope as they run.

              should I create some logic for providing shmem vars prior to a fork
              and then make all checks be made in the parent?

        )

is mocking print and exit calls the true path perhaps..?

what if assertion fails inside the FORK scope? Does that clear the checks remaning
in the parent after the fork cleanup? 

No, after the fork cleanup, the tests that follow are
immediate to the status/system wide side effects of the fn being run, so the assert_failed status
wont be propagated to the parent, however total assert/expect count will be.
*/
    
#define FORK()  \
    for (pid_t _pid_ = (configure_ctx_pre_fork(), -2);   \
            _pid_ == -2;    \
            GLOBAL_CTX->is_forked = (_pid_ > 0 && (configure_ctx_post_fork(), 0)) )   \
        for (_pid_ = fork(); \
                _pid_ == 0 && (dup2_usr_pipes(), 1) && (atexit(close_all_pipes), 1) && (handle_all_catchable_signals(), 1);    \
                exit(EXIT_SUCCESS) )

#define TEST_PROGRAM(...) \
    for (int _prog_flag_ = (start_test_block(PROG, __VA_ARGS__), 1); \
            _prog_flag_; \
            _prog_flag_ = (end_test_block(PROG, 0), 0) )

#define TEST_SUITE(...) \
    for (int _suite_flag_ = (start_test_block(SUITE, __VA_ARGS__), 1); \
            _suite_flag_; \
            _suite_flag_ = (end_test_block(SUITE, 1), 0) )

#define TEST(...) \
    for (int _test_flag_ = (start_test_block(TEST, __VA_ARGS__), 1); \
            _test_flag_; \
            _test_flag_ = (end_test_block(TEST, 1), 0) )

#endif
