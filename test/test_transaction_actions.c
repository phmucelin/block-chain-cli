#include "../test/Unity/src/unity.h"
#include "../test/Unity/src/unity.c"

#include <string.h>
#include <stdlib.h>
#include "../models/user_model.h"
#include "../models/block_model.h"
#include "../models/transaction_model.h"
#include "../models/coinType_model.h"

/* stubs declarados antes do include para evitar implicit-declaration */
int fill_block(Transaction *transaction, Block *block);
struct Users *get_user_by_uuid(struct Users *users, char *uuid);

#include "../services/transaction_actions.c"

static int fill_block_stub_return = 1;
int fill_block(Transaction *transaction, Block *block)
{
    (void)transaction; (void)block;
    return fill_block_stub_return;
}

static Users stub_sender   = { "sender-uuid",   "Sender",   NULL, 25, NULL, NULL, NULL, NULL };
static Users stub_receiver = { "receiver-uuid", "Receiver", NULL, 25, NULL, NULL, NULL, NULL };
Users *get_user_by_uuid(Users *users, char *uuid)
{
    if (users == NULL || uuid == NULL) return NULL;
    if (strcmp(uuid, "sender-uuid")   == 0) return &stub_sender;
    if (strcmp(uuid, "receiver-uuid") == 0) return &stub_receiver;
    return NULL;
}

static void free_queue(Queue *queue)
{
    if (queue == NULL)
    {
        return;
    }

    Transaction *current = queue->first;
    while (current != NULL)
    {
        Transaction *next = current->prox;
        free(current);
        current = next;
    }

    queue->first = NULL;
    queue->last = NULL;
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

    Queue queue = {0};
    Transaction *transaction = new_transaction("uuidSender", "uuidReceive", coin, &queue);

    TEST_ASSERT_NOT_NULL(transaction);
    TEST_ASSERT_EQUAL_STRING("uuidSender", transaction->uuidSender);
    TEST_ASSERT_EQUAL_STRING("uuidReceive", transaction->uuidReceive);
    TEST_ASSERT_EQUAL_INT(100, transaction->coin->qtdCoin);
    TEST_ASSERT_EQUAL_INT(BTC, transaction->coin->type);

    free(coin);
    free_queue(&queue);
}

void test_new_transaction_null(void)
{
    Queue queue = {0};
    Transaction *transaction = new_transaction("uuidSender", "uuidReceive", NULL, &queue);
    Transaction *transaction_full_null = new_transaction(NULL, NULL, NULL, &queue);
    Transaction *transaction_user1_null = new_transaction("user1", NULL, NULL, &queue);
    Transaction *transaction_user2_null = new_transaction(NULL, "user2", NULL, &queue);

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

    Queue queue = {0};
    Transaction *transaction1 = new_transaction("uuidSender1", "uuidReceive1", coin1, &queue);
    Transaction *transaction2 = new_transaction("uuidSender2", "uuidReceive2", coin2, &queue);

    Transaction *found_transaction = get_transaction_by_receipt(&queue, transaction2->receipt);

    TEST_ASSERT_NOT_NULL(found_transaction);
    TEST_ASSERT_EQUAL_STRING("uuidSender2", found_transaction->uuidSender);
    TEST_ASSERT_EQUAL_STRING("uuidReceive2", found_transaction->uuidReceive);
    TEST_ASSERT_EQUAL_INT(50, found_transaction->coin->qtdCoin);
    TEST_ASSERT_EQUAL_INT(ETH, found_transaction->coin->type);

    free(coin1);
    free(coin2);
    free_queue(&queue);
}

void test_get_transaction_by_receipt_not_found(void)
{
    UserCoin *coin1 = (UserCoin *)malloc(sizeof(UserCoin));
    coin1->qtdCoin = 100;
    coin1->type = BTC;

    Queue queue = {0};
    new_transaction("uuidSender1", "uuidReceive1", coin1, &queue);

    Transaction *found_transaction = get_transaction_by_receipt(&queue, 999999); // Receipt inexistente

    TEST_ASSERT_NULL(found_transaction); // A transação não deve ser encontrada

    free(coin1);
    free_queue(&queue);
}

void test_delete_transaction(void)
{
    UserCoin *coin1 = (UserCoin *)malloc(sizeof(UserCoin));
    coin1->qtdCoin = 100;
    coin1->type = BTC;

    UserCoin *coin2 = (UserCoin *)malloc(sizeof(UserCoin));
    coin2->qtdCoin = 50;
    coin2->type = ETH;

    Queue queue = {0};
    Transaction *transaction1 = new_transaction("uuidSender1", "uuidReceive1", coin1, &queue);
    new_transaction("uuidSender2", "uuidReceive2", coin2, &queue);

    int deleted = delete_transaction(&queue, transaction1->receipt);

    Transaction *found_transaction = get_transaction_by_receipt(&queue, transaction1->receipt);

    TEST_ASSERT_NULL(found_transaction); // A transação deletada não deve ser encontrada
    TEST_ASSERT_EQUAL_INT(1, deleted);
    TEST_ASSERT_NOT_NULL(queue.first);
    TEST_ASSERT_EQUAL_STRING("uuidSender2", queue.first->uuidSender);
    TEST_ASSERT_EQUAL_STRING("uuidReceive2", queue.first->uuidReceive);
    TEST_ASSERT_EQUAL_INT(50, queue.first->coin->qtdCoin);
    TEST_ASSERT_EQUAL_INT(ETH, queue.first->coin->type);

    free(coin1);
    free(coin2);
    free_queue(&queue);
}

