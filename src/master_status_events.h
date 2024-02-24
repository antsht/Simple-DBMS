#ifndef MASTER_STATUS_EVENTS_H
#define MASTER_STATUS_EVENTS_H

#include <stdio.h>

#define MASTER_STATUS_EVENTS_DB_PATH "../data/master_status_events.db"
#define MASTER_STATUS_EVENTS_IDX_PATH "../data/master_status_events.idx"

typedef struct master_status_events {
    int event_id;
    int module_id;
    int new_module_status;
    char status_change_date[10 + 1];  // "dd.mm.yyyy\0"
    char status_change_time[8 + 1];   // "hh:mm:ss\0"
} MasterStatusEventsRecord;

MasterStatusEventsRecord select_status_event_by_id(FILE *db, int id);
int get_records_count_in_status_events_table(FILE *db);
void print_status_events_record(MasterStatusEventsRecord data);
MasterStatusEventsRecord read_record_from_status_events_table(FILE *db, int index);
int index_status_events_by_id(FILE *db, int id);
int delete_records_from_status_events_table_by_event_id(FILE *pfile, int id);
int delete_records_from_status_events_table(FILE *pfile, int index1, int index2);
void sort_status_events_records_by_id(FILE *pfile);
void print_all_status_events(FILE *pfile);
void print_n_status_events(FILE *pfile, int n);
void write_record_in_status_events_table(FILE *pfile, const MasterStatusEventsRecord *record_to_write,
                                         int index);
void insert_record_into_status_events_table(FILE *pfile, const MasterStatusEventsRecord *record_to_write);
void update_record_in_status_events_table_by_id(FILE *pfile, int id,
                                                const MasterStatusEventsRecord *record_to_write);
void swap_records_in_status_events_table(FILE *pfile, int record_index1, int record_index2);

// menu 5 functions
int get_last_status_by_module_id(FILE *db, int module_id);

void set_module_status(FILE *db, int module_id, int new_status);
void build_status_events_index(FILE *db);

#endif