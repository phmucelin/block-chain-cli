#include "../test/Unity/src/unity.h"
#include "../test/Unity/src/unity.c"
#include "../services/bank_actions.c"

#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

/* ── helpers ── */

static Bank *make_bank(void)
{
    Bank *b = (Bank *)malloc(sizeof(Bank));
    b->name = "TestBank";
    b->id = "001";
    b->country = "BR";
    b->qtdClients = 0;
    return b;
}

static Users *make_user(void)
{
    Users *u = (Users *)malloc(sizeof(Users));
    u->uuid = "uuid-test";
    u->name = "Alice";
    u->hashPass = "abc";
    u->age = 25;
    u->bank = NULL;
    u->transaction_id = NULL;
    u->coins = NULL;
    u->prox = NULL;
    return u;
}

/* ── is_user_from_bank ── */

void test_is_user_from_bank_null_bank(void)
{
    Users *u = make_user();
    TEST_ASSERT_EQUAL_INT(0, is_user_from_bank(NULL, u));
    free(u);
}

void test_is_user_from_bank_null_user(void)
{
    Bank *b = make_bank();
    TEST_ASSERT_EQUAL_INT(0, is_user_from_bank(b, NULL));
    free(b);
}

void test_is_user_from_bank_both_null(void)
{
    TEST_ASSERT_EQUAL_INT(0, is_user_from_bank(NULL, NULL));
}

void test_is_user_from_bank_user_has_no_bank(void)
{
    Bank *b = make_bank();
    Users *u = make_user(); /* u->bank == NULL */
    TEST_ASSERT_EQUAL_INT(0, is_user_from_bank(b, u));
    free(b);
    free(u);
}

void test_is_user_from_bank_associated(void)
{
    Bank *b = make_bank();
    Users *u = make_user();
    u->bank = b;
    TEST_ASSERT_EQUAL_INT(1, is_user_from_bank(b, u));
    free(b);
    free(u);
}

void test_is_user_from_bank_different_bank(void)
{
    Bank *b1 = make_bank();
    Bank *b2 = make_bank();
    Users *u  = make_user();
    u->bank = b1;
    TEST_ASSERT_EQUAL_INT(0, is_user_from_bank(b2, u));
    free(b1);
    free(b2);
    free(u);
}

/* ── approve_user_account ── */

void test_approve_null_bank(void)
{
    Users *u = make_user();
    TEST_ASSERT_EQUAL_INT(0, approve_user_account(NULL, u));
    free(u);
}

void test_approve_null_user(void)
{
    Bank *b = make_bank();
    TEST_ASSERT_EQUAL_INT(0, approve_user_account(b, NULL));
    free(b);
}

void test_approve_both_null(void)
{
    TEST_ASSERT_EQUAL_INT(0, approve_user_account(NULL, NULL));
}

void test_approve_user_already_has_bank(void)
{
    Bank *b1 = make_bank();
    Bank *b2 = make_bank();
    Users *u  = make_user();
    u->bank = b1;
    TEST_ASSERT_EQUAL_INT(0, approve_user_account(b2, u));
    free(b1);
    free(b2);
    free(u);
}

void test_approve_success_returns_one(void)
{
    Bank *b = make_bank();
    Users *u = make_user();
    TEST_ASSERT_EQUAL_INT(1, approve_user_account(b, u));
    free(b);
    free(u);
}

void test_approve_links_bank_to_user(void)
{
    Bank *b = make_bank();
    Users *u = make_user();
    approve_user_account(b, u);
    TEST_ASSERT_EQUAL_PTR(b, u->bank);
    free(b);
    free(u);
}

void test_approve_increments_qtd_clients(void)
{
    Bank *b  = make_bank();
    Users *u1 = make_user();
    Users *u2 = make_user();
    approve_user_account(b, u1);
    approve_user_account(b, u2);
    TEST_ASSERT_EQUAL_INT(2, b->qtdClients);
    free(b);
    free(u1);
    free(u2);
}

void test_approve_second_bank_rejected_when_already_has_one(void)
{
    Bank *b1 = make_bank();
    Bank *b2 = make_bank();
    Users *u  = make_user();
    approve_user_account(b1, u);
    /* attempt to approve on second bank must fail */
    TEST_ASSERT_EQUAL_INT(0, approve_user_account(b2, u));
    TEST_ASSERT_EQUAL_PTR(b1, u->bank); /* original bank unchanged */
    TEST_ASSERT_EQUAL_INT(0, b2->qtdClients);
    free(b1);
    free(b2);
    free(u);
}

