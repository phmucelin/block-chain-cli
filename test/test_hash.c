#include "../test/Unity/src/unity.h"
#include "../test/Unity/src/unity.c"
#include "../services/hash.c"
#include "../services/hashpass.c"

#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ── hash() ── */

void test_hash_not_null(void)
{
    char *h = hash("any input");
    TEST_ASSERT_NOT_NULL(h);
    free(h);
}

void test_hash_length_is_32(void)
{
    char *h = hash("password");
    TEST_ASSERT_EQUAL_INT(32, (int)strlen(h));
    free(h);
}

void test_hash_null_terminated(void)
{
    char *h = hash("test");
    TEST_ASSERT_EQUAL_CHAR('\0', h[32]);
    free(h);
}

void test_hash_empty_string(void)
{
    char *h = hash("");
    TEST_ASSERT_NOT_NULL(h);
    TEST_ASSERT_EQUAL_INT(32, (int)strlen(h));
    free(h);
}

void test_hash_long_input(void)
{
    char *h = hash("this is a very long password that is definitely over 32 chars");
    TEST_ASSERT_NOT_NULL(h);
    TEST_ASSERT_EQUAL_INT(32, (int)strlen(h));
    free(h);
}

void test_hash_output_chars_in_expected_set(void)
{
    const char *charset = "0121104592XcDKlPq7s8a9b4e6f3g5h0i1j2mnoqrstulkocUiABEFGHIJYZ";
    char *h = hash("password");
    for (int i = 0; i < 32; i++) {
        TEST_ASSERT_NOT_NULL(strchr(charset, h[i]));
    }
    free(h);
}

/* ── hash_password_and_block() ── */

void test_hashpass_null_input_returns_null(void)
{
    char *h = hash_password_and_block(NULL);
    TEST_ASSERT_NULL(h);
}

void test_hashpass_not_null(void)
{
    char *h = hash_password_and_block("password123");
    TEST_ASSERT_NOT_NULL(h);
    free(h);
}

void test_hashpass_null_terminator_at_64(void)
{
    char *h = hash_password_and_block("abc");
    TEST_ASSERT_EQUAL_CHAR('\0', h[64]);
    free(h);
}

void test_hashpass_transformation_char_plus_one(void)
{
    /* each byte = (input[i] + 1) % 256 */
    const char *input = "ABC";
    char *h = hash_password_and_block(input);
    TEST_ASSERT_EQUAL_CHAR((char)(('A' + 1) % 256), h[0]);
    TEST_ASSERT_EQUAL_CHAR((char)(('B' + 1) % 256), h[1]);
    TEST_ASSERT_EQUAL_CHAR((char)(('C' + 1) % 256), h[2]);
    free(h);
}

void test_hashpass_empty_string(void)
{
    /* loop exits immediately at i=0 because p[0]=='\0'; h[64] must be '\0' */
    char *h = hash_password_and_block("");
    TEST_ASSERT_NOT_NULL(h);
    TEST_ASSERT_EQUAL_CHAR('\0', h[64]);
    free(h);
}

void test_hashpass_different_inputs_produce_different_outputs(void)
{
    char *h1 = hash_password_and_block("password1");
    char *h2 = hash_password_and_block("password2");
    /* first differing character is index 8: '1'+1 vs '2'+1 */
    TEST_ASSERT_NOT_EQUAL(0, strncmp(h1, h2, 64));
    free(h1);
    free(h2);
}

void test_hashpass_same_input_same_output(void)
{
    char *h1 = hash_password_and_block("deterministic");
    char *h2 = hash_password_and_block("deterministic");
    TEST_ASSERT_EQUAL_INT(0, strncmp(h1, h2, 64));
    free(h1);
    free(h2);
}

void test_hashpass_long_input_capped_at_64(void)
{
    /* input longer than 64 chars; function only processes up to 64 bytes */
    char *h = hash_password_and_block(
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    TEST_ASSERT_NOT_NULL(h);
    TEST_ASSERT_EQUAL_CHAR('\0', h[64]);
    free(h);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_hash_not_null);
    RUN_TEST(test_hash_length_is_32);
    RUN_TEST(test_hash_null_terminated);
    RUN_TEST(test_hash_empty_string);
    RUN_TEST(test_hash_long_input);
    RUN_TEST(test_hash_output_chars_in_expected_set);

    RUN_TEST(test_hashpass_null_input_returns_null);
    RUN_TEST(test_hashpass_not_null);
    RUN_TEST(test_hashpass_null_terminator_at_64);
    RUN_TEST(test_hashpass_transformation_char_plus_one);
    RUN_TEST(test_hashpass_empty_string);
    RUN_TEST(test_hashpass_different_inputs_produce_different_outputs);
    RUN_TEST(test_hashpass_same_input_same_output);
    RUN_TEST(test_hashpass_long_input_capped_at_64);

    return UNITY_END();
}
