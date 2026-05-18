#ifndef DB_H
#define DB_H

#include <libpq-fe.h>
  
typedef struct Block Block;
typedef struct Transaction Transaction;

int try_connect_db(PGconn **conn);

int disconnect_db(PGconn *conn);

int create_tables_transactions_db(PGconn *conn);

int create_tables_blocks_db(PGconn *conn);

int save_transactions_info_db(Transaction* t);

int save_block_info_db(Block* t);

#endif