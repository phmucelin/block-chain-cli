#include <stdio.h>
#include <stdlib.h>
#include "transaction_model.h"
#include "coinType_model.h"
#include "user_model.h"
#include "user_actions.c"

Transaction *new_transaction(char* uuidSender, char* uuidReceive, UserCoin* coin) {
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
    return new_transaction;
}

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

Transaction *delete_transaction(Transaction *transactions, int receipt){
  Transaction *current = transactions;
  Transaction *previous = NULL;
  while (current != NULL) {
    if (current->receipt == receipt) {
      if (previous == NULL) {
        transactions = current->prox; // Transação a ser deletada é a primeira
      } else {
        previous->coin->prox = current->prox; // Pular a transação a ser deletada
      }
    }
    free(current); // Liberar a memória da transação deletada
    return transactions; // Retornar a lista atualizada de transações      
  }
}