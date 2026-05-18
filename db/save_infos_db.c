#include <libpq-fe.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../models/transaction_model.h"
#include "../models/user_model.h"
#include "db.h"

static const char* coin_type_str[] = { "BTC", "ETH", "USDT" };

int save_transactions_info_db(Transaction* t)
{
    if (t == NULL || t->coin == NULL)
        return 0;

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
