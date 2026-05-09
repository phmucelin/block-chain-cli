#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "../models/transaction_model.h"
#include "../models/coinType_model.h"
#include "../models/user_model.h"
#include "../models/block_model.h"
#include "../services/user_actions.c"
#include "../services/hashpass.c"

static Transaction *dequeue_transaction(Queue *queue)
{
    if (queue == NULL || queue->first == NULL)
    {
        return NULL;
    }

    Transaction *tx = queue->first;
    queue->first = tx->prox;
    if (queue->first == NULL)
    {
        queue->last = NULL;
    }
    tx->prox = NULL;
    return tx;
}

static bool check_transaction(Users* users, Transaction* transaction)
{
    if (users == NULL || transaction == NULL || transaction->coin == NULL)
    {
        return false;
    }
    
    Users* get_sender = get_user_by_uuid(users, transaction->uuidSender);
    Users* get_receiver = get_user_by_uuid(users, transaction->uuidReceive);
    if(!get_sender || !get_receiver) return false; // Usuario remetente ou destinatário nao encontrado
    
    if (transaction->coin->qtdCoin <= 0)
    {
        return false; // Tipo de moeda não corresponde ou quantidade inválida
    }

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

int can_accept_transaction(Block *block, time_t now)
{
    if (block == NULL)
    {
        return 0;
    }

    return now < block->expire_at;
}

int send_to_block(Users* users, Queue* queue, Block* block, time_t now) {
    if (users == NULL || queue == NULL || block == NULL)
    {
        return 0;
    }

    if (!can_accept_transaction(block, now))
    {
        return 0;
    }

    if (queue->first == NULL)
    {
        return 0;
    }

    Transaction *tx = queue->first;
    if (!check_transaction(users, tx))
    {
        return 0;
    }

    tx = dequeue_transaction(queue);
    if (tx == NULL)
    {
        return 0;
    }

    if (!fill_block(tx, block))
    {
        tx->prox = queue->first;
        queue->first = tx;
        if (queue->last == NULL)
        {
            queue->last = tx;
        }
        return 0;
    }

    return 1;
}