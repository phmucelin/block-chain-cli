#ifndef BLOCKS_H
#define BLOCKS_H

typedef struct Block Block;
typedef struct Transaction Transaction;

Block* create_block(char* UUID, char* data, char* time);

void* fill_block(Transaction* t, char* time_init, char* time_final);

#endif