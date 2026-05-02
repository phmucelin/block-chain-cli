#ifndef USER_ACTIONS_MOD_H
#define USER_ACTIONS_MOD_H
#include <stdbool.h>

typedef struct User User;


void generate_uuid();

User* new_user(char* name, char* password, char* dataNasc);

#endif