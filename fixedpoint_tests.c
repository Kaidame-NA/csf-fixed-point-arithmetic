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

    Fixedpoint noerror;
    Fixedpoint format_error;
    Fixedpoint overflow_positive;
    Fixedpoint overflow_negative;
    Fixedpoint underflow_positive;
    Fixedpoint underflow_negative;
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
void test_fixedpoint_negate(TestObjs *objs);
void test_fixedpoint_add(TestObjs *objs);
void test_fixedpoint_sub(TestObjs *objs);
void test_fixedpoint_halve(TestObjs *objs);
void test_fixedpoint_double(TestObjs *objs);
void test_fixedpoint_compare(TestObjs *objs);
void test_fixedpoint_is_err(TestObjs *objs);
void test_fixedpoint_is_neg(TestObjs *objs);
void test_fixedpoint_is_overflow_neg(TestObjs *objs);
void test_fixedpoint_is_overflow_pos(TestObjs *objs);
void test_fixedpoint_is_underflow_neg(TestObjs *objs);
void test_fixedpoint_is_underflow_pos(TestObjs *objs);
void test_fixedpoint_is_valid(TestObjs *objs);
void test_fixedpoint_format_as_hex(TestObjs *objs);

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
    TEST(test_fixedpoint_negate);
    TEST(test_fixedpoint_add);
    TEST(test_fixedpoint_sub);
    TEST(test_fixedpoint_halve);
    TEST(test_fixedpoint_double);
    TEST(test_fixedpoint_compare);
    TEST(test_fixedpoint_is_err);
    TEST(test_fixedpoint_is_neg);
    TEST(test_fixedpoint_is_overflow_neg);
    TEST(test_fixedpoint_is_overflow_pos);
    TEST(test_fixedpoint_is_underflow_neg);
    TEST(test_fixedpoint_is_underflow_pos);

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

    objs->noerror = fixedpoint_create_from_hex("a45f4e3f0a4875.4e9");
    objs->format_error = fixedpoint_create_from_hex("00fsadfka.kerjls-");
    objs->overflow_positive = fixedpoint_add(objs->max, objs->max);
    objs->overflow_negative = fixedpoint_double(fixedpoint_negate(objs->max));
    objs->underflow_positive = fixedpoint_halve(fixedpoint_create_from_hex("0.1111111111111111"));
    objs->underflow_negative = fixedpoint_halve(fixedpoint_create_from_hex("-0.0000000000000001"));
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
    ASSERT(test8.whole == 0x5fUL);
    ASSERT(test8.frac = 0x4761d4081f2c6100UL);
    ASSERT(test8.tag == VALID_NONNEGATIVE);

    // Format: -X.Y all lower, full length
    Fixedpoint test9 = fixedpoint_create_from_hex("-cfe39e37ffd3df69.7dfb9a356ec9c702");
    ASSERT(test9.whole == 0xcfe39e37ffd3df69UL);
    ASSERT(test9.frac == 0x7dfb9a356ec9c702UL);
    ASSERT(test9.tag == VALID_NEGATIVE);

    // Format: -X.Y all lower, non full length
    Fixedpoint test10 = fixedpoint_create_from_hex("-c80c5245c81554e.fe5b80f41");
    ASSERT(test10.whole == 0xc80c5245c81554eUL);
    ASSERT(test10.frac == 0xfe5b80f410000000UL);
    ASSERT(test10.tag == VALID_NEGATIVE);

    // Format: 0
    Fixedpoint test11 = fixedpoint_create_from_hex("0");
    ASSERT(test11.whole == 0UL);
    ASSERT(test11.frac == 0UL);
    ASSERT(test11.tag == VALID_NONNEGATIVE);

    // Format: -0
    Fixedpoint test12 = fixedpoint_create_from_hex("-0");
    ASSERT(test12.whole == 0UL);
    ASSERT(test12.frac == 0UL);
    ASSERT(test12.tag == VALID_NONNEGATIVE);

    // Format: X.
    Fixedpoint test13 = fixedpoint_create_from_hex("5ba9f.");
    ASSERT(test13.whole == 0x5ba9fUL);
    ASSERT(test12.frac == 0UL);
    ASSERT(test13.tag == VALID_NONNEGATIVE);

    // Format: .Y
    Fixedpoint test14 = fixedpoint_create_from_hex(".cda133");
    ASSERT(test14.whole == 0UL);
    ASSERT(test14.frac == 0xcda1330000000000UL);
    ASSERT(test14.tag == VALID_NONNEGATIVE);

    // Format: -X.
    Fixedpoint test15 = fixedpoint_create_from_hex("-4eff75dd.");
    ASSERT(test15.whole == 0x4eff75ddUL);
    ASSERT(test15.frac == 0UL);
    ASSERT(test15.tag == VALID_NEGATIVE);

    // Format : -.Y
    Fixedpoint test16 = fixedpoint_create_from_hex("-.0ed");
    ASSERT(test16.whole == 0UL);
    ASSERT(test16.frac == 0x0ed0000000000000UL);
    ASSERT(test16.tag == VALID_NEGATIVE);

    // Two negative signs
    Fixedpoint test17 = fixedpoint_create_from_hex("-0a-fb");
    ASSERT(test17.tag == ERROR);

    // Negative sign in wrong place
    Fixedpoint test18 = fixedpoint_create_from_hex("0.b7-a");
    ASSERT(test18.tag == ERROR);

    // Two decimal points
    Fixedpoint test19 = fixedpoint_create_from_hex("108.1d43f9.0c5572f");
    ASSERT(test19.tag == ERROR);

    // X.Y too long
    Fixedpoint test20 = fixedpoint_create_from_hex("94aa7a8bc03118f50.94aa7a8bc03118f05");
    ASSERT(test20.tag == ERROR);

    // X.Y, X too long
    Fixedpoint test21 = fixedpoint_create_from_hex("1fab80b1e8e3ccce0.abcdef");
    ASSERT(test21.tag == ERROR);

    // X.Y, Y too long
    Fixedpoint test22 = fixedpoint_create_from_hex("0.2e36b5f14a0bde3c1");
    ASSERT(test22.tag == ERROR);

    // X, X too long
    Fixedpoint test23 = fixedpoint_create_from_hex("5580a175a6f94a391");
    ASSERT(test23.tag == ERROR);

    // Nonsense characters
    Fixedpoint test24 = fixedpoint_create_from_hex("130284yhufdjaads");
    ASSERT(test24.tag == ERROR);

    Fixedpoint test25 = fixedpoint_create_from_hex("5a 75 0a 35. cc b6 28 09 ");
    ASSERT(test25.tag == ERROR);
}