void delete_transaction_not_found(void)
{
    UserCoin *coin1 = (UserCoin *)malloc(sizeof(UserCoin));
    coin1->qtdCoin = 100;
    coin1->type = BTC;

    Queue queue = {0};
    new_transaction("uuidSender1", "uuidReceive1", coin1, &queue);

    int deleted = delete_transaction(&queue, 999999); // Receipt inexistente

    TEST_ASSERT_EQUAL_INT(0, deleted); // A lista de transações deve permanecer inalterada
    TEST_ASSERT_NOT_NULL(queue.first);
    TEST_ASSERT_EQUAL_STRING("uuidSender1", queue.first->uuidSender);
    TEST_ASSERT_EQUAL_STRING("uuidReceive1", queue.first->uuidReceive);
    TEST_ASSERT_EQUAL_INT(100, queue.first->coin->qtdCoin);
    TEST_ASSERT_EQUAL_INT(BTC, queue.first->coin->type);

    free(coin1);
    free_queue(&queue);
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

    Queue queue = {0};
    Transaction *tx = new_transaction("sender", "receiver", coin, &queue);

    /* year is stored as years-since-1900; any recent year is > 120 */
    TEST_ASSERT_GREATER_THAN(120, tx->datetime.tm_year);

    free(coin);
    free_queue(&queue);
}

void test_new_transaction_receipt_in_range(void)
{
    UserCoin *coin = (UserCoin *)malloc(sizeof(UserCoin));
    coin->qtdCoin = 1;
    coin->type    = ETH;
    coin->prox    = NULL;

    Queue queue = {0};
    Transaction *tx = new_transaction("s", "r", coin, &queue);
    TEST_ASSERT_GREATER_OR_EQUAL(0, tx->receipt);
    TEST_ASSERT_LESS_THAN(1000002, tx->receipt);

    free(coin);
    free_queue(&queue);
}

void test_new_transaction_prox_null(void)
{
    UserCoin *coin = (UserCoin *)malloc(sizeof(UserCoin));
    coin->qtdCoin = 5;
    coin->type    = USDT;
    coin->prox    = NULL;

    Queue queue = {0};
    Transaction *tx = new_transaction("s", "r", coin, &queue);
    TEST_ASSERT_NULL(tx->prox);

    free(coin);
    free_queue(&queue);
}

