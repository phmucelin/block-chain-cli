#ifndef ADMIN_ACTIONS_H
#define ADMIN_ACTIONS_H

typedef struct Admin Admin;
typedef struct Bank Bank;
typedef struct Users Users;

int verify_isAdmin(char* UUID); // Verify by uuid, we can have another user with the same name of a admin.

Admin* create_admin(char* name, char* birthday, char* passKey);

Bank* create_bank(char* name, char* id_bank, char* country);

void list_users(Users* u);

int delete_user(Users** head, const char* name, const char* uuid, const char* admin_uuid);

#endif