#ifndef USER_MODEL_H
#define USER_MODEL_H

#include "bank_model.h"
#include "coinType_model.h"

struct Transaction;

typedef struct Coin{
    enum CoinType type;
    double qtdCoin;
  struct Coin* prox;
} UserCoin;

typedef struct Users
{
  char* uuid;
  char* cpf;
  char* name;
  char* hashPass;
  int age;
  double balance;
  struct Bank* bank;
  struct Transaction* transaction_id;
  UserCoin* coins;
  struct Users* prox;
} Users;

typedef struct pos
{
    struct Users* user;
    int ocupado;
} pos;

#endif