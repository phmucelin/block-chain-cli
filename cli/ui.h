#ifndef UI_H
#define UI_H

#include "../models/user_model.h"

/* ANSI color/style macros */
#define RESET    "\033[0m"
#define BOLD     "\033[1m"
#define DIM      "\033[2m"
#define GREEN    "\033[32m"
#define CYAN     "\033[36m"
#define YELLOW   "\033[33m"
#define RED      "\033[31m"
#define BLUE     "\033[34m"
#define MAGENTA  "\033[35m"
#define WHITE    "\033[37m"

void ui_spinner(const char* msg, int ms);
void ui_print_banner(void);
void ui_print_success(const char* msg);
void ui_print_error(const char* msg);
void ui_print_separator(void);
void ui_print_profile(Users* u);
void ui_clear(void);
void ui_copy_uuid(const char* uuid);

#endif
