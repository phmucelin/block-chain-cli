#include <stdio.h>
#include <stdlib.h>
#include "bank_model.h"
#include "transaction_model.h"
#include "coinType_model.h"
#include "user_model.h"

char* generate_uuid(){
    const char *hex = "0121104592XcDKlPq7s8a9b4e6f3g5h0i1j2mnoqrstuuvwxzABEFGHIJYZ";
    char uuid[37];

    srand(time(NULL));

    for(int i = 0; i < 36; i++){
        if(i == 8 || i == 13 || i == 18 || i == 23){
            uuid[i] = '-';
        }else if (i==14){
            uuid[i] = '4';
        } else if(i == 19) {
            uuid[i] = hex[(rand() % 4) + 8];
        } else {
            uuid[i] = hex[rand() % 16];
        }
    }
    uuid[36] = '\0';
    char *uuid_str = (char *)malloc(37 * sizeof(char));
    if (uuid_str == NULL) {
        return NULL; // Falha na alocação de memória
    }
    return uuid_str;
}

char* generate_hash(const char* password) {
    size_t len = strlen(password);
    char* hash = (char*)malloc(33); // 32 caracteres + null terminator
    if (hash == NULL) {
        return NULL; // Falha na alocação de memória
    }
    for (size_t i = 0; i < 32; i++) {
        hash[i] = "0121104592XcDKlPq7s8a9b4e6f3g5h0i1j2mnoqrstulkocUiABEFGHIJYZ"[rand() % 16];
    }
    hash[32] = '\0';
    return hash;
}

Users *new_user(char *name, char *password, char *dataNasc)
{
    Users *new_user = (Users *)malloc(sizeof(Users));
    if (new_user == NULL)
    {
        return NULL; // Falha na alocação de memória
    }
    new_user->name = name;
    new_user->hashPass = generate_hash(password);
    new_user->age = atoi(dataNasc);
    new_user->bank = NULL;
    new_user->transaction_id = NULL;
    new_user->coins = NULL;
    new_user->uuid = generate_uuid();
    return new_user;
}
