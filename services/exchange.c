#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../models/exchange_model.h"
#include "../models_functions/exchange.h"


Exchange* register_exchanges(char* name, int qtdClients, char* addres, char* country)
{
    Exchange* new_exchange = (Exchange*)malloc(sizeof(Exchange));
    if (new_exchange == NULL)
    {
        fprintf(stderr, "Memory allocation failed for new exchange.\n");
        return NULL;
    }

    new_exchange->name = strdup(name);
    new_exchange->qtdClients = qtdClients;
    new_exchange->id = generate_uuid();
    new_exchange->addres = strdup(addres);
    new_exchange->country = strdup(country);
    new_exchange->coin = NULL; // vai ser definido depois, nao tem motivo para ser definido agora

    return new_exchange;
}