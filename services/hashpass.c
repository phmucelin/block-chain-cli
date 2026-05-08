#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* hash_password_and_block(const char* p) {
    if (p == NULL) {
        return NULL; // Verificar se a senha é nula
    }

    // Alocar memória para o hash (64 caracteres + null terminator)
    char* hash = (char*)malloc(65 * sizeof(char));
    if (hash == NULL) {
        return NULL; // Falha na alocação de memória
    }

    // Gerar um hash simples usando uma função de hash fictícia (substitua por uma real, como SHA-256)
    for (size_t i = 0; i < 64 && p[i] != '\0'; i++) {
        hash[i] = (p[i] + 1) % 256; // Exemplo de transformação simples
    }
    hash[64] = '\0'; // Null terminator

    return hash;
}