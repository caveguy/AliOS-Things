#include <yunit.h>
#include <yts.h>
#include <stdio.h>
#include "ali_crypto.h"

extern void ali_crypto_test_entry(void);

static int init(void)
{
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

#if 0
static void ali_crypto_test(void)
{
    return;
}
#endif

static yunit_test_case_t yunos_alicrypto_testcases[] = {
    //{ "alicrypto_test", ali_crypto_test},
    { "alicrypto_test", ali_crypto_test_entry},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "alicrypto", init, cleanup, setup, teardown, yunos_alicrypto_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_alicrypto(void)
{
    yunit_add_test_suites(suites);
}

