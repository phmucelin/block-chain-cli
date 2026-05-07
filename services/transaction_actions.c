#include <stdio.h>
#include <stdlib.h>
#include "../models/transaction_model.h"
#include "../models/coinType_model.h"
#include "../models/user_model.h"
#include "../services/user_actions.c"

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