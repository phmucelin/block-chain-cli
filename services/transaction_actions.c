#include <stdio.h>
#include <stdlib.h>
#include "../models/transaction_model.h"
#include "../models/coinType_model.h"
#include "../models/user_model.h"
#include "../services/user_actions.c"
#include "../models/block_model.h"
#include "../models_functions/transfer_actions.h"
#include "../services/user_actions.c"
#include "../services/hashpass.c"

Transaction *get_transaction_by_receipt(Transaction *transactions, int receipt) {
    Transaction *current = transactions;
    while (current != NULL) {
        if (current->receipt == receipt) {
            return current;
        }
        current = current->prox; // Supondo que as transações estão encadeadas
    }
    return NULL; // Transação não encontrada
}

Transaction *new_transaction(char* uuidSender, char* uuidReceive, UserCoin* coin) {
    if (uuidSender == NULL || uuidReceive == NULL || coin == NULL) {
        return NULL;
    }

    Transaction *new_transaction = (Transaction *)malloc(sizeof(Transaction));
    if (new_transaction == NULL) {
        return NULL; // Falha na alocação de memória
    }
    new_transaction->uuidSender = uuidSender;
    new_transaction->uuidReceive = uuidReceive;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    new_transaction->datetime = tm;
    new_transaction->coin = coin;
    new_transaction->receipt = rand() % 1000002; // Gerar um ID de recibo aleatório
    new_transaction->prox = NULL;
    
    return new_transaction;
}

Transaction *delete_transaction(Transaction *transactions, int receipt){
  Transaction *current = transactions;
  Transaction *previous = NULL;

  while (current != NULL) {
    if (current->receipt == receipt) {
      if (previous == NULL) {
        transactions = current->prox;
      } else {
        previous->prox = current->prox;
      }

      free(current);
      return transactions;
    }

    previous = current;
    current = current->prox;
  }

  return transactions;
}

int send_to_block(Users* u, Transaction* transaction, Block* block) {
    
    if (transaction == NULL || transaction->uuidSender == NULL || transaction->uuidReceive == NULL || transaction->coin == NULL || block == NULL) 
    {
        return 0; // Parâmetros inválidos
    }

    // Verificar se o usuário tem saldo suficiente para a transferência
    if (!check_transaction(transaction->uuidSender, transaction->uuidReceive, transaction->coin->qtdCoin, &transaction->coin->type)) 
    {
        return 0; // Saldo insuficiente ou transação inválida
    }
    
    // Aqui iremos chamar a funcao de criar um bloco direto do model de Blocks.h, passando a transação como parâmetro. 
    Users* sender = get_user_by_uuid(u, transaction->uuidSender);
    Users* receiver = get_user_by_uuid(u, transaction->uuidReceive);
    if(!sender || !receiver) 
    {
        return 0; // Usuário não encontrado
    }
    
    int sending_transaction = fill_block(transaction, block);
    
    if(!sending_transaction) 
    {
        return 0; // Falha ao enviar a transação para o bloco
    }
    return 1; // Transação enviada com sucesso
}