// Additional tests for fixedpoint_negate;
void test_fixedpoint_negate(TestObjs *objs)
{
    (void)objs;
    // X -> -X
    Fixedpoint test1 = fixedpoint_create_from_hex("b4adb9d9d7e644c5");
    Fixedpoint test1negate = fixedpoint_negate(test1);
    ASSERT(test1negate.tag == VALID_NEGATIVE);
    ASSERT(test1.whole == test1negate.whole);
    ASSERT(test1.frac == test1negate.frac);

    // X.Y -> -X.Y
    Fixedpoint test2 = fixedpoint_create_from_hex("cd049a3b5dc1b337.faef5a1039f76c76");
    Fixedpoint test2negate = fixedpoint_negate(test2);
    ASSERT(test2negate.tag == VALID_NEGATIVE);
    ASSERT(test2.whole == test2negate.whole);
    ASSERT(test2.frac == test2negate.frac);

    // -X -> X
    Fixedpoint test3 = fixedpoint_create_from_hex("-48599cc4c4c64660");
    Fixedpoint test3negate = fixedpoint_negate(test3);
    ASSERT(test3negate.tag == VALID_NONNEGATIVE);
    ASSERT(test3.whole == test3negate.whole);
    ASSERT(test3.frac == test3negate.frac);

    // -X.Y -> X.Y
    Fixedpoint test4 = fixedpoint_create_from_hex("-4ea49caad95efe00.fbe77b8e0f1b82cd");
    Fixedpoint test4negate = fixedpoint_negate(test4);
    ASSERT(test4negate.tag == VALID_NONNEGATIVE);
    ASSERT(test4.whole == test4negate.whole);
    ASSERT(test4.frac == test4negate.frac);

    // 0
    Fixedpoint test5 = fixedpoint_create_from_hex("0");
    Fixedpoint test5negate = fixedpoint_negate(test5);
    ASSERT(test5negate.tag == VALID_NONNEGATIVE);
    ASSERT(test5.whole == test5negate.whole);
    ASSERT(test5.frac == test5negate.frac);
}

