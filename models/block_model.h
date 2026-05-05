#ifndef BLOCK_MODEL_H
#define BLOCK_MODEL_H

#include <time.h>
#include "transaction_model.h"

typedef struct Block {
    int index;
    struct tm timestamp;
    struct Transaction* transactions; // ponteiro para lista de transações
    int num_transactions;
    char* previous_hash;
    char* hash;
    struct Block* prox;
} Block;

#endif
