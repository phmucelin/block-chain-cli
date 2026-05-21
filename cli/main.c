#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

#include "ui.h"
#include "../models/user_model.h"
#include "../models/bank_model.h"
#include "../models/block_model.h"
#include "../models/transaction_model.h"
#include "../models/coinType_model.h"
#include "../models_functions/user_actions_mod.h"
#include "../models_functions/bank_actions.h"
#include "../models_functions/admin_actions.h"
#include "../models_functions/blocks.h"
#include "../models_functions/coin_price_api.h"
#include "../models_functions/coins_law.h"
#include "../models_functions/hash.h"
#include "../db/db.h"

/* forward declarations of transaction functions */
Transaction* new_transaction(char* uuidSender, char* uuidReceive, UserCoin* coin, Queue* queue);
int send_to_block(Users* users, Queue* queue, Block* block, time_t now);

/* ── global state ──────────────────────────────────────────────────────────── */
static Users* g_users      = NULL;
static Users* g_current    = NULL;
static Block* g_block      = NULL;
static Queue  g_queue      = {NULL, NULL};
static Bank*  g_banks[10];
static int    g_bank_count = 0;
static int    g_is_admin   = 0;

/* ── helpers ───────────────────────────────────────────────────────────────── */
static void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void read_line(char* buf, int size) {
    if (fgets(buf, size, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
    }
}

static void read_password(char* buf, int size) {
    struct termios old, new_t;
    tcgetattr(STDIN_FILENO, &old);
    new_t = old;
    new_t.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_t);
    if (fgets(buf, size, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    printf("\n");
}

static Users* find_user_by_cpf(const char* cpf) {
    Users* cur = g_users;
    while (cur) {
        if (cur->cpf && strcmp(cur->cpf, cpf) == 0) return cur;
        cur = cur->prox;
    }
    return NULL;
}

static Users* find_user_by_cpf_or_uuid(const char* input) {
    Users* cur = g_users;
    while (cur) {
        if ((cur->cpf  && strcmp(cur->cpf,  input) == 0) ||
            (cur->uuid && strcmp(cur->uuid, input) == 0))
            return cur;
        cur = cur->prox;
    }
    return NULL;
}

static const char* coin_name(CoinType t) {
    switch (t) {
        case BTC:  return "BTC";
        case ETH:  return "ETH";
        case USDT: return "USDT";
        default:   return "???";
    }
}

static double user_coin_qty(Users* u, CoinType t) {
    UserCoin* c = u->coins;
    while (c) {
        if (c->type == t) return c->qtdCoin;
        c = c->prox;
    }
    return 0.0;
}

/* ── user menu actions ─────────────────────────────────────────────────────── */

static void action_transfer(void) {
    ui_clear();
    ui_print_separator();
    printf(BOLD "  Transferência\n" RESET);
    ui_print_separator();

    printf("\n");
    ui_print_profile(g_current);
    printf("\n");

    char recv_input[64];
    printf("  CPF ou UUID do destinatário: ");
    read_line(recv_input, sizeof(recv_input));

    Users* receiver = find_user_by_cpf_or_uuid(recv_input);
    if (!receiver) {
        ui_print_error("Destinatário não encontrado (CPF ou UUID inválido).");
        return;
    }
    if (receiver == g_current) {
        ui_print_error("Não pode transferir para si mesmo.");
        return;
    }

    printf("\n");
    ui_print_profile(receiver);
    printf("\n");

    printf("  Tipo de moeda  [1] BTC  [2] ETH  [3] USDT: ");
    char choice_buf[8];
    read_line(choice_buf, sizeof(choice_buf));
    int choice = atoi(choice_buf);
    if (choice < 1 || choice > 3) { ui_print_error("Opção inválida."); return; }
    CoinType ctype = (CoinType)(choice - 1);

    double available = user_coin_qty(g_current, ctype);
    printf("  Disponível: %.8f %s\n", available, coin_name(ctype));
    printf("  Quantidade de %s: ", coin_name(ctype));
    char qty_buf[32];
    read_line(qty_buf, sizeof(qty_buf));
    double qty = atof(qty_buf);
    if (qty <= 0.0) { ui_print_error("Quantidade inválida."); return; }

    if (available < qty) {
        ui_print_error("Saldo insuficiente de moedas.");
        return;
    }

    printf("\n  Confirmar transferência de %.8f %s para %s? [S/n]: ",
           qty, coin_name(ctype), receiver->name);
    char confirm[4];
    read_line(confirm, sizeof(confirm));
    if (confirm[0] == 'n' || confirm[0] == 'N') {
        printf("  Cancelado.\n");
        return;
    }

    /* Build a UserCoin for the transaction */
    UserCoin* tx_coin = (UserCoin*)malloc(sizeof(UserCoin));
    if (!tx_coin) { ui_print_error("Erro de memória."); return; }
    tx_coin->type    = ctype;
    tx_coin->qtdCoin = qty;
    tx_coin->prox    = NULL;

    Transaction* tx = new_transaction(g_current->uuid, receiver->uuid, tx_coin, &g_queue);
    if (!tx) { ui_print_error("Erro ao criar transação."); free(tx_coin); return; }

    ui_spinner("Processando transação...", 800);

    /* Deduct coins from sender, credit receiver */
    UserCoin* sc = g_current->coins;
    while (sc) { if (sc->type == ctype) { sc->qtdCoin -= qty; break; } sc = sc->prox; }

    /* Add coins to receiver */
    UserCoin* rc = receiver->coins;
    UserCoin* rc_prev = NULL;
    while (rc && rc->type != ctype) { rc_prev = rc; rc = rc->prox; }
    if (!rc) {
        rc = (UserCoin*)malloc(sizeof(UserCoin));
        if (rc) { rc->type = ctype; rc->qtdCoin = 0.0; rc->prox = NULL; }
        if (rc_prev) rc_prev->prox = rc; else receiver->coins = rc;
    }
    if (rc) rc->qtdCoin += qty;

    int r = send_to_block(g_users, &g_queue, g_block, time(NULL));
    if (r) {
        ui_print_success("Transação enviada ao bloco com sucesso!");
    } else {
        ui_print_success("Transação registrada na fila.");
    }
}

static void action_profile(void) {
    ui_clear();
    ui_print_separator();
    ui_print_profile(g_current);
    printf("\n");
    printf("  [C] Copiar UUID   [ENTER] Voltar: ");
    char opt[4];
    read_line(opt, sizeof(opt));
    if (opt[0] == 'c' || opt[0] == 'C') {
        ui_copy_uuid(g_current->uuid);
        printf("  Pressione ENTER para continuar...");
        flush_stdin();
    }
}

static void action_buy_coins(void) {
    ui_clear();
    ui_print_separator();
    printf(BOLD "  Comprar Moedas\n" RESET);
    ui_print_separator();

    ui_spinner("Buscando preços...", 600);

    for (int i = 0; i < NUM_COINS; i++) {
        atualizarPrecosAPI((CoinType)i);
    }

    double p_btc  = getPreco(BTC);
    double p_eth  = getPreco(ETH);
    double p_usdt = getPreco(USDT);
    double bal    = g_current->balance;

    printf("\n");
    printf("  %-6s  $%-14.2f  você pode comprar: %.8f\n", "BTC",  p_btc,  p_btc  > 0 ? bal / p_btc  : 0.0);
    printf("  %-6s  $%-14.2f  você pode comprar: %.8f\n", "ETH",  p_eth,  p_eth  > 0 ? bal / p_eth  : 0.0);
    printf("  %-6s  $%-14.4f  você pode comprar: %.2f\n",  "USDT", p_usdt, p_usdt > 0 ? bal / p_usdt : 0.0);
    printf("\n");
    printf("  Seu saldo: $%.2f\n\n", bal);

    printf("  Tipo de moeda  [1] BTC  [2] ETH  [3] USDT: ");
    char choice_buf[8];
    read_line(choice_buf, sizeof(choice_buf));
    int choice = atoi(choice_buf);
    if (choice < 1 || choice > 3) { ui_print_error("Opção inválida."); return; }
    CoinType ctype = (CoinType)(choice - 1);

    printf("  Quantidade a comprar: ");
    char qty_buf[32];
    read_line(qty_buf, sizeof(qty_buf));
    double qty = atof(qty_buf);
    if (qty <= 0.0) { ui_print_error("Quantidade inválida."); return; }

    double price = getPreco(ctype);
    double total = price * qty;
    printf("\n  Total: $%.2f\n", total);
    if (g_current->balance < total) {
        ui_print_error("Saldo insuficiente.");
        return;
    }

    printf("  Confirmar compra? [S/n]: ");
    char confirm[4];
    read_line(confirm, sizeof(confirm));
    if (confirm[0] == 'n' || confirm[0] == 'N') { printf("  Cancelado.\n"); return; }

    int ok = buy_coins(g_current, ctype, qty);
    if (ok) {
        ui_print_success("Compra realizada com sucesso!");
        printf("  Novo saldo: $%.2f\n", g_current->balance);
    } else {
        ui_print_error("Falha na compra.");
    }
}

static void action_add_balance(void) {
    ui_clear();
    ui_print_separator();
    printf(BOLD "  Adicionar Saldo\n" RESET);
    ui_print_separator();

    printf("  Saldo atual: $%.2f\n", g_current->balance);
    printf("  Valor a depositar: $");
    char buf[32];
    read_line(buf, sizeof(buf));
    double amount = atof(buf);
    if (amount <= 0) { ui_print_error("Valor inválido."); return; }

    int ok = add_balance(g_current, amount);
    if (ok) {
        ui_print_success("Saldo adicionado!");
        printf("  Novo saldo: $%.2f\n", g_current->balance);
    } else {
        ui_print_error("Falha ao adicionar saldo.");
    }
}

static void action_merge_coins(void) {
    ui_clear();
    ui_print_separator();
    printf(BOLD "  Trocar Moedas\n" RESET);
    ui_print_separator();

    ui_spinner("Buscando cotações...", 400);

    for (int i = 0; i < NUM_COINS; i++) {
        atualizarPrecosAPI((CoinType)i);
    }

    double p_btc  = getPreco(BTC);
    double p_eth  = getPreco(ETH);
    double p_usdt = getPreco(USDT);

    /* tabela de proporções */
    printf("\n");
    printf(BOLD "  Proporções de troca (valores atuais):\n" RESET);
    printf(DIM "  ──────────────────────────────────────────────────────\n" RESET);
    if (p_btc > 0 && p_eth  > 0) printf("  1 BTC  = %.6f ETH   |  1 ETH  = %.8f BTC\n",  p_btc / p_eth,  p_eth  / p_btc);
    if (p_btc > 0 && p_usdt > 0) printf("  1 BTC  = %.2f USDT  |  1 USDT = %.8f BTC\n",  p_btc / p_usdt, p_usdt / p_btc);
    if (p_eth > 0 && p_usdt > 0) printf("  1 ETH  = %.2f USDT  |  1 USDT = %.8f ETH\n",  p_eth / p_usdt, p_usdt / p_eth);
    printf(DIM "  ──────────────────────────────────────────────────────\n" RESET);

    printf("\n  Seu saldo de moedas:\n");
    printf("  BTC: %.8f  |  ETH: %.8f  |  USDT: %.2f\n\n",
           user_coin_qty(g_current, BTC),
           user_coin_qty(g_current, ETH),
           user_coin_qty(g_current, USDT));

    printf("  Moeda de origem  [1] BTC  [2] ETH  [3] USDT: ");
    char buf[32];
    read_line(buf, sizeof(buf));
    int sc = atoi(buf);
    if (sc < 1 || sc > 3) { ui_print_error("Opção inválida."); return; }
    CoinType src = (CoinType)(sc - 1);

    double src_available = user_coin_qty(g_current, src);
    printf("  Disponível: %.8f %s\n", src_available, coin_name(src));
    printf("  Quantidade a trocar: ");
    read_line(buf, sizeof(buf));
    double sqty = atof(buf);
    if (sqty <= 0.0) { ui_print_error("Quantidade inválida."); return; }

    printf("  Moeda destino  [1] BTC  [2] ETH  [3] USDT: ");
    read_line(buf, sizeof(buf));
    int dc = atoi(buf);
    if (dc < 1 || dc > 3) { ui_print_error("Opção inválida."); return; }
    CoinType dst = (CoinType)(dc - 1);

    /* calcular e mostrar quanto vai receber */
    double src_price = getPreco(src);
    double dst_price = getPreco(dst);
    double dqty = (dst_price > 0 && src_price > 0) ? (sqty * src_price) / dst_price : 0.0;

    printf("\n  Você receberá aproximadamente: " BOLD "%.8f %s\n" RESET, dqty, coin_name(dst));
    printf("  Confirmar troca? [S/n]: ");
    char confirm[4];
    read_line(confirm, sizeof(confirm));
    if (confirm[0] == 'n' || confirm[0] == 'N') { printf("  Cancelado.\n"); return; }

    int ok = merge_coins(g_current, src, sqty, dst, dqty);
    if (ok) {
        ui_print_success("Troca realizada com sucesso!");
        printf("  BTC: %.8f  |  ETH: %.8f  |  USDT: %.2f\n",
               user_coin_qty(g_current, BTC),
               user_coin_qty(g_current, ETH),
               user_coin_qty(g_current, USDT));
    } else {
        ui_print_error("Falha na troca. Verifique saldo e preços.");
    }
}

static void action_live_prices(void) {
    ui_clear();
    ui_print_separator();
    printf(BOLD "  Preços ao Vivo\n" RESET);
    ui_print_separator();

    ui_spinner("Buscando cotações...", 800);

    for (int i = 0; i < NUM_COINS; i++) {
        atualizarPrecosAPI((CoinType)i);
    }

    printf("\n");
    printf("  ╔══════════════════════════╗\n");
    printf("  ║  %-6s  $%14.2f  ║\n", "BTC",  getPreco(BTC));
    printf("  ║  %-6s  $%14.2f  ║\n", "ETH",  getPreco(ETH));
    printf("  ║  %-6s  $%14.4f  ║\n", "USDT", getPreco(USDT));
    printf("  ╚══════════════════════════╝\n\n");
}

/* ── user menu ─────────────────────────────────────────────────────────────── */
static void user_menu(void) {
    while (1) {
        ui_clear();
        ui_print_banner();
        printf(CYAN "  ╔══════════════════════════════════════════════════════╗\n" RESET);
        printf(CYAN "  ║" RESET BOLD "  Olá, %-15s" RESET "      Saldo: " GREEN "$%-12.2f" RESET CYAN "  ║\n" RESET,
               g_current->name, g_current->balance);
        printf(CYAN "  ╠══════════════════════════════════════════════════════╣\n" RESET);
        printf(CYAN "  ║" RESET "  [1] Realizar Transferência  [2] Ver meu Perfil     " CYAN "║\n" RESET);
        printf(CYAN "  ║" RESET "  [3] Comprar Moedas          [4] Adicionar Saldo    " CYAN "║\n" RESET);
        printf(CYAN "  ║" RESET "  [5] Trocar Moedas           [6] Ver Preços ao Vivo " CYAN "║\n" RESET);
        printf(CYAN "  ║" RESET "  [0] Sair                                           " CYAN "║\n" RESET);
        printf(CYAN "  ╚══════════════════════════════════════════════════════╝\n" RESET);
        printf("\n  Escolha: ");

        char choice[8];
        read_line(choice, sizeof(choice));
        int opt = atoi(choice);

        printf("\n");
        switch (opt) {
            case 1: action_transfer();    break;
            case 2: action_profile();     break;
            case 3: action_buy_coins();   break;
            case 4: action_add_balance(); break;
            case 5: action_merge_coins(); break;
            case 6: action_live_prices(); break;
            case 0:
                g_current  = NULL;
                g_is_admin = 0;
                return;
            default:
                ui_print_error("Opção inválida.");
        }
        printf("\n  Pressione ENTER para continuar...");
        flush_stdin();
    }
}

/* ── admin menu ────────────────────────────────────────────────────────────── */
static void admin_menu(void) {
    while (1) {
        ui_clear();
        ui_print_banner();
        printf(YELLOW "  ╔══════════════════════════════════════════════════════╗\n" RESET);
        printf(YELLOW "  ║" RESET BOLD "  PAINEL ADMIN — %-35s" RESET YELLOW "║\n" RESET, g_current->name);
        printf(YELLOW "  ╠══════════════════════════════════════════════════════╣\n" RESET);
        printf(YELLOW "  ║" RESET "  [1] Criar Banco             [2] Listar Usuários    " YELLOW "║\n" RESET);
        printf(YELLOW "  ║" RESET "  [3] Remover Usuário         [4] Criar Admin        " YELLOW "║\n" RESET);
        printf(YELLOW "  ║" RESET "  [0] Sair                                           " YELLOW "║\n" RESET);
        printf(YELLOW "  ╚══════════════════════════════════════════════════════╝\n" RESET);
        printf("\n  Escolha: ");

        char choice[8];
        read_line(choice, sizeof(choice));
        int opt = atoi(choice);

        printf("\n");
        switch (opt) {
            case 1: {
                ui_clear();
                if (g_bank_count >= 10) { ui_print_error("Limite de bancos atingido."); break; }
                char bname[64], bid[32], bcountry[32];
                printf("  Nome do banco: "); read_line(bname, sizeof(bname));
                printf("  ID do banco:   "); read_line(bid, sizeof(bid));
                printf("  País:          "); read_line(bcountry, sizeof(bcountry));
                char* bn = strdup(bname), *bi = strdup(bid), *bc = strdup(bcountry);
                Bank* nb = create_bank(bn, bi, bc);
                if (nb) {
                    g_banks[g_bank_count++] = nb;
                    ui_print_success("Banco criado com sucesso!");
                } else {
                    ui_print_error("Falha ao criar banco.");
                    free(bn); free(bi); free(bc);
                }
                break;
            }
            case 2:
                ui_clear();
                ui_print_separator();
                list_users(g_users);
                ui_print_separator();
                break;
            case 3: {
                ui_clear();
                char del_name[64], del_uuid[64];
                printf("  Nome do usuário (ou ENTER para pular): ");
                read_line(del_name, sizeof(del_name));
                printf("  UUID do usuário (ou ENTER para pular): ");
                read_line(del_uuid, sizeof(del_uuid));
                const char* dn = strlen(del_name) > 0 ? del_name : NULL;
                const char* du = strlen(del_uuid) > 0 ? del_uuid : NULL;
                int ok = delete_user(&g_users, dn, du, g_current->uuid);
                if (ok) ui_print_success("Usuário removido.");
                else    ui_print_error("Usuário não encontrado ou sem permissão.");
                break;
            }
            case 4: {
                ui_clear();
                char aname[64], abday[32], apass[64];
                printf("  Nome do admin:      "); read_line(aname, sizeof(aname));
                printf("  Data de nascimento: "); read_line(abday, sizeof(abday));
                printf("  Chave de acesso:    "); read_password(apass, sizeof(apass));
                Admin* na = create_admin(strdup(aname), strdup(abday), apass);
                if (na) ui_print_success("Admin criado com sucesso!");
                else    ui_print_error("Falha ao criar admin.");
                break;
            }
            case 0:
                g_current  = NULL;
                g_is_admin = 0;
                return;
            default:
                ui_print_error("Opção inválida.");
        }
        printf("\n  Pressione ENTER para continuar...");
        flush_stdin();
    }
}

/* ── auth screen ───────────────────────────────────────────────────────────── */
static void auth_screen(void) {
    ui_clear();
    ui_print_banner();

    printf(CYAN "  ┌─────────────────────────────┐\n" RESET);
    printf(CYAN "  │" RESET "  [1] Login   [2] Registrar  " CYAN "│\n" RESET);
    printf(CYAN "  │" RESET "  [0] Sair                   " CYAN "│\n" RESET);
    printf(CYAN "  └─────────────────────────────┘\n" RESET);
    printf("\n  Escolha: ");

    char choice[8];
    read_line(choice, sizeof(choice));
    int opt = atoi(choice);

    printf("\n");

    if (opt == 0) {
        printf(BOLD "  Até logo!\n" RESET);
        exit(0);
    }

    if (opt == 1) {
        /* Login por CPF + senha */
        char ucpf[16], upass[64];
        printf("  CPF: ");   read_line(ucpf, sizeof(ucpf));
        printf("  Senha: "); read_password(upass, sizeof(upass));

        char* hashed = hash(upass);
        Users* found = find_user_by_cpf(ucpf);
        if (!found || !hashed || strcmp(found->hashPass, hashed) != 0) {
            free(hashed);
            ui_print_error("CPF ou senha inválidos.");
            printf("\n  Pressione ENTER para continuar...");
            flush_stdin();
            return;
        }
        free(hashed);

        g_current  = found;
        g_is_admin = verify_admin(found->uuid);

        ui_spinner("Autenticando...", 500);
        ui_print_success("Login realizado!");
        printf("  Pressione ENTER para continuar...");
        flush_stdin();

        if (g_is_admin) admin_menu();
        else            user_menu();

    } else if (opt == 2) {
        /* Register */
        char uname[64], ucpf[16], upass[64], uage[8];
        printf("  Nome: ");   read_line(uname, sizeof(uname));
        printf("  CPF: ");    read_line(ucpf, sizeof(ucpf));
        printf("  Senha: ");  read_password(upass, sizeof(upass));
        printf("  Idade: ");  read_line(uage, sizeof(uage));

        if (find_user_by_cpf(ucpf)) {
            ui_print_error("CPF já cadastrado.");
            printf("\n  Pressione ENTER para continuar...");
            flush_stdin();
            return;
        }

        Users* nu = new_user(strdup(uname), strdup(ucpf), upass, uage);
        if (!nu) {
            ui_print_error("Falha ao criar usuário. Verifique os dados.");
            printf("\n  Pressione ENTER para continuar...");
            flush_stdin();
            return;
        }

        if (g_bank_count > 0) {
            approve_user_account(g_banks[0], nu);
        }

        save_users_db(nu);

        /* Prepend to list */
        nu->prox = g_users;
        g_users  = nu;
        g_current = nu;

        ui_spinner("Criando conta...", 600);
        ui_print_success("Conta criada com sucesso!");
        printf("  Pressione ENTER para continuar...");
        flush_stdin();

        user_menu();

    } else {
        ui_print_error("Opção inválida.");
        printf("\n  Pressione ENTER para continuar...");
        flush_stdin();
    }
}

/* ── main ──────────────────────────────────────────────────────────────────── */
int main(void) {
    srand((unsigned int)time(NULL));

    /* Default bank */
    Bank* default_bank = create_bank("BlockBank", "BB001", "Brasil");
    if (default_bank) {
        g_banks[g_bank_count++] = default_bank;
    }

    /* Carregar usuários do banco de dados */
    ui_spinner("Carregando dados...", 400);
    g_users = load_users_from_db();

    /* Default block */
    char empty_hash[65];
    memset(empty_hash, '0', 64);
    empty_hash[64] = '\0';
    char block_uuid[37] = "00000000-0000-0000-0000-000000000000";
    g_block = create_block(block_uuid, time(NULL), empty_hash, empty_hash, NULL);

    /* Main loop */
    while (1) {
        auth_screen();
    }

    /* Cleanup (never reached in normal flow) */
    return 0;
}
