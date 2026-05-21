#include <stdlib.h>
#include "../models/bank_model.h"
#include "../models/user_model.h"
#include "../models/transaction_model.h"

int is_user_from_bank(Bank* bank, Users* user)
{
	if (!bank || !user) {
		return 0;
	}
	return user->bank == bank;
}

int approve_user_account(Bank* bank, Users* user)
{
	if (!bank || !user) {
		return 0;
	}
	if (user->bank != NULL) {
		return 0; // Usuario ja possui banco associado
	}
	user->bank = bank;
	bank->qtdClients += 1;
	return 1;
}

int reject_user_account(Bank* bank, Users* user)
{
	if (!bank || !user) {
		return 0;
	}
	if (user->bank != bank) {
		return 0; // Usuario nao pertence a este banco
	}
	user->bank = NULL;
	if (bank->qtdClients > 0) {
		bank->qtdClients -= 1;
	}
	return 1;
}

int add_balance(Users* u, double amount) {
    if (!u || amount <= 0) return 0;
    u->balance += amount;
    return 1;
}
