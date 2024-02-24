#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>

enum db_table { MODULES, LEVELS, STATUS_EVENTS };

void SELECT(FILE *db, enum db_table db_table, int id);
int INSERT(FILE *db, enum db_table db_table, void *entity);

int DELETE(FILE *db, enum db_table db_table, int id);
int UPDATE(FILE *db, enum db_table db_table, int id, void *entity);
void print_main_menu(void);
void print_dbselect_menu(void);
int get_file_size_in_bytes(FILE *pfile);
void clear_screen();
void print_separator();
void lock_AI(FILE *dbmod, FILE *dblev, FILE *dbstatevents);

#endif