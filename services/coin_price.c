#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "../models_functions/coin_price_api.h"
#include "../models/coin_price.h"

double precoMercado[NUM_COINS];

// Mapa de CoinType para nome da moeda (conforme API CoinGecko)
const char* coinNames[] = {
    "bitcoin",   // BTC
    "ethereum",  // ETH
    "tether"     // USDT
};

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

// parsing simples de preço do JSON (procura por "usd":valor)
double extrairPreco(const char *json) {
    char *pos = strstr(json, "\"usd\":");
    if (!pos) return 0.0;

    double preco;
    sscanf(pos, "\"usd\":%lf", &preco);
    return preco;
}

// compatibilidade com API antiga
double extrairPrecoBTC(const char *json) {
    return extrairPreco(json);
}

void atualizarPrecosAPI(CoinType tipo) {
    // Validar tipo de moeda
    if (tipo < 0 || tipo >= NUM_COINS) {
        printf("Erro: Tipo de moeda inválido\n");
        return;
    }

    CURL *curl;
    CURLcode res;

    Memory chunk;
    chunk.data = malloc(1);
    chunk.size = 0;

    curl = curl_easy_init();
    if (!curl) return;

    // Construir URL dinamicamente baseado no tipo
    char url[256];
    snprintf(url, sizeof(url),
        "https://api.coingecko.com/api/v3/simple/price?ids=%s&vs_currencies=usd",
        coinNames[tipo]);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        double preco = extrairPreco(chunk.data);
        precoMercado[tipo] = preco;
        printf("Preço de %s atualizado: %.2f USD\n", coinNames[tipo], preco);
    } else {
        printf("Erro na requisição para %s\n", coinNames[tipo]);
    }

    curl_easy_cleanup(curl);
    free(chunk.data);
}

double getPreco(CoinType tipo) {
    return precoMercado[tipo];
}

