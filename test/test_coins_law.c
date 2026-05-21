#define UNITY_INCLUDE_DOUBLE
#define UNITY_DOUBLE_PRECISION 0.01

#include "../test/Unity/src/unity.h"
#include "../test/Unity/src/unity.c"

/*
 * coins_law.c chama getPreco() que e definida em coin_price.c (depende de
 * libcurl). Definimos aqui um mock com precos fixos para isolar o teste da
 * rede e da libcurl.
 *
 * Precos mock:  BTC = 45 000.0 USD | ETH = 2 500.0 USD | USDT = 1.0 USD
 */
#include "../models/coinType_model.h"

double getPreco(CoinType type)
{
    switch (type) {
        case BTC:  return 45000.0;
        case ETH:  return 2500.0;
        case USDT: return 1.0;
        default:   return 0.0;
    }
}

#include "../services/coins_law.c"

#include <stdlib.h>
#include <stdbool.h>

void setUp(void) {}
void tearDown(void) {}

/* ── helpers ── */

static Users *make_user_with_coin(CoinType type, double qty)
{
    Users *u  = (Users *)malloc(sizeof(Users));
    u->uuid   = "uuid-test";
    u->name   = "Alice";
    u->hashPass = "hash";
    u->age    = 25;
    u->bank   = NULL;
    u->transaction_id = NULL;
    u->prox   = NULL;

    UserCoin *c = (UserCoin *)malloc(sizeof(UserCoin));
    c->type    = type;
    c->qtdCoin = qty;
    c->prox    = NULL;
    u->coins   = c;
    return u;
}

static void free_user(Users *u)
{
    UserCoin *c = u->coins;
    while (c) {
        UserCoin *nx = c->prox;
        free(c);
        c = nx;
    }
    free(u);
}

/* ── merge_coins — guards ── */

void test_merge_null_user(void)
{
    TEST_ASSERT_FALSE(merge_coins(NULL, BTC, 1, ETH, 1));
}

void test_merge_user_coins_null(void)
{
    Users *u  = (Users *)malloc(sizeof(Users));
    u->coins  = NULL;
    u->prox   = NULL;
    TEST_ASSERT_FALSE(merge_coins(u, BTC, 1, ETH, 1));
    free(u);
}

void test_merge_zero_source_qty(void)
{
    Users *u = make_user_with_coin(BTC, 10);
    TEST_ASSERT_FALSE(merge_coins(u, BTC, 0, ETH, 1));
    free_user(u);
}

void test_merge_negative_source_qty(void)
{
    Users *u = make_user_with_coin(BTC, 10);
    TEST_ASSERT_FALSE(merge_coins(u, BTC, -3, ETH, 1));
    free_user(u);
}

void test_merge_zero_dest_qty(void)
{
    Users *u = make_user_with_coin(BTC, 10);
    TEST_ASSERT_FALSE(merge_coins(u, BTC, 1, ETH, 0));
    free_user(u);
}

void test_merge_negative_dest_qty(void)
{
    Users *u = make_user_with_coin(BTC, 10);
    TEST_ASSERT_FALSE(merge_coins(u, BTC, 1, ETH, -1));
    free_user(u);
}

void test_merge_same_source_and_dest_type(void)
{
    Users *u = make_user_with_coin(BTC, 10);
    TEST_ASSERT_FALSE(merge_coins(u, BTC, 1, BTC, 1));
    free_user(u);
}

/* ── merge_coins — source coin not found ── */

void test_merge_source_coin_absent(void)
{
    /* user has BTC only; try to merge from ETH */
    Users *u = make_user_with_coin(BTC, 10);
    TEST_ASSERT_FALSE(merge_coins(u, ETH, 1, USDT, 1));
    free_user(u);
}

/* ── merge_coins — insufficient balance ── */

void test_merge_source_qty_exceeds_balance(void)
{
    Users *u = make_user_with_coin(BTC, 1);
    TEST_ASSERT_FALSE(merge_coins(u, BTC, 5, ETH, 1));
    free_user(u);
}

void test_merge_total_value_less_than_required(void)
{
    /* 1 USDT ($1) cannot buy 1 BTC ($45000) */
    Users *u = make_user_with_coin(USDT, 1);
    TEST_ASSERT_FALSE(merge_coins(u, USDT, 1, BTC, 1));
    free_user(u);
}

/* ── merge_coins — price zero ── */

void test_merge_dest_price_zero_returns_false(void)
{
    /* CoinType 99 triggers the default: case in mock → 0.0 */
    Users *u = make_user_with_coin(BTC, 10);
    TEST_ASSERT_FALSE(merge_coins(u, BTC, 1, (CoinType)99, 1));
    free_user(u);
}

