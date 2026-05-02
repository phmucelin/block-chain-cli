#ifndef USER_MODEL_H
#define USER_MODEL_H

#include "bank_model.h"

typedef struct
{
  char* uuid;
  char* name;
  int old;
  // transactions (ID) + QTD
  // typecoins que tem + QTD
}Users;

#endif