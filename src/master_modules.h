#ifndef MASTER_MODULES_H
#define MASTER_MODULES_H

#include <stdio.h>

#define MASTER_MODULES_DB_PATH "../data/master_modules.db"
#define MASTER_MODULES_IDX_PATH "../data/master_modules.idx"

typedef struct master_modules {
    int module_id;
    char module_name[30];
    int module_level;
    int module_cell;
    int deleted_flag;
} MasterModulesRecord;

MasterModulesRecord select_module_by_id(FILE *db, int id);
int index_modules_by_id(FILE *db, int id);
int get_records_count_in_modules_table(FILE *db);
void print_modules_record(MasterModulesRecord data);
void print_n_modules(FILE *pfile, int n);
MasterModulesRecord read_record_from_modules_table(FILE *db, int index);
int delete_records_from_modules_table(FILE *pfile, int index1, int index2);
int delete_records_from_modules_table_by_module_id(FILE *pfile, int id);
void sort_modules_records_by_id(FILE *pfile);
void print_all_modules(FILE *pfile);
void write_record_in_modules_table(FILE *pfile, const MasterModulesRecord *record_to_write, int index);
void insert_record_into_modules_table(FILE *pfile, const MasterModulesRecord *record_to_write);
void update_record_in_modules_table_by_id(FILE *pfile, int id, const MasterModulesRecord *record_to_write);
void swap_records_in_modules_table(FILE *pfile, int record_index1, int record_index2);

void print_all_modules_with_status_1(FILE *db, FILE *status_db);
void print_all_modules_by_level_and_cell(FILE *pfile, int level, int cell);

void set_module_deleted_flag_by_id(FILE *db, int id);

void move_module_to_level_cell(FILE *db, FILE *leveldb, int module_id, int level, int cell);
void build_modules_index(FILE *db);
#endif