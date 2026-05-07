#ifndef TRANSACTION_MODEL_H
#define TRANSACTION_MODEL_H

#include <time.h>
#include "coinType_model.h"

typedef struct Coin UserCoin;


typedef struct Transaction
{
    char* uuidSender;
    char* uuidReceive;
    struct tm datetime; // hora detalhada
    UserCoin* coin;
    int receipt; // id de recibo
    struct Transaction* prox; // ponteiro para a próxima transação (para encadeamento)
}Transaction;

#endif