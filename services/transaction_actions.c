#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../models/transaction_model.h"
#include "../models/coinType_model.h"
#include "../models/user_model.h"
#include "../models/block_model.h"
#include "../services/user_actions.c"
#include "../services/hashpass.c"

/* forward declarations para funcoes definidas em outros modulos */
bool check_transaction(Users* u, Transaction* transaction, CoinType coin)
{
    if (u == NULL || transaction == NULL) 
    {
        return false;
    }
    
    Users* get_sender = get_user_by_uuid(u, u->uuid);
    Users* get_receiver = get_user_by_uuid(u, transaction->uuidReceive);
    if(!get_sender || !get_receiver) return false; // Usuario remetente ou destinatário nao encontrado
    
    if (transaction->coin->type != coin || transaction->coin->qtdCoin <= 0)
    {
        return false; // Tipo de moeda não corresponde ou quantidade inválida
    }
    bool test = get_transaction_by_receipt(transaction, transaction->receipt) != NULL;
    if (!test) return false; // Transacao nao existe.

    return true;
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
        return 0;
    }

    /*
     * Fix: check_transaction recebia transaction->uuidSender (char*) onde
     * esperava Users* como primeiro argumento — tipo completamente errado.
     * Correto: buscar o usuario remetente pelo UUID antes de chamar a funcao.
     */
    Users* sender = get_user_by_uuid(u, transaction->uuidSender);
    Users* receiver = get_user_by_uuid(u, transaction->uuidReceive);
    if (!sender || !receiver)
    {
        return 0;
    }

    if (!check_transaction(sender, transaction, transaction->coin->type))
    {
        return 0;
    }

    int sending_transaction = fill_block(transaction, block);

    if (!sending_transaction)
    {
        return 0;
    }
    return 1;
}