#ifndef BLOCK_MODEL_H
#define BLOCK_MODEL_H

#include <time.h>
#include "transaction_model.h"


typedef struct Block {
    int index;
    time_t created_at; // time para controlarmos o tempo em que um bloco deverá ser salvo automaticamente dentro da BlockChain
    struct Transaction* transactions; // ponteiro para lista de transações
    int num_transactions;
    char* previous_hash;
    char* hash;
    struct Block* prox; 
} Block;

typedef struct BlockChain {
    char uuid[37];
    char hash[65];
    struct Block* first_block;
    struct BlockChain* prox;
} BlockChain;

#endif
