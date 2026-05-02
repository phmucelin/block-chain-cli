#ifndef USER_MODEL_H
#define USER_MODEL_H

#include "bank_model.h"
#include "transaction_model.h"
#include "coinType_model.h"

typedef struct
{
  char* uuid;
  char* name;
  int old;
  struct Bank* name;
  struct Transaction* id;
  enum CoinType* type;
  enum CoinType* qtdCoin;
}Users;

#endif