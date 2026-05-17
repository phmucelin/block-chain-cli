#ifndef DB_H
#define DB_H

typedef struct Block Block;
typedef struct Transaction Transaction;
  
bool save_transactions_info_db(Transaction* t);

bool save_block_info_db(Block* t);

#endif