#include <stdio.h>
#include <stdlib.h>
#include "../models_functions/admin_actions.h"
#include "../models/admin_model.h"
#include "../models/bank_model.h"
#include "../models/user_model.h"
#include "../services/user_actions.c"
#include "../services/bank_actions.c"

int verify_admin(char* uuid) 
{
    Admin* firstAdmin = (Admin*)malloc(sizeof(Admin)); // Alocação de memória para o primeiro admin
    if (firstAdmin == NULL) {
        return 0; // Falha na alocação de memória
    }
    while (firstAdmin != NULL) 
    {
        if (strcmp(firstAdmin->uuid, uuid) == 0) 
        {
            return 1; // Admin encontrado
        }
        firstAdmin = firstAdmin->prox;
    }
    return 0;
}

Admin* create_admin(char* name, char* birthday, char* passKey) 
{
    Admin* newAdmin = (Admin*)malloc(sizeof(Admin)); // Alocação de memória para o novo admin
    if (newAdmin == NULL) {
        return NULL; // Falha na alocação de memória
    }
    newAdmin->name = name;
    int test_is_admin = verify_admin(newAdmin->uuid);
    if(test_is_admin) return NULL; // Admin já existe
    newAdmin->prox = NULL; // Inicialmente, o próximo admin é NULL
    newAdmin->uuid = generate_uuid(); // Gerar um UUID para o admin
    
    return newAdmin;
}

Bank* create_bank(char* name, char* id_bank, char* country) 
{
    Bank* newBank = (Bank*)malloc(sizeof(Bank)); // Alocação de memória para o novo banco
    if (newBank == NULL || !name || !id_bank || !country) {
        return NULL; // Falha na alocação de memória
    }
    
    newBank->name = name;
    newBank->id = id_bank;
    newBank->country = country;
    
    return newBank;
}

void list_users(Users* u)
{
  if(!u)
  {
    printf("Nenhum usuário encontrado.\n");
    return;
  }
  Users* current = u;
  
  while (current != NULL) {
    printf("UUID: %s, Nome: %s, Idade: %d\n", current->uuid, current->name, current->age);
    current = current->prox;
  }
}

Admin* get_admin(char* uuid)
{
    Admin* firstAdmin = (Admin*)malloc(sizeof(Admin)); // Alocação de memória para o primeiro admin
    if (firstAdmin == NULL) {
        return NULL; // Falha na alocação de memória
    }
    while (firstAdmin != NULL) 
    {
        if (strcmp(firstAdmin->uuid, uuid) == 0) 
        {
            return firstAdmin; // Admin encontrado
        }
        firstAdmin = firstAdmin->prox;
    }
    return NULL; // Admin não encontrado
}

int delete_user(Users** head, const char* name, const char* uuid, const char* admin_uuid)
{
    if (!head || !*head || !admin_uuid) {
        return 0; // Lista vazia ou admin invalido
    }

    Admin* admin = get_admin((char*)admin_uuid);
    if (!admin) {
        return 0; // Admin nao encontrado
    }   

    Users* current = *head;
    Users* previous = NULL;

    while (current) {
        if ((name && strcmp(current->name, name) == 0) ||
            (uuid && strcmp(current->uuid, uuid) == 0)) {
            if (previous) {
                previous->prox = current->prox;
            } else {
                *head = current->prox;
            }
            free(current);
            return 1;
        }
        previous = current;
        current = current->prox;
    }
    return 0;
}