/* ── reject_user_account ── */

void test_reject_null_bank(void)
{
    Users *u = make_user();
    TEST_ASSERT_EQUAL_INT(0, reject_user_account(NULL, u));
    free(u);
}

void test_reject_null_user(void)
{
    Bank *b = make_bank();
    TEST_ASSERT_EQUAL_INT(0, reject_user_account(b, NULL));
    free(b);
}

void test_reject_both_null(void)
{
    TEST_ASSERT_EQUAL_INT(0, reject_user_account(NULL, NULL));
}

void test_reject_user_not_in_bank(void)
{
    Bank *b1 = make_bank();
    Bank *b2 = make_bank();
    Users *u  = make_user();
    u->bank = b1;
    TEST_ASSERT_EQUAL_INT(0, reject_user_account(b2, u));
    free(b1);
    free(b2);
    free(u);
}

void test_reject_user_has_no_bank(void)
{
    Bank *b = make_bank();
    Users *u = make_user(); /* u->bank == NULL != b */
    TEST_ASSERT_EQUAL_INT(0, reject_user_account(b, u));
    free(b);
    free(u);
}

void test_reject_success_returns_one(void)
{
    Bank *b = make_bank();
    Users *u = make_user();
    u->bank = b;
    b->qtdClients = 1;
    TEST_ASSERT_EQUAL_INT(1, reject_user_account(b, u));
    free(b);
    free(u);
}

void test_reject_clears_user_bank_pointer(void)
{
    Bank *b = make_bank();
    Users *u = make_user();
    u->bank = b;
    b->qtdClients = 1;
    reject_user_account(b, u);
    TEST_ASSERT_NULL(u->bank);
    free(b);
    free(u);
}

void test_reject_decrements_qtd_clients(void)
{
    Bank *b = make_bank();
    Users *u = make_user();
    u->bank = b;
    b->qtdClients = 3;
    reject_user_account(b, u);
    TEST_ASSERT_EQUAL_INT(2, b->qtdClients);
    free(b);
    free(u);
}

void test_reject_qtd_clients_never_goes_negative(void)
{
    Bank *b = make_bank();
    Users *u = make_user();
    u->bank = b;
    b->qtdClients = 0; /* already at zero */
    reject_user_account(b, u);
    TEST_ASSERT_EQUAL_INT(0, b->qtdClients);
    free(b);
    free(u);
}

void test_approve_then_reject_full_cycle(void)
{
    Bank *b = make_bank();
    Users *u = make_user();
    approve_user_account(b, u);
    TEST_ASSERT_EQUAL_INT(1, b->qtdClients);
    TEST_ASSERT_EQUAL_PTR(b, u->bank);

    reject_user_account(b, u);
    TEST_ASSERT_EQUAL_INT(0, b->qtdClients);
    TEST_ASSERT_NULL(u->bank);

    free(b);
    free(u);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_is_user_from_bank_null_bank);
    RUN_TEST(test_is_user_from_bank_null_user);
    RUN_TEST(test_is_user_from_bank_both_null);
    RUN_TEST(test_is_user_from_bank_user_has_no_bank);
    RUN_TEST(test_is_user_from_bank_associated);
    RUN_TEST(test_is_user_from_bank_different_bank);

    RUN_TEST(test_approve_null_bank);
    RUN_TEST(test_approve_null_user);
    RUN_TEST(test_approve_both_null);
    RUN_TEST(test_approve_user_already_has_bank);
    RUN_TEST(test_approve_success_returns_one);
    RUN_TEST(test_approve_links_bank_to_user);
    RUN_TEST(test_approve_increments_qtd_clients);
    RUN_TEST(test_approve_second_bank_rejected_when_already_has_one);

    RUN_TEST(test_reject_null_bank);
    RUN_TEST(test_reject_null_user);
    RUN_TEST(test_reject_both_null);
    RUN_TEST(test_reject_user_not_in_bank);
    RUN_TEST(test_reject_user_has_no_bank);
    RUN_TEST(test_reject_success_returns_one);
    RUN_TEST(test_reject_clears_user_bank_pointer);
    RUN_TEST(test_reject_decrements_qtd_clients);
    RUN_TEST(test_reject_qtd_clients_never_goes_negative);
    RUN_TEST(test_approve_then_reject_full_cycle);

    return UNITY_END();
}
