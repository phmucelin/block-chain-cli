#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../models/bank_model.h"
#include "../models/transaction_model.h"
#include "../models/coinType_model.h"
#include "../models/user_model.h"

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
    strcpy(uuid_str, uuid);
    return uuid_str;
}

char* generate_hash(const char* password) {
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
    if(!name || !password || !dataNasc) {
        free(new_user);
        return NULL;
    }
    new_user->name = name;
    new_user->hashPass = generate_hash(password);
    new_user->age = atoi(dataNasc);
    new_user->bank = NULL;
    new_user->transaction_id = NULL;
    new_user->coins = NULL;
    new_user->uuid = generate_uuid();
    new_user->prox = NULL;
    if(!new_user->hashPass || !new_user->uuid || !new_user->age || new_user->age <= 0) {
        free(new_user->hashPass);
        free(new_user->uuid);
        free(new_user);
        return NULL;
    }
    return new_user;
}

Users* get_user_by_uuid(Users* users, char* uuid) {
    Users* current = users;
    while (current != NULL) {
        if (strcmp(current->uuid, uuid) == 0) {
            return current;
        }
        current = current->prox;
    }
    return NULL;
}

static unsigned int hash_uuid(const char* uuid) {
    unsigned int hash = 0;
    for (size_t i = 0; i < strlen(uuid); i++) {
        hash = hash * 31 + uuid[i];
    }
    return hash % 100; // Retorna um índice entre 0 e 99
}

static int inserir_user_hashTable(Users* user) {
    int endereco = hash_uuid(user->uuid);
    for (int i = 0; i < 100; i++)
    {
        if (users_pos[endereco].ocupado == 0) {
            users_pos[endereco].user = user;
            users_pos[endereco].ocupado = 1;
            return 1; // Sucesso
        }
    }
    return 0; // Falha, tabela cheia
}

static Users* buscar_user_hashTable(char* uuid) {
    int endereco = hash_uuid(uuid);
    for (int i = 0; i < 100; i++)
    {
        if (users_pos[endereco].ocupado == 1 && strcmp(users_pos[endereco].user->uuid, uuid) == 0) {
            return users_pos[endereco].user; // Usuário encontrado
        }
    }
    return NULL; // Usuário não encontrado
}