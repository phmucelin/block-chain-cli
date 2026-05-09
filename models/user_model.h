#ifndef USER_MODEL_H
#define USER_MODEL_H

#include "bank_model.h"
#include "coinType_model.h"

struct Transaction;

typedef struct Coin{
    enum CoinType type;
    int qtdCoin;
  struct Coin* prox;
} UserCoin;

typedef struct Users
{
  char* uuid;
  char* name;
  char* hashPass;
  int age;
  struct Bank* bank;
  struct Transaction* transaction_id;
  UserCoin* coins;
  struct Users* prox;
} Users;

#endif