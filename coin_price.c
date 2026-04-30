#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "coin_price_api.h"

typedef enum {
    BTC,
    ETH,
    USDT,
    NUM_COINS
} CoinType;

double precoMercado[NUM_COINS];

// estrutura para guardar resposta da API
typedef struct {
    char *data;
    size_t size;
} Memory;

// callback do curl (guarda resposta em memória)
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    Memory *mem = (Memory *)userp;

    char *ptr = realloc(mem->data, mem->size + totalSize + 1);
    if (!ptr) return 0;

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, totalSize);
    mem->size += totalSize;
    mem->data[mem->size] = '\0';

    return totalSize;
}

// parsing MUITO simples (não robusto, mas funciona pro exemplo)
double extrairPrecoBTC(const char *json) {
    char *pos = strstr(json, "\"usd\":");
    if (!pos) return 0.0;

    double preco;
    sscanf(pos, "\"usd\":%lf", &preco);
    return preco;
}

void atualizarPrecosAPI() {
    CURL *curl;
    CURLcode res;

    Memory chunk;
    chunk.data = malloc(1);
    chunk.size = 0;

    curl = curl_easy_init();
    if (!curl) return;

    curl_easy_setopt(curl, CURLOPT_URL,
        "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        double preco = extrairPrecoBTC(chunk.data);
        precoMercado[BTC] = preco;
    } else {
        printf("Erro na requisição\n");
    }

    curl_easy_cleanup(curl);
    free(chunk.data);
}

double getPreco(CoinType tipo) {
    return precoMercado[tipo];
}