void test_fixedpoint_add(TestObjs *objs)
{
    // Valid A + B, A, B nonneg
    Fixedpoint test1A = fixedpoint_create_from_hex("d669c.108");
    Fixedpoint test1B = fixedpoint_create_from_hex("510ce1e942a3c19.430fc51");
    Fixedpoint test1sum = fixedpoint_add(test1A, test1B);

    ASSERT(test1sum.tag == VALID_NONNEGATIVE);
    ASSERT(test1sum.whole == 0x510ce1e9437a2b5UL);
    ASSERT(test1sum.frac == 0x538fc51000000000UL);

    // Valid A+B, A, B nonneg carry
    Fixedpoint test2A = fixedpoint_create_from_hex("72c.7e2210b");
    Fixedpoint test2B = fixedpoint_create_from_hex("d480cf0fa6.a62ee52");
    Fixedpoint test2sum = fixedpoint_add(test2A, test2B);
    ASSERT(test2sum.tag == VALID_NONNEGATIVE);
    ASSERT(test2sum.whole == 0xd480cf16d3UL);
    ASSERT(test2sum.frac == 0x2450f5d000000000UL);

    // Overflow A+B, A, B nonneg
    Fixedpoint test3sum = fixedpoint_add(objs->max, objs->one);
    ASSERT(test3sum.tag == OVERFLOW_POSITIVE);

    // Valid A + B, A, B neg
    Fixedpoint test4A = fixedpoint_create_from_hex("-82d3905e2.3748bc8301ee08a");
    Fixedpoint test4B = fixedpoint_create_from_hex("-3.ab8b0e6a4a79");
    Fixedpoint test4sum = fixedpoint_add(test4A, test4B);
    ASSERT(test4sum.tag == VALID_NEGATIVE);
    ASSERT(test4sum.whole == 0x82d3905e5UL);
    ASSERT(test4sum.frac == 0xe2d3caed4c6708a0UL);

    // Valid A + B, A, B neg carry
    Fixedpoint test5A = fixedpoint_create_from_hex("-6f.adde49718d50f");
    Fixedpoint test5B = fixedpoint_create_from_hex("-0c8890.829fe8598cbb9");
    Fixedpoint test5sum = fixedpoint_add(test5A, test5B);
    ASSERT(test5sum.tag == VALID_NEGATIVE);
    ASSERT(test5sum.whole == 0xc8900UL);
    ASSERT(test5sum.frac == 0x307e31cb1a0c8000);

    // Overflow A + B, A, B neg
    Fixedpoint test6A = fixedpoint_negate(objs->max);
    Fixedpoint test6B = fixedpoint_negate(objs->one);
    Fixedpoint test6sum = fixedpoint_add(test6A, test6B);
    ASSERT(test6sum.tag == OVERFLOW_NEGATIVE);

    // Valid A + B, A nonneg, B neg, neg result
    Fixedpoint test7A = fixedpoint_create_from_hex("32df5b1458.d1792dfe63f5");
    Fixedpoint test7B = fixedpoint_create_from_hex("-0bd92be903550f.7b5a389f8ef6");
    Fixedpoint test7sum = fixedpoint_add(test7A, test7B);
    ASSERT(test7sum.tag == VALID_NEGATIVE);
    ASSERT(test7sum.whole == 0xbd8f909a840b6UL);
    ASSERT(test7sum.frac == 0xa9e10aa12b010000UL);

    // Valid A + B, A nonneg, B neg, nonneg result
    Fixedpoint test8A = fixedpoint_create_from_hex("71ffefb2e34.c21af83e");
    Fixedpoint test8B = fixedpoint_create_from_hex("-9ae395aa2d.c");
    Fixedpoint test8sum = fixedpoint_add(test8A, test8B);
    ASSERT(test8sum.tag == VALID_NONNEGATIVE);
    ASSERT(test8sum.whole == 0x6851b658407UL);
    ASSERT(test8sum.frac == 0x021af83e00000000UL);

    // Valid A + B, A neg, B nonneg, neg result
    Fixedpoint test9A = fixedpoint_create_from_hex("-e12dc7fe1b74cdc.62f5a5824e8");
    Fixedpoint test9B = fixedpoint_create_from_hex("8738a364a726e.348e9703");
    Fixedpoint test9sum = fixedpoint_add(test9A, test9B);
    ASSERT(test9sum.tag == VALID_NEGATIVE);
    ASSERT(test9sum.whole == 0xe0a68f5ab6cda6eUL);
    ASSERT(test9sum.frac == 0x2e670e7f4e800000UL);

    // Valid A + B, A neg, B nonneg, nonnneg result
    Fixedpoint test10A = fixedpoint_create_from_hex("-dfc94da.cce");
    Fixedpoint test10B = fixedpoint_create_from_hex("1d4e4700.0");
    Fixedpoint test10sum = fixedpoint_add(test10A, test10B);
    ASSERT(test10sum.tag == VALID_NONNEGATIVE);
    ASSERT(test10sum.whole == 0xf51b225UL);
    ASSERT(test10sum.frac == 0x3320000000000000UL);
}

