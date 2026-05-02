#ifndef DB_H
#define DB_H

typedef struct User User;
typedef struct Transaction Transaction;

bool save_user_infos_db(User* r);

bool save_transactions_info_db(Transaction* t);

#endif