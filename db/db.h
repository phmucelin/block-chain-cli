#ifndef DB_H
#define DB_H

#include <libpq-fe.h>

typedef struct Block Block;
typedef struct Transaction Transaction;
typedef struct Users Users;

PGconn* try_connect_db();

int create_tables_transactions_db();
int create_tables_blocks_db();
int create_tables_users_db();
int create_tables_relation_coins_user_db();

int save_transactions_info_db(Transaction* t);
int save_blocks_info_db(Block* b);
int save_users_db(Users* u);
int save_tables_relation_coins_user_db(Users* u);
Users* load_users_from_db(void);

#endif
