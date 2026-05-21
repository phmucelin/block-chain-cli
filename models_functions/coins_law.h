#ifndef COINS_LAW_H
#define COINS_LAW_H

#include <stdbool.h>

#include "../models/user_model.h"

bool merge_coins(Users* u, CoinType source_type, double qtd_coins, CoinType dest_type, double qtd_coins_destine);
int buy_coins(Users* u, CoinType type, double qtd);

#endif