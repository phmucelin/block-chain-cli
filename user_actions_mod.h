#ifndef USER_ACTIONS_MOD.H
#define USER_ACTIONS_MOD.H
#include <stdbool.h>

typedef struct User User;
typedef struct Bank Bank;

void generate_uuid();

User* new_user(char* name, char* password, char* dataNasc);

bool check_amount(User* u, double amount_transfer);

bool recharge_money(User* u, double amount_recharge, Bank* b);

bool withdraw_money(User* u, double amount_withdraw, Bank* b);

#endif