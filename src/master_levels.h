#ifndef MASTER_LEVELS_H
#define MASTER_LEVELS_H

#include <stdio.h>
#include <stdlib.h>

#define MASTER_LEVELS_DB_PATH "../data/master_levels.db"
#define MASTER_LEVELS_IDX_PATH "../data/master_levels.idx"

typedef struct master_levels {
    int level_number;
    int cells_on_level;
    int protection_flag;
} MasterLevelsRecord;

MasterLevelsRecord select_level_by_id(FILE *db, int id);
int index_level_by_id(FILE *db, int id);
int get_records_count_in_levels_table(FILE *db);
void print_levels_record(MasterLevelsRecord data);
void print_n_levels(FILE *pfile, int n);
MasterLevelsRecord read_record_from_levels_table(FILE *db, int index);
int delete_records_from_levels_table(FILE *pfile, int index1, int index2);
int delete_records_from_levels_table_by_level_number(FILE *pfile, int level_number);
void sort_levels_records_by_id(FILE *pfile);
void print_all_levels(FILE *pfile);
void write_record_in_levels_table(FILE *pfile, const MasterLevelsRecord *record_to_write, int index);
void insert_record_into_levels_table(FILE *pfile, const MasterLevelsRecord *record_to_write);
void update_record_in_levels_table_by_id(FILE *pfile, int id, const MasterLevelsRecord *record_to_write);
void swap_records_in_levels_table(FILE *pfile, int record_index1, int record_index2);
void set_protection_flag_on_level(FILE *pfile, int level_number, int flag);
void build_levels_index(FILE *db);
#endif