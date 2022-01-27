#include <stdio.h>
#include <stdlib.h>
#include "fixedpoint.h"
#include "tctest.h"

// Test fixture object, has some useful values for testing
typedef struct
{
    Fixedpoint zero;
    Fixedpoint one;
    Fixedpoint one_half;
    Fixedpoint one_fourth;
    Fixedpoint large1;
    Fixedpoint large2;
    Fixedpoint max;

    // TODO: add more objects to the test fixture
    Fixedpoint random1;
    Fixedpoint random2;
    Fixedpoint random3;
} TestObjs;

// functions to create and destroy the test fixture
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// test functions
void test_whole_part(TestObjs *objs);
void test_frac_part(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_is_overflow_pos(TestObjs *objs);
void test_is_err(TestObjs *objs);
// TODO: add more test functions
void test_fixedpoint_create(TestObjs *objs);
void test_fixedpoint_create2(TestObjs *objs);
void test_fixedpoint_whole_part(TestObjs *objs);
void test_fixedpoint_frac_part(TestObjs *objs);
void test_fixedpoint_is_zero(TestObjs *objs);
void test_fixedpoint_create_from_hex(TestObjs *objs);

int main(int argc, char **argv)
{
    // if a testname was specified on the command line, only that
    // test function will be executed
    if (argc > 1)
    {
        tctest_testname_to_execute = argv[1];
    }

    TEST_INIT();

    TEST(test_whole_part);
    TEST(test_frac_part);
    TEST(test_create_from_hex);
    TEST(test_format_as_hex);
    TEST(test_negate);
    TEST(test_add);
    TEST(test_sub);
    TEST(test_is_overflow_pos);
    TEST(test_is_err);

    // IMPORTANT: if you add additional test functions (which you should!),
    // make sure they are included here.  E.g., if you add a test function
    // "my_awesome_tests", you should add
    //
    //   TEST(my_awesome_tests);
    //
    // here. This ensures that your test function will actually be executed.
    TEST(test_fixedpoint_create);
    TEST(test_fixedpoint_create2);
    TEST(test_fixedpoint_whole_part);
    TEST(test_fixedpoint_frac_part);
    TEST(test_fixedpoint_is_zero);
    TEST(test_fixedpoint_create_from_hex);

    TEST_FINI();
}

TestObjs *setup(void)
{
    TestObjs *objs = malloc(sizeof(TestObjs));

    objs->zero = fixedpoint_create(0UL);
    objs->one = fixedpoint_create(1UL);
    objs->one_half = fixedpoint_create2(0UL, 0x8000000000000000UL);
    objs->one_fourth = fixedpoint_create2(0UL, 0x4000000000000000UL);
    objs->large1 = fixedpoint_create2(0x4b19efceaUL, 0xec9a1e2418UL);
    objs->large2 = fixedpoint_create2(0xfcbf3d5UL, 0x4d1a23c24fafUL);
    objs->max = fixedpoint_create2(0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL);

    objs->random1 = fixedpoint_create2(0x85470e39ad685963UL, 0x49164df1f4c49560UL);
    objs->random2 = fixedpoint_create2(0x409c93ac8179c4fdUL, 0x445c9ed1442997a9UL);
    objs->random3 = fixedpoint_create2(0x029d73dc487b2702UL, 0x8e6cefdf1153d8c9UL);
    return objs;
}

void cleanup(TestObjs *objs)
{
    free(objs);
}

void test_whole_part(TestObjs *objs)
{
    ASSERT(0UL == fixedpoint_whole_part(objs->zero));
    ASSERT(1UL == fixedpoint_whole_part(objs->one));
    ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
    ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
    ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
    ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
}

void test_frac_part(TestObjs *objs)
{
    ASSERT(0UL == fixedpoint_frac_part(objs->zero));
    ASSERT(0UL == fixedpoint_frac_part(objs->one));
    ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
    ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
    ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
    ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
}

void test_create_from_hex(TestObjs *objs)
{
    (void)objs;

    Fixedpoint val1 = fixedpoint_create_from_hex("f6a5865.00f2");

    ASSERT(fixedpoint_is_valid(val1));

    ASSERT(0xf6a5865UL == fixedpoint_whole_part(val1));

    ASSERT(0x00f2000000000000UL == fixedpoint_frac_part(val1));
}

void test_format_as_hex(TestObjs *objs)
{
    char *s;

    s = fixedpoint_format_as_hex(objs->zero);
    ASSERT(0 == strcmp(s, "0"));
    free(s);

    s = fixedpoint_format_as_hex(objs->one);
    ASSERT(0 == strcmp(s, "1"));
    free(s);

    s = fixedpoint_format_as_hex(objs->one_half);
    ASSERT(0 == strcmp(s, "0.8"));
    free(s);

    s = fixedpoint_format_as_hex(objs->one_fourth);
    ASSERT(0 == strcmp(s, "0.4"));
    free(s);

    s = fixedpoint_format_as_hex(objs->large1);
    ASSERT(0 == strcmp(s, "4b19efcea.000000ec9a1e2418"));
    free(s);

    s = fixedpoint_format_as_hex(objs->large2);
    ASSERT(0 == strcmp(s, "fcbf3d5.00004d1a23c24faf"));
    free(s);
}

void test_negate(TestObjs *objs)
{
    // none of the test fixture objects are negative
    ASSERT(!fixedpoint_is_neg(objs->zero));
    ASSERT(!fixedpoint_is_neg(objs->one));
    ASSERT(!fixedpoint_is_neg(objs->one_half));
    ASSERT(!fixedpoint_is_neg(objs->one_fourth));
    ASSERT(!fixedpoint_is_neg(objs->large1));
    ASSERT(!fixedpoint_is_neg(objs->large2));

    // negate the test fixture values
    Fixedpoint zero_neg = fixedpoint_negate(objs->zero);
    Fixedpoint one_neg = fixedpoint_negate(objs->one);
    Fixedpoint one_half_neg = fixedpoint_negate(objs->one_half);
    Fixedpoint one_fourth_neg = fixedpoint_negate(objs->one_fourth);
    Fixedpoint large1_neg = fixedpoint_negate(objs->large1);
    Fixedpoint large2_neg = fixedpoint_negate(objs->large2);

    // zero does not become negative when negated
    ASSERT(!fixedpoint_is_neg(zero_neg));

    // all of the other values should have become negative when negated
    ASSERT(fixedpoint_is_neg(one_neg));
    ASSERT(fixedpoint_is_neg(one_half_neg));
    ASSERT(fixedpoint_is_neg(one_fourth_neg));
    ASSERT(fixedpoint_is_neg(large1_neg));
    ASSERT(fixedpoint_is_neg(large2_neg));

    // magnitudes should stay the same
    ASSERT(0UL == fixedpoint_whole_part(objs->zero));
    ASSERT(1UL == fixedpoint_whole_part(objs->one));
    ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
    ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
    ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
    ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
    ASSERT(0UL == fixedpoint_frac_part(objs->zero));
    ASSERT(0UL == fixedpoint_frac_part(objs->one));
    ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
    ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
    ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
    ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
}

void test_add(TestObjs *objs)
{
    (void)objs;

    Fixedpoint lhs, rhs, sum;

    lhs = fixedpoint_create_from_hex("-c7252a193ae07.7a51de9ea0538c5");
    rhs = fixedpoint_create_from_hex("d09079.1e6d601");
    sum = fixedpoint_add(lhs, rhs);
    ASSERT(fixedpoint_is_neg(sum));
    ASSERT(0xc7252a0c31d8eUL == fixedpoint_whole_part(sum));
    ASSERT(0x5be47e8ea0538c50UL == fixedpoint_frac_part(sum));
}

void test_sub(TestObjs *objs)
{
    (void)objs;

    Fixedpoint lhs, rhs, diff;

    lhs = fixedpoint_create_from_hex("-ccf35aa3a04a3b.b105");
    rhs = fixedpoint_create_from_hex("f676e8.58");
    diff = fixedpoint_sub(lhs, rhs);
    ASSERT(fixedpoint_is_neg(diff));
    ASSERT(0xccf35aa496c124UL == fixedpoint_whole_part(diff));
    ASSERT(0x0905000000000000UL == fixedpoint_frac_part(diff));
}

void test_is_overflow_pos(TestObjs *objs)
{
    Fixedpoint sum;

    sum = fixedpoint_add(objs->max, objs->one);
    ASSERT(fixedpoint_is_overflow_pos(sum));

    sum = fixedpoint_add(objs->one, objs->max);
    ASSERT(fixedpoint_is_overflow_pos(sum));

    Fixedpoint negative_one = fixedpoint_negate(objs->one);

    sum = fixedpoint_sub(objs->max, negative_one);
    ASSERT(fixedpoint_is_overflow_pos(sum));
}

void test_is_err(TestObjs *objs)
{
    (void)objs;

    // too many characters
    Fixedpoint err1 = fixedpoint_create_from_hex("88888888888888889.6666666666666666");
    ASSERT(fixedpoint_is_err(err1));

    // too many characters
    Fixedpoint err2 = fixedpoint_create_from_hex("6666666666666666.88888888888888889");
    ASSERT(fixedpoint_is_err(err2));

    // this one is actually fine
    Fixedpoint err3 = fixedpoint_create_from_hex("-6666666666666666.8888888888888888");
    ASSERT(fixedpoint_is_valid(err3));
    ASSERT(!fixedpoint_is_err(err3));

    // whole part is too large
    Fixedpoint err4 = fixedpoint_create_from_hex("88888888888888889");
    ASSERT(fixedpoint_is_err(err4));

    // fractional part is too large
    Fixedpoint err5 = fixedpoint_create_from_hex("7.88888888888888889");
    ASSERT(fixedpoint_is_err(err5));

    // invalid hex digits in whole part
    Fixedpoint err6 = fixedpoint_create_from_hex("123xabc.4");
    ASSERT(fixedpoint_is_err(err6));

    // invalid hex digits in fractional part
    Fixedpoint err7 = fixedpoint_create_from_hex("7.0?4");
    ASSERT(fixedpoint_is_err(err7));
}

// TODO: implement more test functions

// Test the fixedpoint_create function
void test_fixedpoint_create(TestObjs *objs)
{
    (void)objs;
    // Test with argument 0
    Fixedpoint test1 = fixedpoint_create(0UL);
    ASSERT(test1.whole == 0UL);
    ASSERT(test1.frac == 0UL);
    ASSERT(test1.tag == VALID_NONNEGATIVE);

    // Test with random positive argument
    Fixedpoint test2 = fixedpoint_create(17532694UL);
    ASSERT(test2.whole == 17532694UL);
    ASSERT(test2.frac == 0UL);
    ASSERT(test2.tag == VALID_NONNEGATIVE);
}

// Test the fixedpoint_create2 function
void test_fixedpoint_create2(TestObjs *objs)
{
    (void)objs;
    // Test with two zero argument
    Fixedpoint test1 = fixedpoint_create2(0UL, 0UL);
    ASSERT(test1.whole == 0UL);
    ASSERT(test1.frac == 0UL);
    ASSERT(test1.tag == VALID_NONNEGATIVE);

    // Test with positive argument for whole part, but zero for the fractional part
    Fixedpoint test2 = fixedpoint_create2(5437923400UL, 0UL);
    ASSERT(test2.whole == 5437923400UL);
    ASSERT(test2.frac == 0UL);
    ASSERT(test2.tag == VALID_NONNEGATIVE);

    // Test with zero for the whole part, but a postive value for the fractional part
    Fixedpoint test3 = fixedpoint_create2(0UL, 2375984UL);
    ASSERT(test3.whole == 0UL);
    ASSERT(test3.frac == 2375984UL);
    ASSERT(test3.tag == VALID_NONNEGATIVE);

    // Test with positive values for both parts
    Fixedpoint test4 = fixedpoint_create2(4582UL, 824UL);
    ASSERT(test4.whole == 4582UL);
    ASSERT(test4.frac == 824UL);
    ASSERT(test4.tag == VALID_NONNEGATIVE);
}

// Additional tests for the fixedpoint_whole_part function
void test_fixedpoint_whole_part(TestObjs *objs)
{
    ASSERT(0x85470e39ad685963UL == fixedpoint_whole_part(objs->random1));
    ASSERT(0x409c93ac8179c4fdUL == fixedpoint_whole_part(objs->random2));
    ASSERT(0x029d73dc487b2702UL == fixedpoint_whole_part(objs->random3));
}

// Additional tests for the fixedpoint_frac_part function
void test_fixedpoint_frac_part(TestObjs *objs)
{
    ASSERT(0x49164df1f4c49560UL == fixedpoint_frac_part(objs->random1));
    ASSERT(0x445c9ed1442997a9UL == fixedpoint_frac_part(objs->random2));
    ASSERT(0x8e6cefdf1153d8c9UL == fixedpoint_frac_part(objs->random3));
}

// Test the fixedpoint_is_zero function
void test_fixedpoint_is_zero(TestObjs *objs)
{
    // Is Zero
    ASSERT(fixedpoint_is_zero(objs->zero));

    // Is Not Zero
    ASSERT(!fixedpoint_is_zero(objs->one));
    ASSERT(!fixedpoint_is_zero(objs->one_half));
    ASSERT(!fixedpoint_is_zero(objs->one_fourth));
    ASSERT(!fixedpoint_is_zero(objs->large1));
    ASSERT(!fixedpoint_is_zero(objs->large2));
    ASSERT(!fixedpoint_is_zero(objs->max));
    ASSERT(!fixedpoint_is_zero(objs->random1));
    ASSERT(!fixedpoint_is_zero(objs->random2));
    ASSERT(!fixedpoint_is_zero(objs->random3));
}

// Additional tests for fixedpoint_format_as_hex hex strings
void test_fixedpoint_create_from_hex(TestObjs *objs)
{
    (void)objs;
    // Valid Hex Strings 
    // Format: X all lower
    Fixedpoint test1 = fixedpoint_create_from_hex("35d40bfd131c245d");
    ASSERT(test1.whole == 0x35d40bfd131c245dUL);
    ASSERT(test1.frac == 0UL);
    ASSERT(test1.tag == VALID_NONNEGATIVE);

    // Format: X all upper
    Fixedpoint test2 = fixedpoint_create_from_hex("0346BfBABF7417A0");
    ASSERT(test2.whole == 0x0346bfbabf7417a0UL);
    ASSERT(test2.frac == 0UL);
    ASSERT(test2.tag == VALID_NONNEGATIVE);

    // Format: X mixed upper and lower
    Fixedpoint test3 = fixedpoint_create_from_hex("3Adf1Af390C74fE9");
    ASSERT(test3.whole == 0x3adf1af390c74fe9);
    ASSERT(test3.frac == 0UL);
    ASSERT(test3.tag == VALID_NONNEGATIVE);

    // Format: -X all lower
    Fixedpoint test4 = fixedpoint_create_from_hex("-52969e49d9bcd9fc");
    ASSERT(test4.whole == 0x52969e49d9bcd9fcUL);
    ASSERT(test4.frac == 0UL);
    ASSERT(test4.tag == VALID_NEGATIVE);

    // Format: -X all upper
    Fixedpoint test5 = fixedpoint_create_from_hex("-1007DBA8E9BD4DF0");
    ASSERT(test5.whole == 0x1007dba8e9bd4df0UL);
    ASSERT(test5.frac == 0UL);
    ASSERT(test5.tag == VALID_NEGATIVE);

    // Format: -X mixed upper and lower
    Fixedpoint test6 = fixedpoint_create_from_hex("-DbCa99C0f5EcDeBd");
    ASSERT(test6.whole == 0xdbca99c0f5ecdebdUL);
    ASSERT(test6.frac == 0UL);
    ASSERT(test6.tag == VALID_NEGATIVE);

    // Format: X.Y all lower, full length
    Fixedpoint test7 = fixedpoint_create_from_hex("86e212bc97e8bdae.31afa78676593a80");
    ASSERT(test7.whole == 0x86e212bc97e8bdaeUL);
    ASSERT(test7.frac == 0x31afa78676593a80UL);
    ASSERT(test7.tag == VALID_NONNEGATIVE);

    // Format: X.Y all lower, non full length
    Fixedpoint test8 = fixedpoint_create_from_hex("5f.4761d4081f2c61");
}
