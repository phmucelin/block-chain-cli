#include "../test/Unity/src/unity.h"
#include "../test/Unity/src/unity.c"
#include "../services/admin_actions.c"

#include <stdlib.h>
#include <string.h>

/* reset da lista global entre testes */
extern Admin* admin_list;

void setUp(void)
{
    admin_list = NULL;
}

void tearDown(void) {}

/* ── create_bank ── */

void test_create_bank_not_null(void)
{
    Bank *b = create_bank("BancoBR", "001", "BR");
    TEST_ASSERT_NOT_NULL(b);
    free(b);
}

void test_create_bank_null_name(void)
{
    Bank *b = create_bank(NULL, "001", "BR");
    TEST_ASSERT_NULL(b);
}

void test_create_bank_null_id(void)
{
    Bank *b = create_bank("BancoBR", NULL, "BR");
    TEST_ASSERT_NULL(b);
}

void test_create_bank_null_country(void)
{
    Bank *b = create_bank("BancoBR", "001", NULL);
    TEST_ASSERT_NULL(b);
}

void test_create_bank_all_null(void)
{
    Bank *b = create_bank(NULL, NULL, NULL);
    TEST_ASSERT_NULL(b);
}

void test_create_bank_name_field(void)
{
    Bank *b = create_bank("BancoBR", "001", "BR");
    TEST_ASSERT_EQUAL_STRING("BancoBR", b->name);
    free(b);
}

void test_create_bank_id_field(void)
{
    Bank *b = create_bank("BancoBR", "007", "BR");
    TEST_ASSERT_EQUAL_STRING("007", b->id);
    free(b);
}

void test_create_bank_country_field(void)
{
    Bank *b = create_bank("BancoBR", "001", "US");
    TEST_ASSERT_EQUAL_STRING("US", b->country);
    free(b);
}

void test_create_bank_empty_strings(void)
{
    Bank *b = create_bank("", "", "");
    TEST_ASSERT_NOT_NULL(b);
    free(b);
}

/* ── list_users ── */

void test_list_users_null_does_not_crash(void)
{
    list_users(NULL);
    TEST_PASS();
}

void test_list_users_single_user_does_not_crash(void)
{
    Users *u = (Users *)malloc(sizeof(Users));
    u->uuid = "uuid-001";
    u->name = "Alice";
    u->age  = 30;
    u->bank = NULL;
    u->transaction_id = NULL;
    u->coins = NULL;
    u->prox  = NULL;
    list_users(u);
    TEST_PASS();
    free(u);
}

void test_list_users_chain_does_not_crash(void)
{
    Users *u1 = (Users *)malloc(sizeof(Users));
    Users *u2 = (Users *)malloc(sizeof(Users));
    u1->uuid = "uuid-001"; u1->name = "Alice"; u1->age = 25;
    u1->bank = NULL; u1->transaction_id = NULL;
    u1->coins = NULL; u1->prox = (struct Users *)u2;
    u2->uuid = "uuid-002"; u2->name = "Bob"; u2->age = 30;
    u2->bank = NULL; u2->transaction_id = NULL;
    u2->coins = NULL; u2->prox = NULL;
    list_users(u1);
    TEST_PASS();
    free(u1);
    free(u2);
}

/* ── verify_admin (Fix: usava malloc+lixo; agora usa lista global) ── */

void test_verify_admin_empty_list_returns_zero(void)
{
    TEST_ASSERT_EQUAL_INT(0, verify_admin("any-uuid"));
}

void test_verify_admin_null_uuid_returns_zero(void)
{
    TEST_ASSERT_EQUAL_INT(0, verify_admin(NULL));
}

void test_verify_admin_known_uuid_returns_one(void)
{
    Admin *a = (Admin *)malloc(sizeof(Admin));
    a->uuid = "fixed-uuid-001";
    a->name = "Root";
    a->prox = NULL;
    admin_list = a;

    TEST_ASSERT_EQUAL_INT(1, verify_admin("fixed-uuid-001"));

    free(a);
    admin_list = NULL;
}

