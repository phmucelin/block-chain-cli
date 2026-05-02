#ifndef USER_MODEL_H
#define USER_MODEL_H

#include "bank_model.h"
#include "transaction_model.h"
#include "coinType_model.h"

typedef struct {
    enum CoinType type;
    int qtdCoin;
} UserCoin;

typedef struct
{
  char* uuid;
  char* name;
  int age;
  struct Bank* bank;
  struct Transaction* id;
  UserCoin* coins; // array de moedas do usuário
  int num_coins;   // quantidade de moedas diferentes
}Users;

#endif