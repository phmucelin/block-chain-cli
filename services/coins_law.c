#include <stdio.h>
#include <stdlib.h>
#include "../models/coinType_model.h"
#include "../models/user_model.h"
#include "../models_functions/coins_law.h"

double getPreco(CoinType type);

static UserCoin *find_coin(UserCoin *coins, CoinType type)
{
    for (UserCoin *current = coins; current != NULL; current = current->prox) {
        if (current->type == type) {
            return current;
        }
    }

    return NULL;
}

static UserCoin *append_coin(UserCoin *coins, CoinType type)
{
    UserCoin *new_coin = (UserCoin *)malloc(sizeof(UserCoin));
    if (new_coin == NULL) {
        return NULL;
    }

    new_coin->type = type;
    new_coin->qtdCoin = 0;
    new_coin->prox = NULL;

    if (coins == NULL) {
        return new_coin;
    }

    UserCoin *tail = coins;
    while (tail->prox != NULL) {
        tail = tail->prox;
    }

    tail->prox = new_coin;
    return new_coin;
}

bool merge_coins(Users *u, CoinType source_type, int qtd_coins, CoinType dest_type, int qtd_coins_destine)
{
    if (u == NULL || u->coins == NULL || qtd_coins <= 0 || qtd_coins_destine <= 0) {
        return false;
    }

    if (source_type == dest_type) {
        return false;
    }

    UserCoin *source_coin = find_coin(u->coins, source_type);
    if (source_coin == NULL || source_coin->qtdCoin < qtd_coins) {
        return false;
    }

    double source_price = getPreco(source_type);
    double dest_price = getPreco(dest_type);
    if (source_price <= 0.0 || dest_price <= 0.0) {
        return false;
    }

    double total_value = (double)qtd_coins * source_price;
    double required_value = (double)qtd_coins_destine * dest_price;
    if (total_value < required_value) {
        return false;
    }

    int qtd_receber = (int)(total_value / dest_price);
    if (qtd_receber < qtd_coins_destine) {
        return false;
    }

    UserCoin *dest_coin = find_coin(u->coins, dest_type);
    if (dest_coin == NULL) {
        dest_coin = append_coin(u->coins, dest_type);
        if (dest_coin == NULL) {
            return false;
        }
    }

    source_coin->qtdCoin -= qtd_coins;
    dest_coin->qtdCoin += qtd_receber;

    return true;
}
