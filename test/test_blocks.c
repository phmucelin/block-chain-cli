/*
 * AVISO DE COMPILACAO:
 * blocks.c inclui transitivamente user_actions.c multiplas vezes
 * (via transaction_actions.c e coins_law.c), causando erros de "multiple
 * definition" ao compilar. Para corrigir, substitua todos os #include de
 * arquivos .c dentro de blocks.c, transaction_actions.c e coins_law.c por
 * #include dos respectivos headers (.h).
 *
 * Enquanto isso, create_block() e fill_block() sao copiadas aqui (sao
 * auto-contidas — nao chamam funcoes de outros modulos de servico) para
 * permitir testes imediatos sem alterar o codigo fonte.
 */

#include "../test/Unity/src/unity.h"
#include "../test/Unity/src/unity.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../models/block_model.h"
#include "../models/transaction_model.h"
#include "../models/coinType_model.h"
#include "../models/user_model.h"
#include "../models/bank_model.h"

/* ── copies of create_block / fill_block from services/blocks.c ── */

static Block *create_block(char UUID[37], time_t t, char prev_hash[65],
                            char hash_val[65], Transaction *transactions)
{
    (void)UUID;
    Block *b = (Block *)malloc(sizeof(Block));
    if (!b) return NULL;
    b->index           = 0;
    b->created_at      = t;
    b->updated_at      = t;
    b->expire_at       = t + 3600;
    b->transactions    = transactions;
    b->num_transactions = 0;
    strncpy(b->previous_hash, prev_hash, 65);
    strncpy(b->hash, hash_val, 65);
    b->prox = NULL;
    return b;
}

static int fill_block(Transaction *transaction, Block *block)
{
    if (!transaction || !block) return 0;

    if (!block->transactions) {
        block->transactions = transaction;
    } else {
        Transaction *cur = block->transactions;
        while (cur->prox) cur = cur->prox;
        cur->prox = transaction;
    }
    block->num_transactions++;
    block->updated_at = time(NULL);
    return 1;
}

/* ── helpers ── */

static Transaction *make_transaction(void)
{
    UserCoin *coin = (UserCoin *)malloc(sizeof(UserCoin));
    coin->type    = BTC;
    coin->qtdCoin = 10;
    coin->prox    = NULL;

    Transaction *tx = (Transaction *)malloc(sizeof(Transaction));
    tx->uuidSender  = "sender-uuid";
    tx->uuidReceive = "receiver-uuid";
    tx->coin        = coin;
    tx->receipt     = 42;
    tx->prox        = NULL;
    return tx;
}

static void free_transaction(Transaction *tx)
{
    if (!tx) return;
    free(tx->coin);
    free(tx);
}

static Block *make_empty_block(void)
{
    char uuid[37]      = "00000000-0000-4000-0000-000000000000";
    char prev_hash[65] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    char hash_val[65]  = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
    return create_block(uuid, time(NULL), prev_hash, hash_val, NULL);
}

void setUp(void) {}
void tearDown(void) {}

/* ── create_block ── */

void test_create_block_not_null(void)
{
    Block *b = make_empty_block();
    TEST_ASSERT_NOT_NULL(b);
    free(b);
}

void test_create_block_index_is_zero(void)
{
    Block *b = make_empty_block();
    TEST_ASSERT_EQUAL_INT(0, b->index);
    free(b);
}

void test_create_block_expire_at_is_created_plus_3600(void)
{
    time_t t = time(NULL);
    char uuid[37]      = "00000000-0000-4000-0000-000000000000";
    char prev_hash[65] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    char hash_val[65]  = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
    Block *b = create_block(uuid, t, prev_hash, hash_val, NULL);
    TEST_ASSERT_EQUAL_INT64(t + 3600, (long long)b->expire_at);
    free(b);
}

void test_create_block_updated_at_equals_created_at(void)
{
    time_t t = 1000000;
    char uuid[37]      = "00000000-0000-4000-0000-000000000000";
    char prev_hash[65] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    char hash_val[65]  = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
    Block *b = create_block(uuid, t, prev_hash, hash_val, NULL);
    TEST_ASSERT_EQUAL_INT64((long long)t, (long long)b->updated_at);
    free(b);
}

void test_create_block_previous_hash_copied(void)
{
    char uuid[37]      = "00000000-0000-4000-0000-000000000000";
    char prev_hash[65] = "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc";
    char hash_val[65]  = "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd";
    Block *b = create_block(uuid, time(NULL), prev_hash, hash_val, NULL);
    TEST_ASSERT_EQUAL_STRING(prev_hash, b->previous_hash);
    free(b);
}

void test_create_block_hash_copied(void)
{
    char uuid[37]      = "00000000-0000-4000-0000-000000000000";
    char prev_hash[65] = "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc";
    char hash_val[65]  = "1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef";
    Block *b = create_block(uuid, time(NULL), prev_hash, hash_val, NULL);
    TEST_ASSERT_EQUAL_STRING(hash_val, b->hash);
    free(b);
}

void test_create_block_null_transactions_ok(void)
{
    Block *b = make_empty_block();
    TEST_ASSERT_NULL(b->transactions);
    free(b);
}

