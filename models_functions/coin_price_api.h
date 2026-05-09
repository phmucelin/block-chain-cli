#ifndef COIN_PRICE_API_H
#define COIN_PRICE_API_H

#include <stddef.h>
#include "../models/coinType_model.h"

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);

double extrairPrecoBTC(const char *json);

void atualizarPrecosAPI(CoinType tipo);

double getPreco(CoinType tipo);

#endif