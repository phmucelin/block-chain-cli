#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "../models_functions/coin_price_api.h"
#include "../models/coin_price.h"

double precoMercado[NUM_COINS];

const char* coinNames[] = {
    "bitcoin",   // BTC
    "ethereum",  // ETH
    "tether"     // USDT
};

typedef struct {
    char *data;
    size_t size;
} Memory;

static void cleanup_curl(void) {
    curl_global_cleanup();
}

static void ensure_curl_init(void) {
    static int initialized = 0;
    if (!initialized) {
        curl_global_init(CURL_GLOBAL_ALL);
        atexit(cleanup_curl);
        initialized = 1;
    }
}

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

// Retorna -1.0 em falha (0.0 é ambíguo para crypto)
double extrairPreco(const char *json) {
    if (!json || json[0] == '\0') return -1.0;

    const char *pos = strstr(json, "\"usd\":");
    if (!pos) return -1.0;

    double preco;
    if (sscanf(pos, "\"usd\":%lf", &preco) != 1) return -1.0;
    if (preco <= 0.0) return -1.0;

    return preco;
}

double extrairPrecoBTC(const char *json) {
    return extrairPreco(json);
}

void atualizarPrecosAPI(CoinType tipo) {
    if (tipo < 0 || tipo >= NUM_COINS) {
        fprintf(stderr, "Erro: tipo de moeda inválido (%d)\n", tipo);
        return;
    }

    ensure_curl_init();

    Memory chunk;
    chunk.data = malloc(1);
    if (!chunk.data) {
        fprintf(stderr, "Erro: falha ao alocar memória para resposta\n");
        return;
    }
    chunk.size = 0;

    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Erro: falha ao inicializar curl\n");
        free(chunk.data);
        return;
    }

    char url[256];
    snprintf(url, sizeof(url),
        "https://api.coingecko.com/api/v3/simple/price?ids=%s&vs_currencies=usd",
        coinNames[tipo]);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Erro curl para %s: %s\n", coinNames[tipo], curl_easy_strerror(res));
        goto cleanup;
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200) {
        fprintf(stderr, "Erro HTTP %ld para %s\n", http_code, coinNames[tipo]);
        goto cleanup;
    }

    double preco = extrairPreco(chunk.data);
    if (preco < 0.0) {
        fprintf(stderr, "Erro: resposta inválida para %s: %s\n", coinNames[tipo], chunk.data);
        goto cleanup;
    }

    precoMercado[tipo] = preco;
    printf("Preço de %s atualizado: %.2f USD\n", coinNames[tipo], preco);

cleanup:
    curl_easy_cleanup(curl);
    free(chunk.data);
}

double getPreco(CoinType tipo) {
    if (tipo < 0 || tipo >= NUM_COINS) return -1.0;
    return precoMercado[tipo];
}
