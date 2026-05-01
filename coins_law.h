#ifndef COINS_LAW_H
#define COINS_LAW_H

typedef struct User User;
typedef struct CoinsType CoinsType;

bool merge_coins(User* u, CoinsType* type1, int qtd_coins, CoinsType* type2, int qtd_coins_destine);

#endif