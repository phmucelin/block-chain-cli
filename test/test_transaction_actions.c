#include "../test/Unity/src/unity.h"
#include "../test/Unity/src/unity.c"
#include "../services/transaction_actions.c"

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

void test_new_transaction(void)
{
    UserCoin *coin = (UserCoin *)malloc(sizeof(UserCoin));
    coin->qtdCoin = 100;
    coin->type = BTC;

    Transaction *transaction = new_transaction("uuidSender", "uuidReceive", coin);

    TEST_ASSERT_NOT_NULL(transaction);
    TEST_ASSERT_EQUAL_STRING("uuidSender", transaction->uuidSender);
    TEST_ASSERT_EQUAL_STRING("uuidReceive", transaction->uuidReceive);
    TEST_ASSERT_EQUAL_INT(100, transaction->coin->qtdCoin);
    TEST_ASSERT_EQUAL_INT(BTC, transaction->coin->type);

    free(coin);
    free(transaction);
}

void test_new_transaction_null(void)
{
    Transaction *transaction = new_transaction("uuidSender", "uuidReceive", NULL);
    Transaction *transaction_full_null = new_transaction(NULL, NULL, NULL);
    Transaction *transaction_user1_null = new_transaction("user1", NULL, NULL);
    Transaction *transaction_user2_null = new_transaction(NULL, "user2", NULL);

    TEST_ASSERT_NULL(transaction_full_null);
    TEST_ASSERT_NULL(transaction);
    TEST_ASSERT_NULL(transaction_user1_null);
    TEST_ASSERT_NULL(transaction_user2_null);
}

void test_get_transaction_by_receipt(void)
{
    UserCoin *coin1 = (UserCoin *)malloc(sizeof(UserCoin));
    coin1->qtdCoin = 100;
    coin1->type = BTC;

    UserCoin *coin2 = (UserCoin *)malloc(sizeof(UserCoin));
    coin2->qtdCoin = 50;
    coin2->type = ETH;

    Transaction *transaction1 = new_transaction("uuidSender1", "uuidReceive1", coin1);
    Transaction *transaction2 = new_transaction("uuidSender2", "uuidReceive2", coin2);

    transaction1->prox = transaction2; // Encadeando as transações

    Transaction *found_transaction = get_transaction_by_receipt(transaction1, transaction2->receipt);

    TEST_ASSERT_NOT_NULL(found_transaction);
    TEST_ASSERT_EQUAL_STRING("uuidSender2", found_transaction->uuidSender);
    TEST_ASSERT_EQUAL_STRING("uuidReceive2", found_transaction->uuidReceive);
    TEST_ASSERT_EQUAL_INT(50, found_transaction->coin->qtdCoin);
    TEST_ASSERT_EQUAL_INT(ETH, found_transaction->coin->type);

    free(coin1);
    free(coin2);
    free(transaction1);
    free(transaction2);
}

void test_get_transaction_by_receipt_not_found(void)
{
    UserCoin *coin1 = (UserCoin *)malloc(sizeof(UserCoin));
    coin1->qtdCoin = 100;
    coin1->type = BTC;

    Transaction *transaction1 = new_transaction("uuidSender1", "uuidReceive1", coin1);

    Transaction *found_transaction = get_transaction_by_receipt(transaction1, 999999); // Receipt inexistente

    TEST_ASSERT_NULL(found_transaction); // A transação não deve ser encontrada

    free(coin1);
    free(transaction1);
}

void test_delete_transaction(void)
{
    UserCoin *coin1 = (UserCoin *)malloc(sizeof(UserCoin));
    coin1->qtdCoin = 100;
    coin1->type = BTC;

    UserCoin *coin2 = (UserCoin *)malloc(sizeof(UserCoin));
    coin2->qtdCoin = 50;
    coin2->type = ETH;

    Transaction *transaction1 = new_transaction("uuidSender1", "uuidReceive1", coin1);
    Transaction *transaction2 = new_transaction("uuidSender2", "uuidReceive2", coin2);

    transaction1->prox = transaction2; // Encadeando as transações

    Transaction *updated_transactions = delete_transaction(transaction1, transaction1->receipt);

    Transaction *found_transaction = get_transaction_by_receipt(updated_transactions, transaction1->receipt);

    TEST_ASSERT_NULL(found_transaction); // A transação deletada não deve ser encontrada
    TEST_ASSERT_NOT_NULL(updated_transactions);
    TEST_ASSERT_EQUAL_STRING("uuidSender2", updated_transactions->uuidSender);
    TEST_ASSERT_EQUAL_STRING("uuidReceive2", updated_transactions->uuidReceive);
    TEST_ASSERT_EQUAL_INT(50, updated_transactions->coin->qtdCoin);
    TEST_ASSERT_EQUAL_INT(ETH, updated_transactions->coin->type);

    free(coin1);
    free(coin2);
    free(updated_transactions);
}

void delete_transaction_not_found(void)
{
    UserCoin *coin1 = (UserCoin *)malloc(sizeof(UserCoin));
    coin1->qtdCoin = 100;
    coin1->type = BTC;

    Transaction *transaction1 = new_transaction("uuidSender1", "uuidReceive1", coin1);

    Transaction *updated_transactions = delete_transaction(transaction1, 999999); // Receipt inexistente

    TEST_ASSERT_NOT_NULL(updated_transactions); // A lista de transações deve permanecer inalterada
    TEST_ASSERT_EQUAL_STRING("uuidSender1", updated_transactions->uuidSender);
    TEST_ASSERT_EQUAL_STRING("uuidReceive1", updated_transactions->uuidReceive);
    TEST_ASSERT_EQUAL_INT(100, updated_transactions->coin->qtdCoin);
    TEST_ASSERT_EQUAL_INT(BTC, updated_transactions->coin->type);

    free(coin1);
    free(updated_transactions);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_new_transaction);
    RUN_TEST(test_new_transaction_null);
    RUN_TEST(test_get_transaction_by_receipt);
    RUN_TEST(test_get_transaction_by_receipt_not_found);
    RUN_TEST(test_delete_transaction);
    RUN_TEST(delete_transaction_not_found);
    return UNITY_END();
}