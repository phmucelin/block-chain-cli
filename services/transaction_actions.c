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
bool check_transaction(Users* u, Transaction* transaction, Queue* queue, CoinType coin)
{
    if (u == NULL || transaction == NULL || queue == NULL) 
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
    bool test = get_transaction_by_receipt(queue, transaction->receipt) != NULL;
    if (!test) return false; // Transacao nao existe.

    return true;
}

Transaction *get_transaction_by_receipt(Queue *queue, int receipt) {
    if (queue == NULL) {
        return NULL;
    }
    Transaction *current = queue->first;
    while (current != NULL) {
        if (current->receipt == receipt) {
            return current;
        }
        current = current->prox;
    }
    return NULL; // Transação não encontrada
}

Transaction *new_transaction(char* uuidSender, char* uuidReceive, UserCoin* coin, Queue* queue) {
    if (uuidSender == NULL || uuidReceive == NULL || coin == NULL || queue == NULL) {
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
    
    if (queue->first == NULL) {
        queue->first = new_transaction;
        queue->last = new_transaction;
    } else {
        queue->last->prox = new_transaction;
        queue->last = new_transaction;
    }
    
    return new_transaction;
}

int delete_transaction(Queue *queue, int receipt){
  if (queue == NULL) {
    return 0;
  }
  
  Transaction *current = queue->first;
  Transaction *previous = NULL;

  while (current != NULL) {
    if (current->receipt == receipt) {
      if (previous == NULL) {
        queue->first = current->prox;
      } else {
        previous->prox = current->prox;
      }
      
      if (current == queue->last) {
        queue->last = previous;
      }

      free(current);
      return 1;
    }

    previous = current;
    current = current->prox;
  }

  return 0;
}

int send_to_block(Users* u, Transaction* transaction, Queue* queue, Block* block) {

    if (transaction == NULL || transaction->uuidSender == NULL || transaction->uuidReceive == NULL || transaction->coin == NULL || block == NULL || queue == NULL)
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

    if (!check_transaction(sender, transaction, queue, transaction->coin->type))
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