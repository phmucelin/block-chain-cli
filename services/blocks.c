#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../models/transaction_model.h"
#include "../models/coinType_model.h"
#include "../models/user_model.h"
#include "../models/block_model.h"
#include "../models_functions/blocks.h"
#include "../services/user_actions.c"
#include "../services/hash.c"
#include "../services/transaction_actions.c" 
#include "../services/coins_law.c" // verificar se a moeda de fato existe. 

Block* create_block(char UUID[37], time_t time, char prev_hash[65], char hash[65], Transaction* transactions) {
    Block* new_block = (Block*)malloc(sizeof(Block));
    if (new_block == NULL)
    {
        return NULL; // Falha na alocação de memória
    }
    new_block->index = 0; // Defina o índice conforme necessário
    new_block->created_at = time;
    new_block->updated_at = time; // Inicialmente, updated_at é igual a created_at
    new_block->expire_at = time + 3600; // Exemplo: o bloco expira em 1 hora
    new_block->transactions = transactions;
    new_block->num_transactions = 0; // Defina o número de transações conforme necessário
    strncpy(new_block->previous_hash, prev_hash, 65);
    strncpy(new_block->hash, hash, 65);
    new_block->prox = NULL;
    
    return new_block;
}

int fill_block(Transaction* transaction, Block* block) {
    if (transaction == NULL || block == NULL) {
        return 0; // Parâmetros inválidos
    }

    // Adicionar a transação ao bloco
    if (block->transactions == NULL) {
        block->transactions = transaction;
    } else {
        Transaction* current = block->transactions;
        while (current->prox != NULL) {
            current = current->prox;
        }
        current->prox = transaction;
    }
    block->num_transactions++;

    // Atualizar o timestamp de atualização do bloco
    block->updated_at = time(NULL);

    return 1; // Transação adicionada com sucesso
}