// Additional tests for fixedpoint subtraction
void test_fixedpoint_sub(TestObjs *objs)
{
    // Valid A - B, A nonneg, B nonneg, nonneg result
    Fixedpoint test1A = fixedpoint_create_from_hex("65f4dd17c108b6.32ed");
    Fixedpoint test1B = fixedpoint_create_from_hex("4d30ee90b.82dbe48c3fe1");
    Fixedpoint test1diff = fixedpoint_sub(test1A, test1B);
    ASSERT(test1diff.tag == VALID_NONNEGATIVE);
    ASSERT(test1diff.whole == 0x65f4d844b21faaUL);
    ASSERT(test1diff.frac == 0xb0111b73c01f0000UL);

    // Valid A - B, A nonneg, B nonneg, neg result
    Fixedpoint test2A = fixedpoint_create_from_hex("461053.a");
    Fixedpoint test2B = fixedpoint_create_from_hex("c3667eb.645a5b");
    Fixedpoint test2diff = fixedpoint_sub(test2A, test2B);
    ASSERT(test2diff.tag == VALID_NEGATIVE);
    ASSERT(test2diff.whole == 0xbf05797UL);
    ASSERT(test2diff.frac == 0xc45a5b0000000000UL);

    // Valid A - B, A = B, zero result
    Fixedpoint test3diff = fixedpoint_sub(objs->random1, objs->random1);
    ASSERT(fixedpoint_is_zero(test3diff));
    ASSERT(test3diff.tag == VALID_NONNEGATIVE);

    // Valid A - B, A neg, B neg, nonneg result
    Fixedpoint test4A = fixedpoint_create_from_hex("-316c1832a1.e844428906968");
    Fixedpoint test4B = fixedpoint_create_from_hex("-49bf5e3b79bf17.7777d1e8f");
    Fixedpoint test4diff = fixedpoint_sub(test4A, test4B);
    ASSERT(test4diff.tag == VALID_NONNEGATIVE);
    ASSERT(test4diff.whole == 0x49bf2ccf618c75UL);
    ASSERT(test4diff.frac == 0x8f338f5fe9698000UL);

    // Valid A - B, A neg, B neg, neg result
    Fixedpoint test5A = fixedpoint_create_from_hex("-3f8d2494a9.5ac545d");
    Fixedpoint test5B = fixedpoint_create_from_hex("-27c137f.7a0affc8ff");
    Fixedpoint test5diff = fixedpoint_sub(test5A, test5B);
    ASSERT(test5diff.tag == VALID_NEGATIVE);
    ASSERT(test5diff.whole == 0x3f8aa88129UL);
    ASSERT(test5diff.frac == 0xe0ba460701000000UL);

    // Valid A - B, A neg, B nonneg, neg result
    Fixedpoint test6A = fixedpoint_create_from_hex("-a9983fa9.1d2");
    Fixedpoint test6B = fixedpoint_create_from_hex("9aec7246cb8.e8552c0");
    Fixedpoint test6diff = fixedpoint_sub(test6A, test6B);
    ASSERT(test6diff.tag == VALID_NEGATIVE);
    ASSERT(test6diff.whole == 0x9af70bcac62UL);
    ASSERT(test6diff.frac == 0x05752c0000000000UL);

    // Negative overflow A - B, A neg, B nonneg
    Fixedpoint test7A = fixedpoint_negate(objs->max);
    Fixedpoint test7B = objs->one;
    Fixedpoint test7diff = fixedpoint_sub(test7A, test7B);
    ASSERT(test7diff.tag == OVERFLOW_NEGATIVE);

    // Valid A - B, A pos, B neg, nonneg result
    Fixedpoint test8A = fixedpoint_create_from_hex("ba011d3720f.0f9273");
    Fixedpoint test8B = fixedpoint_create_from_hex("-beae87e117.2ab0d237691b7b2");
    Fixedpoint test8diff = fixedpoint_sub(test8A, test8B);
    ASSERT(test8diff.tag == VALID_NONNEGATIVE);
    ASSERT(test8diff.whole == 0xc5ec05b5326UL);
    ASSERT(test8diff.frac == 0x3a434537691b7b20UL);

    // Positive overflow A - B, A pos, B neg
    Fixedpoint test9A = objs->max;
    Fixedpoint test9B = fixedpoint_negate(objs->one);
    Fixedpoint test9diff = fixedpoint_sub(test9A, test9B);
    ASSERT(test9diff.tag == OVERFLOW_POSITIVE);
}

