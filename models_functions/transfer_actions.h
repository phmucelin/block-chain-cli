#ifndef TRANSFER_ACTIONS_H
#define TRANSFER_ACTIONS_H

#include "coin_price_api.h"

typedef struct User User;
typedef struct Bank Bank;
typedef struct Exchange Exchange;

bool check_amount(User* u, double amount_transfer);

bool recharge_money(User* u, double amount_recharge, Bank* b);

bool withdraw_money(User* u, double amount_withdraw, Bank* b);

bool transfer_exchange(User* u, double amount_transfer, Exchange* e);

bool transaction_users(User* u, char* destine, CoinType* Type, int qtd_coins); // Destine == UUID

bool check_transaction(User* u, char* destine, int qtd_coins, CoinType* Type); // Dentro dela ja iremos chamar check amount, com base no qtd de coins e tipo de coin. Faz a conta de qt tem e se pode.


#endif