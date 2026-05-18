#ifndef DB_H
#define DB_H

#include <libpq-fe.h>

typedef struct Block Block;
typedef struct Transaction Transaction;

PGconn* try_connect_db();

int create_tables_transactions_db();
int create_tables_blocks_db();

int save_transactions_info_db(Transaction* t);
int save_block_info_db(Block* b);

#endif
