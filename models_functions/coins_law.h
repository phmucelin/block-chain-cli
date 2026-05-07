#ifndef COINS_LAW_H
#define COINS_LAW_H

#include <stdbool.h>

#include "../models/user_model.h"

bool merge_coins(Users* u, CoinType source_type, int qtd_coins, CoinType dest_type, int qtd_coins_destine);

#endif