void test_fixedpoint_halve(TestObjs *objs)
{
    // Valid X -> X/2, x nonneg
    Fixedpoint test1 = fixedpoint_halve(fixedpoint_create_from_hex("e98a46c7860.d6"));
    ASSERT(test1.tag == VALID_NONNEGATIVE);
    ASSERT(test1.whole == 0x074c52363c30UL);
    ASSERT(test1.frac == 0x6b00000000000000UL);

    // Valid X -> X/2, x nonneg, whole part is odd
    Fixedpoint test2 = fixedpoint_halve(fixedpoint_create_from_hex("13008cc93.f0c5f79138aa528"));
    ASSERT(test2.tag == VALID_NONNEGATIVE);
    ASSERT(test2.whole == 0x98046649UL);
    ASSERT(test2.frac == 0xf862fbc89c552940UL);

    // X -> X/2, x nonneg, underflow
    Fixedpoint test3 = fixedpoint_halve(fixedpoint_create_from_hex("0.1111111111111111"));
    ASSERT(test3.tag == UNDERFLOW_POSITIVE);

    // Valid X -> X/2, x neg
    Fixedpoint test4 = fixedpoint_halve(fixedpoint_create_from_hex("-d95a7652bf78.d9257b920d74"));
    ASSERT(test4.tag == VALID_NEGATIVE);
    ASSERT(test4.whole == 0x6cad3b295fbcUL);
    ASSERT(test4.frac == 0x6c92bdc906ba0000UL);

    // Valid X -> X/2, x neg, whole part is odd
    Fixedpoint test5 = fixedpoint_halve(fixedpoint_create_from_hex("-691921ef4b8ba03.7c"));
    ASSERT(test5.tag == VALID_NEGATIVE);
    ASSERT(test5.whole == 0x348c90f7a5c5d01UL);
    ASSERT(test5.frac == 0xbe00000000000000UL);

    // X -> X/2, x neg, underflow
    Fixedpoint test6 = fixedpoint_halve(fixedpoint_create_from_hex("-0.1111111111111111"));
    ASSERT(test6.tag == UNDERFLOW_NEGATIVE);

    // X -> X/2, x = 0
    Fixedpoint test7 = fixedpoint_halve(objs->zero);
    ASSERT(test7.tag == VALID_NONNEGATIVE);
    ASSERT(test7.whole == 0UL);
    ASSERT(test7.frac == 0UL);
}