void test_delete_transaction_null_list_returns_null(void)
{
    int result = delete_transaction(NULL, 99);
    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_delete_transaction_only_element_returns_null(void)
{
    UserCoin *coin = (UserCoin *)malloc(sizeof(UserCoin));
    coin->qtdCoin = 1; coin->type = BTC; coin->prox = NULL;
    Queue queue = {0};
    Transaction *tx = new_transaction("s", "r", coin, &queue);
    int receipt = tx->receipt;

    int result = delete_transaction(&queue, receipt);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_NULL(queue.first);

    free(coin);
}

void test_delete_transaction_middle_element(void)
{
    UserCoin *c1 = (UserCoin *)malloc(sizeof(UserCoin));
    UserCoin *c2 = (UserCoin *)malloc(sizeof(UserCoin));
    UserCoin *c3 = (UserCoin *)malloc(sizeof(UserCoin));
    c1->qtdCoin = 1; c1->type = BTC; c1->prox = NULL;
    c2->qtdCoin = 2; c2->type = ETH; c2->prox = NULL;
    c3->qtdCoin = 3; c3->type = USDT; c3->prox = NULL;

    Queue queue = {0};
    Transaction *t1 = new_transaction("s1", "r1", c1, &queue);
    Transaction *t2 = new_transaction("s2", "r2", c2, &queue);
    Transaction *t3 = new_transaction("s3", "r3", c3, &queue);

    int mid_receipt = t2->receipt;
    int deleted = delete_transaction(&queue, mid_receipt);

    TEST_ASSERT_EQUAL_INT(1, deleted);
    TEST_ASSERT_EQUAL_PTR(t1, queue.first);
    TEST_ASSERT_EQUAL_PTR(t3, queue.first->prox);
    TEST_ASSERT_NULL(get_transaction_by_receipt(&queue, mid_receipt));

    free(c1); free(c2); free(c3);
    free_queue(&queue);
}

/* ── helpers para can_accept / send_to_block ── */

static Block *make_block_expiring_at(time_t expire)
{
    Block *b = (Block *)malloc(sizeof(Block));
    b->index            = 0;
    b->created_at       = expire - 3600;
    b->updated_at       = expire - 3600;
    b->expire_at        = expire;
    b->transactions     = NULL;
    b->num_transactions = 0;
    b->prox             = NULL;
    memset(b->previous_hash, 'a', 64); b->previous_hash[64] = '\0';
    memset(b->hash,          'b', 64); b->hash[64]          = '\0';
    return b;
}

static UserCoin *make_coin(void)
{
    UserCoin *c = (UserCoin *)malloc(sizeof(UserCoin));
    c->type    = BTC;
    c->qtdCoin = 10;
    c->prox    = NULL;
    return c;
}

/* ── can_accept_transaction ── */

void test_can_accept_null_block_returns_zero(void)
{
    TEST_ASSERT_EQUAL_INT(0, can_accept_transaction(NULL, time(NULL)));
}

void test_can_accept_not_expired_returns_one(void)
{
    time_t now    = time(NULL);
    Block *b      = make_block_expiring_at(now + 3600);
    TEST_ASSERT_EQUAL_INT(1, can_accept_transaction(b, now));
    free(b);
}

void test_can_accept_expired_returns_zero(void)
{
    time_t now = time(NULL);
    Block *b   = make_block_expiring_at(now - 1);
    TEST_ASSERT_EQUAL_INT(0, can_accept_transaction(b, now));
    free(b);
}

void test_can_accept_exactly_at_expiry_returns_zero(void)
{
    time_t now = time(NULL);
    Block *b   = make_block_expiring_at(now);
    TEST_ASSERT_EQUAL_INT(0, can_accept_transaction(b, now));
    free(b);
}

/* ── send_to_block ── */

void test_send_to_block_null_users_returns_zero(void)
{
    Queue queue  = {0};
    Block *block = make_block_expiring_at(time(NULL) + 3600);
    TEST_ASSERT_EQUAL_INT(0, send_to_block(NULL, &queue, block, time(NULL)));
    free(block);
}

void test_send_to_block_null_queue_returns_zero(void)
{
    Users  users = {0};
    Block *block = make_block_expiring_at(time(NULL) + 3600);
    TEST_ASSERT_EQUAL_INT(0, send_to_block(&users, NULL, block, time(NULL)));
    free(block);
}

void test_send_to_block_null_block_returns_zero(void)
{
    Users  users = {0};
    Queue  queue = {0};
    TEST_ASSERT_EQUAL_INT(0, send_to_block(&users, &queue, NULL, time(NULL)));
}

void test_send_to_block_expired_block_returns_zero(void)
{
    Users  users = {0};
    Queue  queue = {0};
    time_t now   = time(NULL);
    Block *block = make_block_expiring_at(now - 1);
    TEST_ASSERT_EQUAL_INT(0, send_to_block(&users, &queue, block, now));
    free(block);
}

void test_send_to_block_empty_queue_returns_zero(void)
{
    Users  users = {0};
    Queue  queue = {0};
    time_t now   = time(NULL);
    Block *block = make_block_expiring_at(now + 3600);
    TEST_ASSERT_EQUAL_INT(0, send_to_block(&users, &queue, block, now));
    free(block);
}

void test_send_to_block_valid_transaction_success(void)
{
    /* sender e receiver existem via get_user_by_uuid stub */
    Users  users = {0};
    Queue  queue = {0};
    time_t now   = time(NULL);
    Block *block = make_block_expiring_at(now + 3600);

    UserCoin   *coin = make_coin();
    Transaction *tx  = new_transaction("sender-uuid", "receiver-uuid", coin, &queue);
    (void)tx;

    fill_block_stub_return = 1;
    int result = send_to_block(&users, &queue, block, now);
    TEST_ASSERT_EQUAL_INT(1, result);

    free(block);
    free(coin);
    free_queue(&queue);
}

void test_send_to_block_unknown_sender_returns_zero(void)
{
    Users  users = {0};
    Queue  queue = {0};
    time_t now   = time(NULL);
    Block *block = make_block_expiring_at(now + 3600);

    UserCoin   *coin = make_coin();
    Transaction *tx  = new_transaction("unknown-uuid", "receiver-uuid", coin, &queue);
    (void)tx;

    int result = send_to_block(&users, &queue, block, now);
    TEST_ASSERT_EQUAL_INT(0, result);

    free(block);
    free(coin);
    free_queue(&queue);
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

    RUN_TEST(test_can_accept_null_block_returns_zero);
    RUN_TEST(test_can_accept_not_expired_returns_one);
    RUN_TEST(test_can_accept_expired_returns_zero);
    RUN_TEST(test_can_accept_exactly_at_expiry_returns_zero);

    RUN_TEST(test_send_to_block_null_users_returns_zero);
    RUN_TEST(test_send_to_block_null_queue_returns_zero);
    RUN_TEST(test_send_to_block_null_block_returns_zero);
    RUN_TEST(test_send_to_block_expired_block_returns_zero);
    RUN_TEST(test_send_to_block_empty_queue_returns_zero);
    RUN_TEST(test_send_to_block_valid_transaction_success);
    RUN_TEST(test_send_to_block_unknown_sender_returns_zero);
    return UNITY_END();
}