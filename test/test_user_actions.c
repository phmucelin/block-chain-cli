#include "../test/Unity/src/unity.h"
#include "../test/Unity/src/unity.c"
#include "../services/user_actions.c"

#include <string.h>
#include <stdlib.h>
#include <string.h>

void setUp(void)
{
    // roda antes de cada teste
}

void tearDown(void)
{
    // roda depois de cada teste
}

void test_generate_uuid_not_null(void)
{
    char *id = generate_uuid();

    TEST_ASSERT_NOT_NULL(id);

    free(id);
}

void test_generate_uuid_length(void)
{
    char *id = generate_uuid();

    TEST_ASSERT_EQUAL_INT(36, strlen(id));

    free(id);
}

void test_generate_uuid_format(void)
{
    char *id = generate_uuid();

    TEST_ASSERT_EQUAL_CHAR('-', id[8]);
    TEST_ASSERT_EQUAL_CHAR('-', id[13]);
    TEST_ASSERT_EQUAL_CHAR('-', id[18]);
    TEST_ASSERT_EQUAL_CHAR('-', id[23]);

    TEST_ASSERT_EQUAL_CHAR('4', id[14]);

    free(id);
}

void test_generate_hash_not_null(void)
{
    char *hash = generate_hash("password");

    TEST_ASSERT_NOT_NULL(hash);

    free(hash);
}

void test_generate_hash_length(void)
{
    char *hash = generate_hash("password");

    TEST_ASSERT_EQUAL_INT(32, strlen(hash));

    free(hash);
}

void test_generate_hash_content(void)
{
    char *hash = generate_hash("password");

    for (size_t i = 0; i < 32; i++) {
        TEST_ASSERT_TRUE(strchr("0121104592XcDKlPq7s8a9b4e6f3g5h0i1j2mnoqrstulkocUiABEFGHIJYZ", hash[i]) != NULL);
    }

    free(hash);
}

void test_generate_hash_different_inputs(void)
{
    char *hash1 = generate_hash("password1");
    char *hash2 = generate_hash("password2");

    TEST_ASSERT_NOT_EQUAL_UINT(hash1, hash2);

    free(hash1);
    free(hash2);
}

void test_new_user(void)
{
    Users *user = new_user("Alice", "password123", "30");

    TEST_ASSERT_NOT_NULL(user);
    TEST_ASSERT_EQUAL_STRING("Alice", user->name);
    TEST_ASSERT_EQUAL_INT(30, user->age);
    TEST_ASSERT_NOT_NULL(user->hashPass);
    TEST_ASSERT_NOT_NULL(user->uuid);

    free(user->hashPass);
    free(user->uuid);
    free(user);
}

void test_new_user_null(void)
{
    Users *user = new_user(NULL, NULL, NULL);

    TEST_ASSERT_NULL(user);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_generate_uuid_not_null);
    RUN_TEST(test_generate_uuid_length);
    RUN_TEST(test_generate_uuid_format);
    RUN_TEST(test_generate_hash_not_null);
    RUN_TEST(test_generate_hash_length);
    RUN_TEST(test_generate_hash_content);
    RUN_TEST(test_generate_hash_different_inputs);
    RUN_TEST(test_new_user);
    RUN_TEST(test_new_user_null);

    return UNITY_END();
}