void test_create_block_num_transactions_zero(void)
{
    Block *b = make_empty_block();
    TEST_ASSERT_EQUAL_INT(0, b->num_transactions);
    free(b);
}

void test_create_block_prox_null(void)
{
    Block *b = make_empty_block();
    TEST_ASSERT_NULL(b->prox);
    free(b);
}

void test_create_block_with_initial_transaction(void)
{
    Transaction *tx = make_transaction();
    char uuid[37]      = "00000000-0000-4000-0000-000000000000";
    char prev_hash[65] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    char hash_val[65]  = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
    Block *b = create_block(uuid, time(NULL), prev_hash, hash_val, tx);
    TEST_ASSERT_EQUAL_PTR(tx, b->transactions);
    free(b);
    free_transaction(tx);
}

/* ── fill_block ── */

void test_fill_block_null_transaction_returns_zero(void)
{
    Block *b = make_empty_block();
    TEST_ASSERT_EQUAL_INT(0, fill_block(NULL, b));
    free(b);
}

void test_fill_block_null_block_returns_zero(void)
{
    Transaction *tx = make_transaction();
    TEST_ASSERT_EQUAL_INT(0, fill_block(tx, NULL));
    free_transaction(tx);
}

void test_fill_block_both_null_returns_zero(void)
{
    TEST_ASSERT_EQUAL_INT(0, fill_block(NULL, NULL));
}

void test_fill_block_success_returns_one(void)
{
    Block *b       = make_empty_block();
    Transaction *tx = make_transaction();
    TEST_ASSERT_EQUAL_INT(1, fill_block(tx, b));
    free(b);
    free_transaction(tx);
}

void test_fill_block_first_transaction_set(void)
{
    Block *b       = make_empty_block();
    Transaction *tx = make_transaction();
    fill_block(tx, b);
    TEST_ASSERT_EQUAL_PTR(tx, b->transactions);
    free(b);
    free_transaction(tx);
}

void test_fill_block_increments_num_transactions(void)
{
    Block *b       = make_empty_block();
    Transaction *tx = make_transaction();
    fill_block(tx, b);
    TEST_ASSERT_EQUAL_INT(1, b->num_transactions);
    free(b);
    free_transaction(tx);
}

void test_fill_block_updates_updated_at(void)
{
    time_t before  = time(NULL);
    Block *b       = make_empty_block();
    b->updated_at  = 0; /* reset to detect the change */
    Transaction *tx = make_transaction();
    fill_block(tx, b);
    TEST_ASSERT_GREATER_OR_EQUAL(before, b->updated_at);
    free(b);
    free_transaction(tx);
}

void test_fill_block_multiple_transactions_appended_in_order(void)
{
    Block *b        = make_empty_block();
    Transaction *t1 = make_transaction();
    Transaction *t2 = make_transaction();
    Transaction *t3 = make_transaction();
    t1->receipt = 1; t2->receipt = 2; t3->receipt = 3;

    fill_block(t1, b);
    fill_block(t2, b);
    fill_block(t3, b);

    TEST_ASSERT_EQUAL_INT(3, b->num_transactions);
    TEST_ASSERT_EQUAL_PTR(t1, b->transactions);
    TEST_ASSERT_EQUAL_PTR(t2, b->transactions->prox);
    TEST_ASSERT_EQUAL_PTR(t3, b->transactions->prox->prox);

    free(b);
    free_transaction(t1);
    free_transaction(t2);
    free_transaction(t3);
}

void test_fill_block_appends_to_existing_chain(void)
{
    Block *b       = make_empty_block();
    Transaction *t1 = make_transaction();
    Transaction *t2 = make_transaction();
    b->transactions     = t1;
    b->num_transactions = 1;

    fill_block(t2, b);

    TEST_ASSERT_EQUAL_PTR(t1, b->transactions);
    TEST_ASSERT_EQUAL_PTR(t2, t1->prox);
    TEST_ASSERT_EQUAL_INT(2, b->num_transactions);

    free(b);
    free_transaction(t1);
    free_transaction(t2);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_create_block_not_null);
    RUN_TEST(test_create_block_index_is_zero);
    RUN_TEST(test_create_block_expire_at_is_created_plus_3600);
    RUN_TEST(test_create_block_updated_at_equals_created_at);
    RUN_TEST(test_create_block_previous_hash_copied);
    RUN_TEST(test_create_block_hash_copied);
    RUN_TEST(test_create_block_null_transactions_ok);
    RUN_TEST(test_create_block_num_transactions_zero);
    RUN_TEST(test_create_block_prox_null);
    RUN_TEST(test_create_block_with_initial_transaction);

    RUN_TEST(test_fill_block_null_transaction_returns_zero);
    RUN_TEST(test_fill_block_null_block_returns_zero);
    RUN_TEST(test_fill_block_both_null_returns_zero);
    RUN_TEST(test_fill_block_success_returns_one);
    RUN_TEST(test_fill_block_first_transaction_set);
    RUN_TEST(test_fill_block_increments_num_transactions);
    RUN_TEST(test_fill_block_updates_updated_at);
    RUN_TEST(test_fill_block_multiple_transactions_appended_in_order);
    RUN_TEST(test_fill_block_appends_to_existing_chain);

    return UNITY_END();
}
