#ifndef BANK_ACTIONS_H
#define BANK_ACTIONS_H

typedef struct Bank Bank;
typedef struct Users Users;

int is_user_from_bank(Bank* bank, Users* user);
int approve_user_account(Bank* bank, Users* user);
int reject_user_account(Bank* bank, Users* user);
int add_balance(Users* u, double amount);

#endif