void test_merge_source_price_zero_returns_false(void)
{
    Users *u = make_user_with_coin((CoinType)99, 10);
    u->coins->type = (CoinType)99;
    u->coins->qtdCoin = 10;
    TEST_ASSERT_FALSE(merge_coins(u, (CoinType)99, 1, ETH, 1));
    free_user(u);
}

/* ── merge_coins — success path ── */

void test_merge_success_returns_true(void)
{
    /* 1 BTC ($45000) → enough for 1 ETH ($2500) */
    Users *u = make_user_with_coin(BTC, 1);
    TEST_ASSERT_TRUE(merge_coins(u, BTC, 1, ETH, 1));
    free_user(u);
}

void test_merge_source_coin_decremented(void)
{
    Users *u = make_user_with_coin(BTC, 5);
    merge_coins(u, BTC, 2, ETH, 1);
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 3.0, u->coins->qtdCoin);
    free_user(u);
}

void test_merge_dest_coin_created_when_absent(void)
{
    /* user has BTC only; ETH coin node must be created */
    Users *u = make_user_with_coin(BTC, 1);
    merge_coins(u, BTC, 1, ETH, 1);

    bool found = false;
    for (UserCoin *c = u->coins; c; c = c->prox) {
        if (c->type == ETH) { found = true; break; }
    }
    TEST_ASSERT_TRUE(found);
    free_user(u);
}

void test_merge_dest_amount_correct(void)
{
    /* 1 BTC ($45000) / ETH ($2500) = 18 ETH */
    Users *u = make_user_with_coin(BTC, 1);
    merge_coins(u, BTC, 1, ETH, 1);

    double eth_qty = 0.0;
    for (UserCoin *c = u->coins; c; c = c->prox) {
        if (c->type == ETH) { eth_qty = c->qtdCoin; break; }
    }
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 18.0, eth_qty);
    free_user(u);
}

void test_merge_existing_dest_coin_incremented(void)
{
    /* user already has 5 ETH; after merge it should gain more */
    Users *u  = make_user_with_coin(BTC, 1);
    UserCoin *eth = (UserCoin *)malloc(sizeof(UserCoin));
    eth->type    = ETH;
    eth->qtdCoin = 5.0;
    eth->prox    = NULL;
    u->coins->prox = eth;

    merge_coins(u, BTC, 1, ETH, 1);

    double eth_qty = 0.0;
    for (UserCoin *c = u->coins; c; c = c->prox) {
        if (c->type == ETH) { eth_qty = c->qtdCoin; break; }
    }
    TEST_ASSERT_GREATER_THAN(5.0, eth_qty);
    free_user(u);
}

void test_merge_btc_to_usdt(void)
{
    /* 1 BTC ($45000) → 45000 USDT ($1 each) */
    Users *u = make_user_with_coin(BTC, 1);
    bool result = merge_coins(u, BTC, 1, USDT, 1);
    TEST_ASSERT_TRUE(result);

    double usdt_qty = 0.0;
    for (UserCoin *c = u->coins; c; c = c->prox) {
        if (c->type == USDT) { usdt_qty = c->qtdCoin; break; }
    }
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 45000.0, usdt_qty);
    free_user(u);
}

void test_merge_eth_to_usdt(void)
{
    /* 1 ETH ($2500) → 2500 USDT */
    Users *u = make_user_with_coin(ETH, 1);
    merge_coins(u, ETH, 1, USDT, 1);

    double usdt_qty = 0.0;
    for (UserCoin *c = u->coins; c; c = c->prox) {
        if (c->type == USDT) { usdt_qty = c->qtdCoin; break; }
    }
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 2500.0, usdt_qty);
    free_user(u);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_merge_null_user);
    RUN_TEST(test_merge_user_coins_null);
    RUN_TEST(test_merge_zero_source_qty);
    RUN_TEST(test_merge_negative_source_qty);
    RUN_TEST(test_merge_zero_dest_qty);
    RUN_TEST(test_merge_negative_dest_qty);
    RUN_TEST(test_merge_same_source_and_dest_type);
    RUN_TEST(test_merge_source_coin_absent);
    RUN_TEST(test_merge_source_qty_exceeds_balance);
    RUN_TEST(test_merge_total_value_less_than_required);
    RUN_TEST(test_merge_dest_price_zero_returns_false);
    RUN_TEST(test_merge_source_price_zero_returns_false);
    RUN_TEST(test_merge_success_returns_true);
    RUN_TEST(test_merge_source_coin_decremented);
    RUN_TEST(test_merge_dest_coin_created_when_absent);
    RUN_TEST(test_merge_dest_amount_correct);
    RUN_TEST(test_merge_existing_dest_coin_incremented);
    RUN_TEST(test_merge_btc_to_usdt);
    RUN_TEST(test_merge_eth_to_usdt);

    return UNITY_END();
}
