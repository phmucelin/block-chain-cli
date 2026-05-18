#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include "db.h"

#define CONN_STR "user=blockchain password=blockchain123 dbname=blockchain_db host=localhost port=5434"

PGconn* try_connect_db()
{
    PGconn *conn = PQconnectdb(CONN_STR);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "DB connection failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }
    return conn;
}

int create_tables_transactions_db()
{
    PGconn *conn = try_connect_db();
    if (!conn)
        return 0;

    PGresult *res = PQexec(conn,
        "CREATE TABLE IF NOT EXISTS transactions ("
        "  id        SERIAL PRIMARY KEY,"
        "  sender    VARCHAR(64) NOT NULL,"
        "  receiver  VARCHAR(64) NOT NULL,"
        "  qtdCoins  INTEGER     NOT NULL,"
        "  coinType  VARCHAR(8)  NOT NULL,"
        "  timestamp TIMESTAMP   NOT NULL,"
        "  receipt   INTEGER     NOT NULL UNIQUE"
        ")");

    int ok = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!ok)
        fprintf(stderr, "Create transactions table failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);
    return ok;
}

int create_tables_blocks_db()
{
    PGconn *conn = try_connect_db();
    if (!conn)
        return 0;

    PGresult *res = PQexec(conn,
        "CREATE TABLE IF NOT EXISTS blocks ("
        "  id                   SERIAL PRIMARY KEY,"
        "  blockHash            VARCHAR(64) NOT NULL UNIQUE,"
        "  previousHash         VARCHAR(64),"
        "  timestamp            TIMESTAMP   NOT NULL,"
        "  nonce                INTEGER,"
        "  transactionsID       VARCHAR(64),"
        "  numberOfTransactions INTEGER,"
        "  createdAt            TIMESTAMP,"
        "  updatedAt            TIMESTAMP,"
        "  expireAt             TIMESTAMP"
        ")");

    int ok = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!ok)
        fprintf(stderr, "Create blocks table failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);
    return ok;
}
