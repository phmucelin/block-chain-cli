#ifndef TRANSACTION_MODEL_H
#define TRANSACTION_MODEL_H

#include <time.h>
#include "coinType_model.h"
#include "user_model.h"


typedef struct Transaction
{
    char* uuidSender;
    char* uuidReceive;
    struct tm datetime; // hora detalhada
    UserCoin* coin;
    int receipt; // id de recibo
    Transaction* prox; // ponteiro para a próxima transação (para encadeamento)
}Transaction;

#endif