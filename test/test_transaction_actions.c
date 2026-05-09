#include "../test/Unity/src/unity.h"
#include "../test/Unity/src/unity.c"
#include "../services/transaction_actions.c"

#include <string.h>
#include <stdlib.h>

/* stubs para funcoes referenciadas por send_to_block (nao testada aqui) */
bool check_transaction(Users *u, char *destine, int qtd_coins, CoinType *Type)
{
    (void)u; (void)destine; (void)qtd_coins; (void)Type;
    return false;
}
int fill_block(Transaction *transaction, Block *block)
{
    (void)transaction; (void)block;
    return 0;
}

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

void test_get_transaction_by_receipt_null_list(void)
{
    Transaction *found = get_transaction_by_receipt(NULL, 42);
    TEST_ASSERT_NULL(found);
}

void test_new_transaction_datetime_is_set(void)
{
    UserCoin *coin = (UserCoin *)malloc(sizeof(UserCoin));
    coin->qtdCoin = 1;
    coin->type    = BTC;
    coin->prox    = NULL;

    Transaction *tx = new_transaction("sender", "receiver", coin);

    /* year is stored as years-since-1900; any recent year is > 120 */
    TEST_ASSERT_GREATER_THAN(120, tx->datetime.tm_year);

    free(coin);
    free(tx);
}

void test_new_transaction_receipt_in_range(void)
{
    UserCoin *coin = (UserCoin *)malloc(sizeof(UserCoin));
    coin->qtdCoin = 1;
    coin->type    = ETH;
    coin->prox    = NULL;

    Transaction *tx = new_transaction("s", "r", coin);
    TEST_ASSERT_GREATER_OR_EQUAL(0, tx->receipt);
    TEST_ASSERT_LESS_THAN(1000002, tx->receipt);

    free(coin);
    free(tx);
}

void test_new_transaction_prox_null(void)
{
    UserCoin *coin = (UserCoin *)malloc(sizeof(UserCoin));
    coin->qtdCoin = 5;
    coin->type    = USDT;
    coin->prox    = NULL;

    Transaction *tx = new_transaction("s", "r", coin);
    TEST_ASSERT_NULL(tx->prox);

    free(coin);
    free(tx);
}

void test_delete_transaction_null_list_returns_null(void)
{
    Transaction *result = delete_transaction(NULL, 99);
    TEST_ASSERT_NULL(result);
}

void test_delete_transaction_only_element_returns_null(void)
{
    UserCoin *coin = (UserCoin *)malloc(sizeof(UserCoin));
    coin->qtdCoin = 1; coin->type = BTC; coin->prox = NULL;
    Transaction *tx = new_transaction("s", "r", coin);
    int receipt = tx->receipt;

    Transaction *result = delete_transaction(tx, receipt);
    TEST_ASSERT_NULL(result);

    free(coin);
    /* tx was freed inside delete_transaction */
}

void test_delete_transaction_middle_element(void)
{
    UserCoin *c1 = (UserCoin *)malloc(sizeof(UserCoin));
    UserCoin *c2 = (UserCoin *)malloc(sizeof(UserCoin));
    UserCoin *c3 = (UserCoin *)malloc(sizeof(UserCoin));
    c1->qtdCoin = 1; c1->type = BTC; c1->prox = NULL;
    c2->qtdCoin = 2; c2->type = ETH; c2->prox = NULL;
    c3->qtdCoin = 3; c3->type = USDT; c3->prox = NULL;

    Transaction *t1 = new_transaction("s1", "r1", c1);
    Transaction *t2 = new_transaction("s2", "r2", c2);
    Transaction *t3 = new_transaction("s3", "r3", c3);
    t1->prox = t2;
    t2->prox = t3;

    int mid_receipt = t2->receipt;
    Transaction *head = delete_transaction(t1, mid_receipt);

    TEST_ASSERT_EQUAL_PTR(t1, head);
    TEST_ASSERT_EQUAL_PTR(t3, head->prox);
    TEST_ASSERT_NULL(get_transaction_by_receipt(head, mid_receipt));

    free(c1); free(c2); free(c3);
    free(t1); free(t3);
    /* t2 freed inside delete_transaction */
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_new_transaction);
    RUN_TEST(test_new_transaction_null);
    RUN_TEST(test_new_transaction_datetime_is_set);
    RUN_TEST(test_new_transaction_receipt_in_range);
    RUN_TEST(test_new_transaction_prox_null);
    RUN_TEST(test_get_transaction_by_receipt);
    RUN_TEST(test_get_transaction_by_receipt_not_found);
    RUN_TEST(test_get_transaction_by_receipt_null_list);
    RUN_TEST(test_delete_transaction);
    RUN_TEST(delete_transaction_not_found);
    RUN_TEST(test_delete_transaction_null_list_returns_null);
    RUN_TEST(test_delete_transaction_only_element_returns_null);
    RUN_TEST(test_delete_transaction_middle_element);
    return UNITY_END();
}