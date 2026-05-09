#ifndef EXCHANGE_H
#define EXCHANGE_H

typedef struct Exchange Exchange;

/*
 * Necessario sempre chamar verify_admin(char* uuid), antes de permitir executar essa function.
 */
Exchange* register_exchanges(char* name, int qtdClients); 

#endif