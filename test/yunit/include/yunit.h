/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YOC_UNIT_TEST_H
#define YOC_UNIT_TEST_H

#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*yunit_test_case_proc)(void);

typedef struct {
    const char          *name;
    yunit_test_case_proc test_proc;
} yunit_test_case_t;

#define YUNIT_TEST_CASE_NULL { 0 }

typedef int (*yunit_test_suit_init)(void);
typedef int (*yunit_test_suit_deinit)(void);
typedef void (*yunit_test_case_setup)(void);
typedef void (*yunit_test_case_teardown)(void);

typedef struct {
    const char              *name;
    yunit_test_suit_init     init;
    yunit_test_suit_deinit   deinit;
    yunit_test_case_setup    setup;
    yunit_test_case_teardown teardown;
    yunit_test_case_t       *test_case_array;
} yunit_test_suite_t;

#define YUNIT_TEST_SUITE_NULL { 0 }

void *yunit_add_test_suite(
         const char *name,
         yunit_test_suit_init init,
         yunit_test_suit_deinit deinit,
         yunit_test_case_setup setup,
         yunit_test_case_teardown teardown);

int yunit_add_test_case(
         void *test_suite,
         const char *name, yunit_test_case_proc proc);
int yunit_add_test_suites(yunit_test_suite_t *test_suite_array);

void *yunit_get_test_suite(const char *name);
void *yunit_get_test_case(void *test_suite, const char *name);

void yunit_test_init(void);
void yunit_test_deinit(void);
int yunit_test_run(void);
int yunit_run_test_suite(void *test_suite);
int yunit_run_test_case(void *test_suite, void *test_case);
void yunit_test_print_result(void);

enum {
   TEST_RESULT_SUCCESS = 0,
   TEST_RESULT_FAILURE,
   TEST_RESULT_FATAL
};
void yunit_add_test_case_result(
         int result_type,
         const char *file, size_t line, const char *fmt, ...);

#define YUNIT_FAIL(msg) \
    yunit_add_test_case_result( \
        TEST_RESULT_FAILURE, __FILE__, __LINE__, "%s", msg);

#define YUNIT_FAIL_FATAL(msg) \
    yunit_add_test_case_result( \
        TEST_RESULT_FATAL, __FILE__, __LINE__, "%s", msg);

#define YUNIT_ASSERT(expr) \
    do { \
       yunit_add_test_case_result( \
           (expr) ? TEST_RESULT_SUCCESS : TEST_RESULT_FAILURE, \
            __FILE__, __LINE__, "%s", #expr); \
    } while (0);

#define YUNIT_ASSERT_FETAL(expr) \
    do { \
       yunit_add_test_case_result( \
           (expr) ? TEST_RESULT_SUCCESS : TEST_RESULT_FATAL, \
            __FILE__, __LINE__, "%s", #expr); \
    } while (0);

#define YUNIT_ASSERT_TRUE(expr) YUNIT_ASSERT(expr)

#define YUNIT_ASSERT_FALSE(expr) YUNIT_ASSERT(!(expr))

#define YUNIT_ASSERT_TRUE_FATAL(expr) YUNIT_ASSERT_FETAL(expr)

#define YUNIT_ASSERT_EQUAL(expect, actual) \
    do { \
       int result = (expect == actual); \
       yunit_add_test_case_result( \
           result ? TEST_RESULT_SUCCESS : TEST_RESULT_FAILURE, \
            __FILE__, __LINE__, \
           "%s==%s", #expect, #actual); \
    } while (0);

#define YUNIT_ASSERT_EQUAL_FATAL(expect, actual) \
    do { \
       yunit_add_test_case_result( \
           (expect == actual) ? TEST_RESULT_SUCCESS : TEST_RESULT_FATAL, \
            __FILE__, __LINE__, "%s==%s", #expect, #actual); \
    } while (0);

#define YUNIT_ASSERT_STR_EQUAL(expect, actual) \
    do { \
       int result = strcmp((const char *)expect, (const char *)actual) == 0; \
       yunit_add_test_case_result( \
           result ? TEST_RESULT_SUCCESS : TEST_RESULT_FAILURE, \
            __FILE__, __LINE__, \
            result ? "%s(%s)==%s(%s)" : "expect %s(%s) but actual is %s(%s)", \
           #expect, (const char *)expect, #actual, (const char *)actual); \
    } while (0);

#define YUNIT_ASSERT_STR_N_EQUAL(expect, actual, len) \
    do { \
       int result = strncmp((const char *)expect, (const char *)actual, len) == 0; \
       yunit_add_test_case_result( \
           result ? TEST_RESULT_SUCCESS : TEST_RESULT_FAILURE, \
           __FILE__, __LINE__, \
           result ? "%s(%s)==%s(%s) len=%d" \
                  : "expect %s(%s) but actual is %s(%s) len=%d", \
           #expect, (const char *)expect, #actual, (const char *)actual, len); \
    } while (0);

#define YUNIT_ASSERT_PTR_NULL(p) YUNIT_ASSERT_PTR_EQUAL(p, NULL)

#define YUNIT_ASSERT_PTR_NOT_NULL(p) YUNIT_ASSERT_PTR_NOT_EQUAL(p, NULL)

#define YUNIT_ASSERT_PTR_EQUAL(expect, actual) \
    do { \
       int result = (expect == actual); \
       yunit_add_test_case_result( \
           result ? TEST_RESULT_SUCCESS : TEST_RESULT_FAILURE, \
            __FILE__, __LINE__, \
           result ? "%s(%p)==%s(%p)" : "expect %s(%p) but actual is %s(%p)", \
           #expect, expect, #actual, actual); \
    } while (0);

#define YUNIT_ASSERT_PTR_NOT_EQUAL(expect, actual) \
    do { \
       int result = (expect != actual); \
       yunit_add_test_case_result( \
           result ? TEST_RESULT_SUCCESS : TEST_RESULT_FAILURE, \
            __FILE__, __LINE__, \
           result ? "%s(%p)!=%s(%p)" : "expect %s(%p) but actual is %s(%p)", \
           #expect, expect, #actual, actual); \
    } while (0);

#ifdef __cplusplus
}
#endif

#endif /* YOC_UNIT_TEST_H */

