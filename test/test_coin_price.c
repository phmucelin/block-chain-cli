#define UNITY_INCLUDE_DOUBLE
#define UNITY_DOUBLE_PRECISION 0.001

#include "../test/Unity/src/unity.h"
#include "../test/Unity/src/unity.c"
#include "../services/coin_price.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setUp(void)
{
    /* zera precoMercado entre testes */
    for (int i = 0; i < NUM_COINS; i++)
        precoMercado[i] = 0.0;
}

void tearDown(void) {}

/* ── extrairPreco ── */

void test_extrairPreco_null_returns_minus_one(void)
{
    TEST_ASSERT_DOUBLE_WITHIN(0.001, -1.0, extrairPreco(NULL));
}

void test_extrairPreco_empty_string_returns_minus_one(void)
{
    TEST_ASSERT_DOUBLE_WITHIN(0.001, -1.0, extrairPreco(""));
}

void test_extrairPreco_no_usd_key_returns_minus_one(void)
{
    TEST_ASSERT_DOUBLE_WITHIN(0.001, -1.0, extrairPreco("{\"eur\":45000.0}"));
}

void test_extrairPreco_valid_json_returns_price(void)
{
    double p = extrairPreco("{\"bitcoin\":{\"usd\":45000.5}}");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 45000.5, p);
}

void test_extrairPreco_integer_price(void)
{
    double p = extrairPreco("{\"ethereum\":{\"usd\":2500}}");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 2500.0, p);
}

void test_extrairPreco_price_zero_returns_minus_one(void)
{
    /* preco <= 0 deve ser rejeitado */
    TEST_ASSERT_DOUBLE_WITHIN(0.001, -1.0, extrairPreco("{\"x\":{\"usd\":0}}"));
}

void test_extrairPreco_price_negative_returns_minus_one(void)
{
    TEST_ASSERT_DOUBLE_WITHIN(0.001, -1.0, extrairPreco("{\"x\":{\"usd\":-100}}"));
}

void test_extrairPreco_malformed_value_returns_minus_one(void)
{
    TEST_ASSERT_DOUBLE_WITHIN(0.001, -1.0, extrairPreco("{\"usd\":abc}"));
}

void test_extrairPreco_usdt_one_dollar(void)
{
    double p = extrairPreco("{\"tether\":{\"usd\":1.0}}");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 1.0, p);
}

/* ── extrairPrecoBTC (alias) ── */

void test_extrairPrecoBTC_delegates_correctly(void)
{
    double p = extrairPrecoBTC("{\"bitcoin\":{\"usd\":30000.0}}");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 30000.0, p);
}

void test_extrairPrecoBTC_null_returns_minus_one(void)
{
    TEST_ASSERT_DOUBLE_WITHIN(0.001, -1.0, extrairPrecoBTC(NULL));
}

/* ── write_callback ── */

void test_write_callback_returns_correct_size(void)
{
    Memory mem;
    mem.data = malloc(1);
    mem.data[0] = '\0';
    mem.size = 0;

    const char *chunk = "hello";
    size_t written = write_callback((void *)chunk, 1, 5, &mem);

    TEST_ASSERT_EQUAL_size_t(5, written);

    free(mem.data);
}

void test_write_callback_appends_data(void)
{
    Memory mem;
    mem.data = malloc(1);
    mem.data[0] = '\0';
    mem.size = 0;

    const char *chunk = "world";
    write_callback((void *)chunk, 1, 5, &mem);

    TEST_ASSERT_EQUAL_STRING("world", mem.data);

    free(mem.data);
}

void test_write_callback_accumulates_chunks(void)
{
    Memory mem;
    mem.data = malloc(1);
    mem.data[0] = '\0';
    mem.size = 0;

    write_callback((void *)"foo", 1, 3, &mem);
    write_callback((void *)"bar", 1, 3, &mem);

    TEST_ASSERT_EQUAL_STRING("foobar", mem.data);
    TEST_ASSERT_EQUAL_size_t(6, mem.size);

    free(mem.data);
}

void test_write_callback_null_terminates(void)
{
    Memory mem;
    mem.data = malloc(1);
    mem.data[0] = '\0';
    mem.size = 0;

    write_callback((void *)"ok", 1, 2, &mem);

    TEST_ASSERT_EQUAL_CHAR('\0', mem.data[2]);

    free(mem.data);
}

/* ── getPreco ── */

void test_getPreco_invalid_negative_type_returns_minus_one(void)
{
    TEST_ASSERT_DOUBLE_WITHIN(0.001, -1.0, getPreco((CoinType)-1));
}

void test_getPreco_invalid_num_coins_returns_minus_one(void)
{
    TEST_ASSERT_DOUBLE_WITHIN(0.001, -1.0, getPreco(NUM_COINS));
}

void test_getPreco_btc_initial_zero(void)
{
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 0.0, getPreco(BTC));
}

void test_getPreco_reflects_precoMercado(void)
{
    precoMercado[ETH] = 1234.56;
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 1234.56, getPreco(ETH));
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_extrairPreco_null_returns_minus_one);
    RUN_TEST(test_extrairPreco_empty_string_returns_minus_one);
    RUN_TEST(test_extrairPreco_no_usd_key_returns_minus_one);
    RUN_TEST(test_extrairPreco_valid_json_returns_price);
    RUN_TEST(test_extrairPreco_integer_price);
    RUN_TEST(test_extrairPreco_price_zero_returns_minus_one);
    RUN_TEST(test_extrairPreco_price_negative_returns_minus_one);
    RUN_TEST(test_extrairPreco_malformed_value_returns_minus_one);
    RUN_TEST(test_extrairPreco_usdt_one_dollar);

    RUN_TEST(test_extrairPrecoBTC_delegates_correctly);
    RUN_TEST(test_extrairPrecoBTC_null_returns_minus_one);

    RUN_TEST(test_write_callback_returns_correct_size);
    RUN_TEST(test_write_callback_appends_data);
    RUN_TEST(test_write_callback_accumulates_chunks);
    RUN_TEST(test_write_callback_null_terminates);

    RUN_TEST(test_getPreco_invalid_negative_type_returns_minus_one);
    RUN_TEST(test_getPreco_invalid_num_coins_returns_minus_one);
    RUN_TEST(test_getPreco_btc_initial_zero);
    RUN_TEST(test_getPreco_reflects_precoMercado);

    return UNITY_END();
}