void test_verify_admin_unknown_uuid_returns_zero(void)
{
    Admin *a = (Admin *)malloc(sizeof(Admin));
    a->uuid = "fixed-uuid-001";
    a->name = "Root";
    a->prox = NULL;
    admin_list = a;

    TEST_ASSERT_EQUAL_INT(0, verify_admin("not-this-uuid"));

    free(a);
    admin_list = NULL;
}

/* ── create_admin (Fix: uuid gerado antes do verify; lista global gerenciada) ── */

void test_create_admin_not_null(void)
{
    Admin *a = create_admin("Root", "1990-01-01", "secret");
    TEST_ASSERT_NOT_NULL(a);
    free(a->uuid);
    free(a);
    admin_list = NULL;
}

void test_create_admin_null_name_returns_null(void)
{
    Admin *a = create_admin(NULL, "1990-01-01", "secret");
    TEST_ASSERT_NULL(a);
}

void test_create_admin_null_birthday_returns_null(void)
{
    Admin *a = create_admin("Root", NULL, "secret");
    TEST_ASSERT_NULL(a);
}

void test_create_admin_null_passkey_returns_null(void)
{
    Admin *a = create_admin("Root", "1990-01-01", NULL);
    TEST_ASSERT_NULL(a);
}

void test_create_admin_all_null_returns_null(void)
{
    Admin *a = create_admin(NULL, NULL, NULL);
    TEST_ASSERT_NULL(a);
}

void test_create_admin_added_to_global_list(void)
{
    create_admin("Root", "1990-01-01", "key");
    TEST_ASSERT_NOT_NULL(admin_list);

    free(admin_list->uuid);
    free(admin_list);
    admin_list = NULL;
}

/* ── get_admin (Fix: usava malloc+lixo; agora percorre lista global) ── */

void test_get_admin_empty_list_returns_null(void)
{
    TEST_ASSERT_NULL(get_admin("any-uuid"));
}

void test_get_admin_null_uuid_returns_null(void)
{
    TEST_ASSERT_NULL(get_admin(NULL));
}

void test_get_admin_found(void)
{
    Admin *a = (Admin *)malloc(sizeof(Admin));
    a->uuid = "fixed-uuid-002";
    a->name = "Root";
    a->prox = NULL;
    admin_list = a;

    Admin *found = get_admin("fixed-uuid-002");
    TEST_ASSERT_EQUAL_PTR(a, found);

    free(a);
    admin_list = NULL;
}

void test_get_admin_not_found(void)
{
    Admin *a = (Admin *)malloc(sizeof(Admin));
    a->uuid = "fixed-uuid-002";
    a->name = "Root";
    a->prox = NULL;
    admin_list = a;

    Admin *found = get_admin("wrong-uuid");
    TEST_ASSERT_NULL(found);

    free(a);
    admin_list = NULL;
}

/* ── delete_user (Fix: dependia de get_admin, que agora funciona) ── */

void test_delete_user_null_head_returns_zero(void)
{
    Admin *a = (Admin *)malloc(sizeof(Admin));
    a->uuid = "admin-uuid";
    a->name = "Root";
    a->prox = NULL;
    admin_list = a;

    TEST_ASSERT_EQUAL_INT(0, delete_user(NULL, "Alice", NULL, "admin-uuid"));

    free(a);
    admin_list = NULL;
}

void test_delete_user_null_admin_uuid_returns_zero(void)
{
    Users *u = (Users *)malloc(sizeof(Users));
    u->uuid = "user-001"; u->name = "Alice"; u->age = 25;
    u->bank = NULL; u->transaction_id = NULL; u->coins = NULL; u->prox = NULL;

    TEST_ASSERT_EQUAL_INT(0, delete_user(&u, "Alice", NULL, NULL));

    free(u);
}

void test_delete_user_admin_not_found_returns_zero(void)
{
    Users *u = (Users *)malloc(sizeof(Users));
    u->uuid = "user-001"; u->name = "Alice"; u->age = 25;
    u->bank = NULL; u->transaction_id = NULL; u->coins = NULL; u->prox = NULL;

    /* admin_list is empty — get_admin returns NULL */
    TEST_ASSERT_EQUAL_INT(0, delete_user(&u, "Alice", NULL, "ghost-admin-uuid"));

    free(u);
}

