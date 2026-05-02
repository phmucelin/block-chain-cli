#ifndef ADMIN_ACTIONS_H
#define ADMIN_ACTIONS_H

typedef struct Admin Admin;
typedef struct Bank Bank;
typedef struct Users Users;

bool verify_isAdmin(char* UUID); // Verify by uuid, we can have another user with the same name of a admin.

Admin* create_admin(char* name, char* birthday, char* passKey);

Bank* create_bank(char* name, char* id_bank, char* country);

void list_users(Users* u);

bool delete_user(char* name, char* uuid); //Recebe UUID?? queria saber se da p usar OR

#endif