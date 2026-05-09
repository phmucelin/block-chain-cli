#ifndef ADMIN_MODEL_H
#define ADMIN_MODEL_H

#include "user_model.h"

typedef struct Admin {
    char* uuid;
    char* name;
    Admin* prox;
    // Adicione outros campos de admin se necessário
} Admin;

#endif
