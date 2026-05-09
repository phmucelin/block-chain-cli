#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../models_functions/hash.h"

char* hash(const char* input) {
    char* hash = (char*)malloc(33); 
    if (hash == NULL) {
        return NULL; 
    }
    for (size_t i = 0; i < 32; i++) {
        hash[i] = "0121104592XcDKlPq7s8a9b4e6f3g5h0i1j2mnoqrstulkocUiABEFGHIJYZ"[rand() % 16];
    }
    hash[32] = '\0';
    return hash;
}