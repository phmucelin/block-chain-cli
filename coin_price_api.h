#ifndef COIN_PRICE_API_H
#define COIN_PRICE_API_H

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);

double extrairPrecoBTC(const char *json);

void atualizarPrecosAPI();

double getPreco(CoinType tipo);

#endif