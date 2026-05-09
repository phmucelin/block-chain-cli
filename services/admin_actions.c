#include <stdio.h>
#include <stdlib.h>
#include "../models_functions/admin_actions.h"
#include "../models/admin_model.h"
#include "../models/bank_model.h"
#include "../models/user_model.h"
#include "../services/user_actions.c"
#include "../services/bank_actions.c"

/*
 * Fix: a lista de admins precisa de persistencia em memoria.
 * verify_admin() e get_admin() criavam um Admin via malloc() (memoria nao
 * inicializada) e percorriam seus campos uuid/prox como se fosse uma lista
 * valida — comportamento indefinido garantido (segfault ou loop infinito).
 * Solucao: lista estatica global gerenciada por create_admin().
 */
static Admin* admin_list = NULL;

int verify_admin(char* uuid)
{
    if (!uuid) return 0; // Fix: guard contra uuid NULL

    Admin* current = admin_list; // Fix: percorre a lista real, nao malloc() lixo
    while (current != NULL)
    {
        if (strcmp(current->uuid, uuid) == 0)
        {
            return 1;
        }
        current = current->prox;
    }
    return 0;
}

Admin* create_admin(char* name, char* birthday, char* passKey)
{
    if (!name || !birthday || !passKey) return NULL; // Fix: guard contra parametros NULL

    Admin* newAdmin = (Admin*)malloc(sizeof(Admin));
    if (newAdmin == NULL) {
        return NULL;
    }
    newAdmin->name = name;
    newAdmin->uuid = generate_uuid(); // Fix: gera UUID antes de chamar verify_admin
    newAdmin->prox = NULL;

    if (verify_admin(newAdmin->uuid)) { // Fix: agora verifica contra a lista real
        free(newAdmin->uuid);
        free(newAdmin);
        return NULL; // Admin ja existe (colisao de UUID)
    }

    // Fix: insere na lista global de admins
    newAdmin->prox = admin_list;
    admin_list = newAdmin;

    return newAdmin;
}

Bank* create_bank(char* name, char* id_bank, char* country)
{
    Bank* newBank = (Bank*)malloc(sizeof(Bank));
    if (newBank == NULL || !name || !id_bank || !country) {
        return NULL;
    }

    newBank->name = name;
    newBank->id = id_bank;
    newBank->country = country;

    return newBank;
}

void list_users(Users* u)
{
    if (!u)
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
    if (!uuid) return NULL; // Fix: guard contra uuid NULL

    Admin* current = admin_list; // Fix: percorre a lista real, nao malloc() lixo
    while (current != NULL)
    {
        if (strcmp(current->uuid, uuid) == 0)
        {
            return current;
        }
        current = current->prox;
    }
    return NULL;
}

int delete_user(Users** head, const char* name, const char* uuid, const char* admin_uuid)
{
    if (!head || !*head || !admin_uuid) {
        return 0;
    }

    Admin* admin = get_admin((char*)admin_uuid); // Fix: get_admin agora funciona corretamente
    if (!admin) {
        return 0;
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
