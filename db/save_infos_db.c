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
    snprintf(qtd_str, sizeof(qtd_str), "%d", t->coin->qtdCoin);

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
    // Garantir que os hashes não sejam NULL para evitar falhas na inserção
    if (b->hash == NULL) { fprintf(stderr, "Block hash is NULL\n"); PQfinish(conn); return 0; }
    if (b->previous_hash == NULL) { fprintf(stderr, "Block previous hash is NULL\n"); PQfinish(conn); return 0; }
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

int save_tables_relation_coins_user_db(Users* u)
{
    if(u == NULL || strlen(u->uuid) <= 1 || u->uuid == NULL || u->coins->type == NULL || u->coins->qtdCoin == 0)
        return 0; // error, querendo salvar infos null ou invalidas no banco
    PGconn *conn = try_connect_db();
    if (!conn)
        return 0;
    const char* query =
        "INSERT INTO relation_coins_users (userId, coin_name, qtd_coin) "
        "VALUES ($1, $2, $3) ";
    // preparando parametros para a query
    const char* params[3];
        params[0] = u->uuid;
        params[1] = coin_type_str[t->coin->type];,
        params[2] = u->coins->qtdCoin;

    // realizando a execucao da query, verificando com o PGresult res, para retornar se a insercao foi OK ou nao.
    
    PGresult *res = PQexecParams(conn, query, 3, NULL, params, NULL, NULL, 0);
    int ok = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!ok)
        fprintf(stderr, "Insert relation to users and coins failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);
    return ok;
}