void test_fixedpoint_double(TestObjs *objs)
{
    // Valid X -> 2X, X is nonneg
    Fixedpoint test1 = fixedpoint_double(fixedpoint_create_from_hex("a5213d2e.8192012758576"));
    ASSERT(test1.tag == VALID_NONNEGATIVE);
    ASSERT(test1.whole == 0x14a427a5dUL);
    ASSERT(test1.frac == 0x0324024eb0aec000UL);

    // Overflow X -> 2X, X is nonneg
    Fixedpoint test2 = fixedpoint_double(objs->max);
    ASSERT(test2.tag == OVERFLOW_POSITIVE);

    // Valid X -> 2X, X is neg
    Fixedpoint test3 = fixedpoint_double(fixedpoint_create_from_hex("-4072ac5f09d.11467503"));
    ASSERT(test3.tag == VALID_NEGATIVE);
    ASSERT(test3.whole == 0x80e558be13aUL);
    ASSERT(test3.frac == 0x228cea0600000000UL);

    // Overflow X -> 2X, X is neg
    Fixedpoint test4 = fixedpoint_double(fixedpoint_negate(objs->max));
    ASSERT(test4.tag == OVERFLOW_NEGATIVE);
}

void test_fixedpoint_compare(TestObjs *objs)
{
    (void)objs;
    // A > B, A nonneg, B nonneg
    Fixedpoint test1A = fixedpoint_create_from_hex("101.10");
    Fixedpoint test1B = fixedpoint_create_from_hex("101.001");
    ASSERT(fixedpoint_compare(test1A, test1B) == 1);

    // A = B, A nonneg, B nonneg
    Fixedpoint test2A = fixedpoint_create_from_hex("1124.134");
    Fixedpoint test2B = fixedpoint_create_from_hex("00000001124.13400");
    ASSERT(fixedpoint_compare(test2A, test2B) == 0);

    // A < B, A nonneg, B nonneg
    Fixedpoint test3A = fixedpoint_create_from_hex("9217f12c4d2333.0515047f39a07");
    Fixedpoint test3B = fixedpoint_create_from_hex("9217f12c4d2343.0515047f39a07");
    ASSERT(fixedpoint_compare(test3A, test3B) == -1);

    // A > B, A nonneg, B neg
    Fixedpoint test4A = fixedpoint_create_from_hex("3cf6c445.827b10946ce00");
    Fixedpoint test4B = fixedpoint_create_from_hex("-6.1291e");
    ASSERT(fixedpoint_compare(test4A, test4B) == 1);

    // A < B, A neg, B nonneg
    Fixedpoint test5A = fixedpoint_create_from_hex("-7779fe12f.ef0077525f9");
    Fixedpoint test5B = fixedpoint_create_from_hex("3e5aec3b31e7c.65af0151a4");
    ASSERT(fixedpoint_compare(test5A, test5B) == -1);

    // A > B, A neg, B neg
    Fixedpoint test6A = fixedpoint_create_from_hex("-1000");
    Fixedpoint test6B = fixedpoint_create_from_hex("-100000");
    ASSERT(fixedpoint_compare(test6A, test6B) == 1);

    // A = B, A neg, B neg
    Fixedpoint test7A = fixedpoint_create_from_hex("-100.000");
    Fixedpoint test7B = fixedpoint_create_from_hex("-0000100.00000");
    ASSERT(fixedpoint_compare(test7A, test7B) == 0);

    // A < B, A neg, B neg
    Fixedpoint test8A = fixedpoint_create_from_hex("-100.010");
    Fixedpoint test8B = fixedpoint_create_from_hex("-100.001");
    ASSERT(fixedpoint_compare(test8A, test8B) == -1);
}

void test_fixedpoint_is_err(TestObjs *objs)
{
    // No error
    ASSERT(fixedpoint_is_err(objs->noerror) == 0);

    // Error on creation
    ASSERT(fixedpoint_is_err(objs->format_error) == 1);

    // Positive Overflow Errors
    ASSERT(fixedpoint_is_err(objs->overflow_positive) == 1);

    // Negative Overflow Errors
    ASSERT(fixedpoint_is_err(objs->overflow_negative) == 1);

    // Positive Underflow Error
    ASSERT(fixedpoint_is_err(objs->underflow_positive) == 1);

    // Negative Underflow Error
    ASSERT(fixedpoint_is_err(objs->underflow_negative) == 1);
}

