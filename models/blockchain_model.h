#ifndef BLOCKCHAIN_MODEL_H
#define BLOCKCHAIN_MODEL_H

#include "block_model.h"

typedef struct Blockchain {
    Block* head; // ponteiro para o primeiro bloco
    int num_blocks;
} Blockchain;

#endif
