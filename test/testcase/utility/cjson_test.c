#include <stdio.h>
#include <stdlib.h>

#include <yunit.h>
#include <yts.h>

#include <cJSON.h>

static void test_simple_case(void)
{
    cJSON *obj = cJSON_CreateObject();

    cJSON_AddStringToObject(obj, "a", "b");
    char *p = cJSON_PrintUnformatted(obj);
    YUNIT_ASSERT(strcmp(p, "{\"a\":\"b\"}") == 0);
    cJSON_free(p);

    cJSON_Delete(obj);
}

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

static yunit_test_case_t yunos_basic_testcases[] = {
    { "simple", test_simple_case },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "cjson", init, cleanup, setup, teardown, yunos_basic_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_cjson(void)
{
    yunit_add_test_suites(suites);
}

