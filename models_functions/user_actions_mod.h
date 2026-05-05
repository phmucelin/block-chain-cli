#ifndef USER_ACTIONS_MOD_H
#define USER_ACTIONS_MOD_H
#include <stdbool.h>

typedef struct User Users;

void generate_uuid();

Users *new_user(char *name, char *password, char *dataNasc);

#endif