void test_fixedpoint_is_neg(TestObjs *objs)
{
    (void)objs;
    // Positive
    Fixedpoint test1 = fixedpoint_create_from_hex("10.10");
    ASSERT(fixedpoint_is_neg(test1) == 0);

    // Zero
    Fixedpoint test2 = fixedpoint_create_from_hex("0");
    ASSERT(fixedpoint_is_neg(test2) == 0);

    // Negative
    Fixedpoint test3 = fixedpoint_create_from_hex("-a");
    ASSERT(fixedpoint_is_neg(test3) == 1);
        // Error on creation
    ASSERT(fixedpoint_is_neg(objs->format_error) == 0);

    // Positive Overflow Errors
    ASSERT(fixedpoint_is_neg(objs->overflow_positive) == 0);

    // Negative Overflow Errors
    ASSERT(fixedpoint_is_neg(objs->overflow_negative) == 0);

    // Positive Underflow Error
    ASSERT(fixedpoint_is_neg(objs->underflow_positive) == 0);

    // Negative Underflow Error
    ASSERT(fixedpoint_is_neg(objs->underflow_negative) == 0);
}

void test_fixedpoint_is_overflow_neg(TestObjs *objs)
{
    // No error
    ASSERT(fixedpoint_is_overflow_neg(objs->noerror) == 0);

    // Error on Creation
    ASSERT(fixedpoint_is_overflow_neg(objs->format_error) == 0);

    // Positive Overflow Errors
    ASSERT(fixedpoint_is_overflow_neg(objs->overflow_positive) == 0);

    // Negative Overflow Errors
    ASSERT(fixedpoint_is_overflow_neg(objs->overflow_negative) == 1);

    // Positive Underflow Error
    ASSERT(fixedpoint_is_overflow_neg(objs->underflow_positive) == 0);

    // Negative Underflow Error
    ASSERT(fixedpoint_is_overflow_neg(objs->underflow_negative) == 0);
}

void test_fixedpoint_is_overflow_pos(TestObjs *objs)
{
    // No error
    ASSERT(fixedpoint_is_overflow_pos(objs->noerror) == 0);

    // Error on Creation
    ASSERT(fixedpoint_is_overflow_pos(objs->format_error) == 0);

    // Positive Overflow Errors
    ASSERT(fixedpoint_is_overflow_pos(objs->overflow_positive) == 1);

    // Negative Overflow Errors
    ASSERT(fixedpoint_is_overflow_pos(objs->overflow_negative) == 0);

    // Positive Underflow Error
    ASSERT(fixedpoint_is_overflow_pos(objs->underflow_positive) == 0);

    // Negative Underflow Error
    ASSERT(fixedpoint_is_overflow_pos(objs->underflow_negative) == 0);
}

void test_fixedpoint_is_underflow_neg(TestObjs *objs)
{
    // No error
    ASSERT(fixedpoint_is_underflow_neg(objs->noerror) == 0);

    // Error on Creation
    ASSERT(fixedpoint_is_underflow_neg(objs->format_error) == 0);

    // Positive Overflow Errors
    ASSERT(fixedpoint_is_underflow_neg(objs->overflow_positive) == 0);

    // Negative Overflow Errors
    ASSERT(fixedpoint_is_underflow_neg(objs->overflow_negative) == 0);

    // Positive Underflow Error
    ASSERT(fixedpoint_is_underflow_neg(objs->underflow_positive) == 0);

    // Negative Underflow Error
    ASSERT(fixedpoint_is_underflow_neg(objs->underflow_negative) == 1);
}

void test_fixedpoint_is_underflow_pos(TestObjs *objs)
{
    // No error
    ASSERT(fixedpoint_is_underflow_pos(objs->noerror) == 0);

    // Error on Creation
    ASSERT(fixedpoint_is_underflow_pos(objs->format_error) == 0);

    // Positive Overflow Errors
    ASSERT(fixedpoint_is_underflow_pos(objs->overflow_positive) == 0);

    // Negative Overflow Errors
    ASSERT(fixedpoint_is_underflow_pos(objs->overflow_negative) == 0);

    // Positive Underflow Error
    ASSERT(fixedpoint_is_underflow_pos(objs->underflow_positive) == 1);

    // Negative Underflow Error
    ASSERT(fixedpoint_is_underflow_pos(objs->underflow_negative) == 0);
}