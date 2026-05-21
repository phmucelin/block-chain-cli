#include <libpq-fe.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../models/transaction_model.h"
#include "../models/block_model.h"
#include "../models/user_model.h"
#include "db.h"

static const char* coin_type_str[] = { "BTC", "ETH", "USDT" };

int save_transactions_info_db(Transaction* t)
{
    if (t == NULL || t->coin == NULL || t->uuidSender == NULL || t->uuidReceive == NULL || t->coin->type < 0 || t->coin->type >= sizeof(coin_type_str) / sizeof(coin_type_str[0])) {
        fprintf(stderr, "Transaction or its fields are NULL\n");
        return 0;
    }
    PGconn *conn = try_connect_db();
    if (!conn)
        return 0;

    const char* query =
        "INSERT INTO transactions (sender, receiver, qtdCoins, coinType, timestamp, receipt) "
        "VALUES ($1, $2, $3, $4, to_timestamp($5::bigint), $6) "
        "ON CONFLICT (receipt) DO NOTHING;";

    char qtd_str[64];
    snprintf(qtd_str, sizeof(qtd_str), "%.8f", t->coin->qtdCoin);

    char timestamp_str[64];
    snprintf(timestamp_str, sizeof(timestamp_str), "%ld", (long)time(NULL));

    char receipt_str[64];
    snprintf(receipt_str, sizeof(receipt_str), "%d", t->receipt);

    const char* params[6];
    params[0] = t->uuidSender;
    params[1] = t->uuidReceive;
    params[2] = qtd_str;
    params[3] = coin_type_str[t->coin->type];
    params[4] = timestamp_str;
    params[5] = receipt_str;

    PGresult *res = PQexecParams(conn, query, 6, NULL, params, NULL, NULL, 0);
    int ok = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!ok)
        fprintf(stderr, "Insert transaction failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);
    return ok;
}


int save_blocks_info_db(Block* b)
{
    if(b == NULL || b->num_transactions == 0 || b->transactions == NULL) 
    {
        fprintf(stderr, "Block is NULL or has no transactions\n");
        return 0;
    }
    
    PGconn *conn = try_connect_db();
    if (!conn)
        return 0;
    const char* query =
        "INSERT INTO blocks (blockHash, previousHash, created_at, updated_at, expire_at,numberOfTransactions) "
        "VALUES ($1, $2, to_timestamp($3::bigint), to_timestamp($4::bigint), to_timestamp($5::bigint), $6) "
        "ON CONFLICT (blockHash) DO NOTHING;";
    char timestamp_str[64];
    snprintf(timestamp_str, sizeof(timestamp_str), "%ld", (long)time(NULL));
    char expire_str[64];
    snprintf(expire_str, sizeof(expire_str), "%ld", (long)(time(NULL) + 3600)); // Expire in 1 hour
    char num_tx_str[64];
    snprintf(num_tx_str, sizeof(num_tx_str), "%d", b->num_transactions);
    // preparando parametros para a query
    const char* params[6] = {
        b->hash,
        b->previous_hash,
        timestamp_str,
        timestamp_str, // created_at and updated_at are the same on insert
        expire_str,
        num_tx_str
    };
    
    // realizando a execucao da query, verificando com o PGresult res, para retornar se a insercao foi OK ou nao.
    PGresult *res = PQexecParams(conn, query, 6, NULL, params, NULL, NULL, 0);
    int ok = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!ok)
        fprintf(stderr, "Insert block failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);
    return ok;
}

int save_users_db(Users* u)
{
    if (!u || !u->uuid || !u->cpf || !u->name || !u->hashPass)
        return 0;

    PGconn *conn = try_connect_db();
    if (!conn)
        return 0;

    const char* query =
        "INSERT INTO users (uuid, cpf, name, hashpass, age, balance, bank_name) "
        "VALUES ($1, $2, $3, $4, $5, $6, $7) "
        "ON CONFLICT (uuid) DO NOTHING;";

    char age_str[16];
    snprintf(age_str, sizeof(age_str), "%d", u->age);
    char balance_str[32];
    snprintf(balance_str, sizeof(balance_str), "%.2f", u->balance);
    const char* bank_name = (u->bank && u->bank->name) ? u->bank->name : "BlockBank";

    const char* params[7] = {
        u->uuid, u->cpf, u->name, u->hashPass,
        age_str, balance_str, bank_name
    };

    PGresult *res = PQexecParams(conn, query, 7, NULL, params, NULL, NULL, 0);
    int ok = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!ok)
        fprintf(stderr, "Insert to users table failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);
    return ok;
}

Users* load_users_from_db(void)
{
    PGconn *conn = try_connect_db();
    if (!conn)
        return NULL;

    PGresult *res = PQexec(conn,
        "SELECT uuid, cpf, name, hashpass, age, balance FROM users;");

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Load users failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return NULL;
    }

    Users* head = NULL;
    int rows = PQntuples(res);
    for (int i = rows - 1; i >= 0; i--) {
        Users* u = (Users*)calloc(1, sizeof(Users));
        if (!u) break;
        u->uuid     = strdup(PQgetvalue(res, i, 0));
        u->cpf      = strdup(PQgetvalue(res, i, 1));
        u->name     = strdup(PQgetvalue(res, i, 2));
        u->hashPass = strdup(PQgetvalue(res, i, 3));
        u->age      = atoi(PQgetvalue(res, i, 4));
        u->balance  = atof(PQgetvalue(res, i, 5));
        u->prox     = head;
        head        = u;
    }

    PQclear(res);
    PQfinish(conn);
    return head;
}

int save_tables_relation_coins_user_db(Users* u)
{
    if(u == NULL || strlen(u->uuid) <= 1 || u->uuid == NULL || u->coins->qtdCoin == 0)
        return 0; // error, querendo salvar infos null ou invalidas no banco
    PGconn *conn = try_connect_db();
    if (!conn)
        return 0;
    const char* query =
        "INSERT INTO relation_coins_users (userId, coin_name, qtd_coin) "
        "VALUES ($1, $2, $3) ";
    // preparando parametros para a query
    char qtd_str2[32];
    snprintf(qtd_str2, sizeof(qtd_str2), "%.8f", u->coins->qtdCoin);
    const char* params[3];
        params[0] = u->uuid;
        params[1] = coin_type_str[u->coins->type];
        params[2] = qtd_str2;

    // realizando a execucao da query, verificando com o PGresult res, para retornar se a insercao foi OK ou nao.
    
    PGresult *res = PQexecParams(conn, query, 3, NULL, params, NULL, NULL, 0);
    int ok = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!ok)
        fprintf(stderr, "Insert relation to users and coins failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);
    return ok;
}
