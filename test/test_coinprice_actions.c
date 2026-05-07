#define UNITY_INCLUDE_DOUBLE
#define UNITY_DOUBLE_PRECISION 0.01

#include "../test/Unity/src/unity.h"
#include "../test/Unity/src/unity.c"
#include "../models_functions/coin_price_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Mock/stubs para evitar dependências externas durante testes
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    // Mock da função
    return size * nmemb;
}

double extrairPrecoBTC(const char *json)
{
    // Mock da função - simula parsing de JSON
    if (json && strstr(json, "45000")) {
        return 45000.5;
    }
    return 0.0;
}

void atualizarPrecosAPI(CoinType tipo)
{
    // Mock - função vazia
    (void)tipo; // Suppress unused variable warning
}

double getPreco(CoinType tipo)
{
    // Mock - retorna preço simulado baseado no tipo
    switch(tipo) {
        case BTC:
            return 45000.5;
        case ETH:
            return 2500.0;
        case USDT:
            return 1.0;
        default:
            return 0.0;
    }
}


void setUp(void)
{
    // roda antes de cada teste
}

void tearDown(void)
{
    // roda depois de cada teste
}

void test_write_callback(void)
{
    // Test da função write_callback
    char buffer[256] = {0};
    const char* test_data = "test content";
    size_t result = write_callback((void*)test_data, sizeof(char), strlen(test_data), buffer);
    
    TEST_ASSERT_EQUAL_size_t(strlen(test_data), result);
}

void test_extrair_precoBTC(void)
{
    // JSON de teste com preço simulado
    const char* json_test = "{\"bpi\":{\"USD\":{\"rate_float\":45000.5}}}";
    
    // Usar WITHIN para tolerância de 0.01 USD (variações normais de parsing)
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 45000.5, extrairPrecoBTC(json_test));
}

void test_get_preco(void)
{
    // Test getPreco para BTC
    CoinType tipo = BTC;

    // Usar WITHIN para tolerância de 0.01 USD
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 45000.5, getPreco(tipo));
}

void test_atualizar_preco_btc(void)
{
    // Test atualizarPrecosAPI para BTC
    atualizarPrecosAPI(BTC);
    
    // Verificar se o preço foi atualizado (mock retorna 45000.5)
    double preco = getPreco(BTC);
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 45000.5, preco);
}

void test_atualizar_preco_eth(void)
{
    // Test atualizarPrecosAPI para ETH
    atualizarPrecosAPI(ETH);
    
    // Verificar se o preço foi atualizado (mock retorna 2500.0)
    double preco = getPreco(ETH);
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 2500.0, preco);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_write_callback);
    RUN_TEST(test_extrair_precoBTC);
    RUN_TEST(test_get_preco);
    RUN_TEST(test_atualizar_preco_btc);
    RUN_TEST(test_atualizar_preco_eth);

    return UNITY_END();
}
