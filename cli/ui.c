#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "ui.h"
#include "../models/user_model.h"
#include "../models/coinType_model.h"

void ui_clear(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void ui_print_banner(void) {
    printf(BOLD CYAN);
    printf("  ██████╗ ██╗      ██████╗  ██████╗██╗  ██╗ ██████╗██╗  ██╗ █████╗ ██╗███╗   ██╗\n");
    printf("  ██╔══██╗██║     ██╔═══██╗██╔════╝██║ ██╔╝██╔════╝██║  ██║██╔══██╗██║████╗  ██║\n");
    printf("  ██████╔╝██║     ██║   ██║██║     █████╔╝ ██║     ███████║███████║██║██╔██╗ ██║\n");
    printf("  ██╔══██╗██║     ██║   ██║██║     ██╔═██╗ ██║     ██╔══██║██╔══██║██║██║╚████║\n");
    printf("  ██████╔╝███████╗╚██████╔╝╚██████╗██║  ██╗╚██████╗██║  ██║██║  ██║██║██║ ╚███║\n");
    printf("  ╚═════╝ ╚══════╝ ╚═════╝  ╚═════╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚══╝\n");
    printf(RESET);
    printf(DIM "           Sua plataforma de blockchain segura e descentralizada\n" RESET);
    printf("\n");
}

void ui_print_success(const char* msg) {
    printf(BOLD GREEN "  [OK] %s" RESET "\n", msg);
}

void ui_print_error(const char* msg) {
    printf(BOLD RED "  [ERRO] %s" RESET "\n", msg);
}

void ui_print_separator(void) {
    printf(DIM);
    for (int i = 0; i < 60; i++) printf("\xe2\x94\x80"); /* UTF-8 for ─ */
    printf(RESET "\n");
}

void ui_spinner(const char* msg, int ms) {
    const char* frames[] = {"|", "/", "-", "\\"};
    int total_frames = ms / 80;
    if (total_frames < 1) total_frames = 1;
    for (int i = 0; i < total_frames; i++) {
        printf("\r  %s [%s]   ", msg, frames[i % 4]);
        fflush(stdout);
        usleep(80000);
    }
    printf("\r%*s\r", 60, ""); /* clear line */
    fflush(stdout);
}

void ui_copy_uuid(const char* uuid) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "printf '%%s' '%s' | pbcopy 2>/dev/null", uuid);
    if (system(cmd) == 0) {
        printf(GREEN "  [OK] UUID copiado para a área de transferência!\n" RESET);
    } else {
        printf(YELLOW "  UUID: %s\n" RESET, uuid);
        printf(DIM "  (copie manualmente acima)\n" RESET);
    }
}

void ui_print_profile(Users* u) {
    if (!u) return;

    printf(CYAN "  ┌──────────────────────────────────────────────────────────────┐\n" RESET);
    printf(CYAN "  │" RESET BOLD "  Perfil do Usuário" RESET CYAN "                                            │\n" RESET);
    printf(CYAN "  ├──────────────────────────────────────────────────────────────┤\n" RESET);
    printf(CYAN "  │" RESET "  Nome    : %-52s" CYAN "│\n" RESET, u->name);
    printf(CYAN "  │" RESET "  CPF     : %-52s" CYAN "│\n" RESET, u->cpf ? u->cpf : "—");
    printf(CYAN "  │" RESET "  UUID    : " BOLD "%-52s" RESET CYAN "│\n" RESET, u->uuid);
    printf(CYAN "  │" RESET "  Saldo   : $%-51.2f" CYAN "│\n" RESET, u->balance);
    printf(CYAN "  ├──────────────────────────────────────────────────────────────┤\n" RESET);
    printf(CYAN "  │" RESET BOLD "  Moedas" RESET CYAN "                                                       │\n" RESET);

    const char* coin_names[] = {"BTC", "ETH", "USDT"};
    for (int t = 0; t < NUM_COINS; t++) {
        double qty = 0.0;
        UserCoin* c = u->coins;
        while (c) {
            if ((int)c->type == t) { qty = c->qtdCoin; break; }
            c = c->prox;
        }
        printf(CYAN "  │" RESET "  %-6s : %-46.8f" CYAN "│\n" RESET, coin_names[t], qty);
    }

    printf(CYAN "  └──────────────────────────────────────────────────────────────┘\n" RESET);
}
