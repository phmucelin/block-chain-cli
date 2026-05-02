#ifndef TRANSACTION_MODEL_H
#define TRANSACTION_MODEL_H

#include <time.h>
#include "coinType_model.h"


typedef struct Transaction
{
    char* uuidSender;
    char* uuidReceive;
    struct tm datetime; // hora detalhada
    enum CoinType* type;
    enum CoinType* qtdCoin;
    int receipt; // id de recibo

}Transaction;

#endif