#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include "db.h"

/*
  Resumo do setup
  1. Subir o banco:
  docker compose up -d
  2. Verificar que subiu:
  docker compose ps
  # ou conectar direto:
  docker exec -it blockchain_db psql -U blockchain -d blockchain_db
  3. Compilar código C com libpq:
  make db_test
*/

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

int create_tables_users_db()
{
    PGconn *conn = try_connect_db();
    if (!conn)
        return 0;

    PGresult *res = PQexec(conn,
        "CREATE TABLE IF NOT EXISTS users ("
        "  id        SERIAL PRIMARY KEY,"
        "  name    VARCHAR(64) NOT NULL,"
        "  hashpass  VARCHAR(64) NOT NULL,"
        "  age  INTEGER     NOT NULL,"
        "  bank_name  VARCHAR(8)  NOT NULL"
        ")");

    int ok = PQresultStatus(res) == PGRES_COMMAND_OK;
    if (!ok)
        fprintf(stderr, "Create users table failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);
    return ok;
}

int create_tables_relation_coins_user_db()
{
    PGconn *conn = try_connect_db();
    if(!conn) 
        return 0;
    PGresult *res = PQexec(conn, 
        "CREATE TABLE IF NOT EXISTS relation_transaction_users("
        "   userId     SERIAL PRIMARY KEY,"
        "   coin_name  VARCHAR(20) NOT NULL,"
        "   qtd_coin   INTEGER NOT NULL"
        ")");
    int ok = PQresultStatus(res) == PGRES_COMMAND_OK;
    if(!ok)
        fprintf(stderr, "Create relation table failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);
    return ok;
}
