#ifndef BLOCKS_H
#define BLOCKS_H

#include <time.h>

typedef struct Block Block;
typedef struct Transaction Transaction;

Block* create_block(char UUID[37], time_t time, char prev_hash[65], char hash[65], Transaction* transactions);

int fill_block(Transaction* transaction, Block* block);

#endif