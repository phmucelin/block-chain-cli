#ifndef EXCHANGE_MODEL_H
#define EXCHANGE_MODEL_H

#include "user_model.h"

typedef struct Exchange
{
    char* name;
    int qtdClients;
    char* id;
    char* addres;
    char* country;
    UserCoin* coin; // Para pegarmos qual tipo de Moeda trabalha, quantas tem.
}Exchange;

#endif