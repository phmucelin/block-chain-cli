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

void test_new_user_null_name(void)
{
    Users *user = new_user(NULL, "password", "25");
    TEST_ASSERT_NULL(user);
}

void test_new_user_null_password(void)
{
    Users *user = new_user("Alice", NULL, "25");
    TEST_ASSERT_NULL(user);
}

void test_new_user_null_age(void)
{
    Users *user = new_user("Alice", "password", NULL);
    TEST_ASSERT_NULL(user);
}

void test_new_user_age_zero_returns_null(void)
{
    /* atoi("0") == 0; condition !age || age<=0 triggers NULL return */
    Users *user = new_user("Alice", "password", "0");
    TEST_ASSERT_NULL(user);
}

void test_new_user_negative_age_returns_null(void)
{
    Users *user = new_user("Alice", "password", "-5");
    TEST_ASSERT_NULL(user);
}

void test_new_user_non_numeric_age_returns_null(void)
{
    /* atoi("abc") == 0 → triggers NULL return */
    Users *user = new_user("Alice", "password", "abc");
    TEST_ASSERT_NULL(user);
}

void test_new_user_bank_initially_null(void)
{
    Users *user = new_user("Alice", "password", "25");
    TEST_ASSERT_NULL(user->bank);
    free(user->hashPass);
    free(user->uuid);
    free(user);
}

void test_new_user_coins_initially_null(void)
{
    Users *user = new_user("Alice", "password", "25");
    TEST_ASSERT_NULL(user->coins);
    free(user->hashPass);
    free(user->uuid);
    free(user);
}

void test_new_user_prox_initially_null(void)
{
    Users *user = new_user("Alice", "password", "25");
    TEST_ASSERT_NULL(user->prox);
    free(user->hashPass);
    free(user->uuid);
    free(user);
}

/* ── get_user_by_uuid ── */

/* ── helpers para UUID search (UUIDs fixos para evitar colisao por srand) ── */

static Users *make_user_fixed_uuid(char *name, char *uuid)
{
    Users *u = (Users *)malloc(sizeof(Users));
    u->name = name;
    u->uuid = uuid;
    u->hashPass = "hash";
    u->age = 25;
    u->bank = NULL;
    u->transaction_id = NULL;
    u->coins = NULL;
    u->prox = NULL;
    return u;
}

void test_get_user_by_uuid_found(void)
{
    Users *u1 = make_user_fixed_uuid("Alice", "aaaa-aaaa-aaaa-aaaa-aaaa");
    Users *u2 = make_user_fixed_uuid("Bob",   "bbbb-bbbb-bbbb-bbbb-bbbb");
    u1->prox  = (struct Users *)u2;

    Users *found = get_user_by_uuid(u1, "bbbb-bbbb-bbbb-bbbb-bbbb");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("Bob", found->name);

    free(u1); free(u2);
}

void test_get_user_by_uuid_not_found(void)
{
    Users *u = make_user_fixed_uuid("Alice", "aaaa-aaaa-aaaa-aaaa-aaaa");

    Users *found = get_user_by_uuid(u, "xxxx-xxxx-xxxx-xxxx-xxxx");
    TEST_ASSERT_NULL(found);

    free(u);
}

void test_get_user_by_uuid_null_list(void)
{
    Users *found = get_user_by_uuid(NULL, "any-uuid");
    TEST_ASSERT_NULL(found);
}

void test_get_user_by_uuid_first_in_list(void)
{
    Users *u1 = make_user_fixed_uuid("Alice", "aaaa-aaaa-aaaa-aaaa-aaaa");
    Users *u2 = make_user_fixed_uuid("Bob",   "bbbb-bbbb-bbbb-bbbb-bbbb");
    u1->prox  = (struct Users *)u2;

    Users *found = get_user_by_uuid(u1, "aaaa-aaaa-aaaa-aaaa-aaaa");
    TEST_ASSERT_EQUAL_PTR(u1, found);

    free(u1); free(u2);
}

void test_get_user_by_uuid_last_in_list(void)
{
    Users *u1 = make_user_fixed_uuid("Alice", "aaaa-aaaa-aaaa-aaaa-aaaa");
    Users *u2 = make_user_fixed_uuid("Bob",   "bbbb-bbbb-bbbb-bbbb-bbbb");
    Users *u3 = make_user_fixed_uuid("Carol", "cccc-cccc-cccc-cccc-cccc");
    u1->prox  = (struct Users *)u2;
    u2->prox  = (struct Users *)u3;

    Users *found = get_user_by_uuid(u1, "cccc-cccc-cccc-cccc-cccc");
    TEST_ASSERT_EQUAL_PTR(u3, found);

    free(u1); free(u2); free(u3);
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
    RUN_TEST(test_new_user_null_name);
    RUN_TEST(test_new_user_null_password);
    RUN_TEST(test_new_user_null_age);
    RUN_TEST(test_new_user_age_zero_returns_null);
    RUN_TEST(test_new_user_negative_age_returns_null);
    RUN_TEST(test_new_user_non_numeric_age_returns_null);
    RUN_TEST(test_new_user_bank_initially_null);
    RUN_TEST(test_new_user_coins_initially_null);
    RUN_TEST(test_new_user_prox_initially_null);
    RUN_TEST(test_get_user_by_uuid_found);
    RUN_TEST(test_get_user_by_uuid_not_found);
    RUN_TEST(test_get_user_by_uuid_null_list);
    RUN_TEST(test_get_user_by_uuid_first_in_list);
    RUN_TEST(test_get_user_by_uuid_last_in_list);

    return UNITY_END();
}