void test_delete_user_by_name_success(void)
{
    Admin *a = (Admin *)malloc(sizeof(Admin));
    a->uuid = "admin-uuid"; a->name = "Root"; a->prox = NULL;
    admin_list = a;

    Users *u = (Users *)malloc(sizeof(Users));
    u->uuid = "user-001"; u->name = "Alice"; u->age = 25;
    u->bank = NULL; u->transaction_id = NULL; u->coins = NULL; u->prox = NULL;

    TEST_ASSERT_EQUAL_INT(1, delete_user(&u, "Alice", NULL, "admin-uuid"));
    TEST_ASSERT_NULL(u); /* lista agora vazia */

    free(a);
    admin_list = NULL;
}

void test_delete_user_by_uuid_success(void)
{
    Admin *a = (Admin *)malloc(sizeof(Admin));
    a->uuid = "admin-uuid"; a->name = "Root"; a->prox = NULL;
    admin_list = a;

    Users *u = (Users *)malloc(sizeof(Users));
    u->uuid = "user-001"; u->name = "Alice"; u->age = 25;
    u->bank = NULL; u->transaction_id = NULL; u->coins = NULL; u->prox = NULL;

    TEST_ASSERT_EQUAL_INT(1, delete_user(&u, NULL, "user-001", "admin-uuid"));
    TEST_ASSERT_NULL(u);

    free(a);
    admin_list = NULL;
}

void test_delete_user_not_found_returns_zero(void)
{
    Admin *a = (Admin *)malloc(sizeof(Admin));
    a->uuid = "admin-uuid"; a->name = "Root"; a->prox = NULL;
    admin_list = a;

    Users *u = (Users *)malloc(sizeof(Users));
    u->uuid = "user-001"; u->name = "Alice"; u->age = 25;
    u->bank = NULL; u->transaction_id = NULL; u->coins = NULL; u->prox = NULL;

    TEST_ASSERT_EQUAL_INT(0, delete_user(&u, "Bob", NULL, "admin-uuid"));
    TEST_ASSERT_NOT_NULL(u); /* lista intacta */

    free(u);
    free(a);
    admin_list = NULL;
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_create_bank_not_null);
    RUN_TEST(test_create_bank_null_name);
    RUN_TEST(test_create_bank_null_id);
    RUN_TEST(test_create_bank_null_country);
    RUN_TEST(test_create_bank_all_null);
    RUN_TEST(test_create_bank_name_field);
    RUN_TEST(test_create_bank_id_field);
    RUN_TEST(test_create_bank_country_field);
    RUN_TEST(test_create_bank_empty_strings);

    RUN_TEST(test_list_users_null_does_not_crash);
    RUN_TEST(test_list_users_single_user_does_not_crash);
    RUN_TEST(test_list_users_chain_does_not_crash);

    RUN_TEST(test_verify_admin_empty_list_returns_zero);
    RUN_TEST(test_verify_admin_null_uuid_returns_zero);
    RUN_TEST(test_verify_admin_known_uuid_returns_one);
    RUN_TEST(test_verify_admin_unknown_uuid_returns_zero);

    RUN_TEST(test_create_admin_not_null);
    RUN_TEST(test_create_admin_null_name_returns_null);
    RUN_TEST(test_create_admin_null_birthday_returns_null);
    RUN_TEST(test_create_admin_null_passkey_returns_null);
    RUN_TEST(test_create_admin_all_null_returns_null);
    RUN_TEST(test_create_admin_added_to_global_list);

    RUN_TEST(test_get_admin_empty_list_returns_null);
    RUN_TEST(test_get_admin_null_uuid_returns_null);
    RUN_TEST(test_get_admin_found);
    RUN_TEST(test_get_admin_not_found);

    RUN_TEST(test_delete_user_null_head_returns_zero);
    RUN_TEST(test_delete_user_null_admin_uuid_returns_zero);
    RUN_TEST(test_delete_user_admin_not_found_returns_zero);
    RUN_TEST(test_delete_user_by_name_success);
    RUN_TEST(test_delete_user_by_uuid_success);
    RUN_TEST(test_delete_user_not_found_returns_zero);

    return UNITY_END();
}
