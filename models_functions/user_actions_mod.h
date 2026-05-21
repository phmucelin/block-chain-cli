#ifndef USER_ACTIONS_MOD_H
#define USER_ACTIONS_MOD_H
#include <stdbool.h>
#include "../models/user_model.h"

char* generate_uuid();

Users *new_user(char *name, char *cpf, char *password, char *dataNasc);
Users* get_user_by_uuid(Users* users, char* uuid);

#endif