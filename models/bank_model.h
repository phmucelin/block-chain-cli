#ifndef BANK_MODEL_H
#define BANK_MODEL_H

//Usuarios sao ligados ao banco, nao tem necessidade de ter uma lista de usuarios nessa struct.

typedef struct
{
  char* name;
  char* id;
  char* country;
  int qtdClients;
}Bank;

#endif