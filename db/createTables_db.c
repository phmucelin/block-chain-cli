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

static PGresult* res = NULL;
static PGconn* conn = NULL;

int try_connect_db()
{
    PGconn *conn = PQconnectdb("user=postgres password=blockchain123 dbname=blockchain_db host=localhost port=5434");
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }
    PQfinish(conn);
    return 1;  
}

int create_tables_transactions_db(PGconn *conn)
{
  int try = try_connect_db();
  if(!try)
    return 0;
    
  res = PQexec(conn, "CREATE TABLE IF NOT EXISTS transactions "
            "(id SERIAL PRIMARY KEY, sender VARCHAR(64), "
            "receiver VARCHAR(64), amount DOUBLE PRECISION, coinType VARCHAR(64), "
            "coinType VARCHAR(64), timestamp TIMESTAMP, receipt INTEGER)");
  if(PQresultStatus(res) != PGRES_COMMAND_OK)
      return 0;
  clearRes();
  return 1;
}

int create_tables_blocks_db(PGconn *conn)
{
  int try = try_connect_db();
  if(!try)
    return 0;
    
  res = PQexec(conn, "CREATE TABLE IF NOT EXISTS blocks "
            "(id SERIAL PRIMARY KEY, blockHash VARCHAR(64), "
            "previousHash VARCHAR(64), timestamp TIMESTAMP, nonce INTEGER, "
            "transactionsID VARCHAR(64), numberOfTransactions INTEGER, createdAt TIMESTAMP, "
            "updatedAt TIMESTAMP, expireAt TIMESTAMP)");
            if(PQresultStatus(res) != PGRES_COMMAND_OK)
      return 0;
  clearRes();   
  return 1;
}
/*
 
-------> EXEMPLOS TIRADO DA LIB PARA EXECUTAR QUERYS E PEGAR RESULTADOS

*********************ATENCAO*********************
************NAO APAGAR ATE SEGUNDA ORDEM!************
    int main(){
    
    res = PQexec(conn, "CREATE TABLE IF NOT EXISTS phonebook "
                "(id SERIAL PRIMARY KEY, name VARCHAR(64), "
                "phone VARCHAR(64), last_changed TIMESTAMP)");
        if(PQresultStatus(res) != PGRES_COMMAND_OK)
            terminate(1);
        clearRes();
        
        res = PQexec(conn, "DELETE FROM phonebook");
        if(PQresultStatus(res) != PGRES_COMMAND_OK)
            terminate(1);
        clearRes();

        const char* query =
            "INSERT INTO phonebook (name, phone, last_changed) "
            " VALUES ($1, $2, now());";
        const char* params[2];

        for(int i = 0; ; i++)
        {
            /* const char* user = [i][0];
            const char* phone = user_phone_arr[i][1];
            if(user == NULL || phone == NULL)
                break;

            params[0] = user;
            params[1] = phone;

            res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);
            if(PQresultStatus(res) != PGRES_COMMAND_OK)
                terminate(1);
            clearRes(); */
/*
        }

        res = PQexec(conn, "SELECT id, name, phone, last_changed "
                        "FROM phonebook");
        if(PQresultStatus(res) != PGRES_TUPLES_OK)
            terminate(1);

        int ncols = PQnfields(res);
        printf("There are %d columns:", ncols);
        for(int i = 0; i < ncols; i++)
        {
            char *name = PQfname(res, i);
            printf(" %s", name);
        }
        printf("\n");

        int nrows = PQntuples(res);
        for(int i = 0; i < nrows; i++)
        {
            char* id = PQgetvalue(res, i, 0);
            char* name = PQgetvalue(res, i, 1);
            char* phone = PQgetvalue(res, i, 2);
            char* last_changed = PQgetvalue(res, i, 3);
            printf("Id: %s, Name: %s, Phone: %s, Last changed: %s\n",
                id, name, phone, last_changed);
        }

        printf("Total: %d rows\n", nrows);

        clearRes();
        terminate(0);
        return 0;
        
    }
*/