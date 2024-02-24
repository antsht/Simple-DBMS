#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>
#include <stdlib.h>

#include "master_levels.h"
#include "master_modules.h"
#include "master_status_events.h"

typedef struct {
    int record_id;
    int index;
} IndexRecord;

int get_records_count_in_index(FILE *db);
// void print_index_record(IndexRecord data);
IndexRecord read_record_from_index(FILE *db, int index);
// void print_index(char *index_path);
void sort_index_records_by_id(char *index_path);
void swap_records_in_index(FILE *pfile, int record_index1, int record_index2);
void write_record_in_index(FILE *pfile, const IndexRecord *record_to_write, int index);
int binary_search_index_by_id(char *index_path, int id);
#endif