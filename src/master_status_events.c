#include "master_status_events.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "index.h"
#include "master_levels.h"
#include "master_modules.h"
#include "shared.h"

void build_status_events_index(FILE *db) {
    FILE *indexfile = fopen(MASTER_STATUS_EVENTS_IDX_PATH, "wb");
    int recs_cnt = get_records_count_in_status_events_table(db);
    for (int i = 0; i < recs_cnt; i++) {
        MasterStatusEventsRecord rec = read_record_from_status_events_table(db, i);
        IndexRecord index_record;
        index_record.record_id = rec.event_id;
        index_record.index = i;
        fwrite(&index_record, sizeof(IndexRecord), 1, indexfile);
    }
    fclose(indexfile);
    sort_index_records_by_id(MASTER_STATUS_EVENTS_IDX_PATH);
    // print_index(MASTER_STATUS_EVENTS_IDX_PATH);
}

MasterStatusEventsRecord select_status_event_by_id(FILE *db, int id) {
    MasterStatusEventsRecord status_event = {-1, -1, -1, "", ""};
    int recs_cnt = get_records_count_in_status_events_table(db);
    for (int i = 0; i < recs_cnt; i++) {
        MasterStatusEventsRecord rec = read_record_from_status_events_table(db, i);
        if (rec.event_id == id) {
            status_event = rec;
            break;
        }
    }
    return status_event;
}

int get_records_count_in_status_events_table(FILE *db) {
    return get_file_size_in_bytes(db) / sizeof(MasterStatusEventsRecord);
}

void print_status_events_record(MasterStatusEventsRecord data) {
    printf("%d %d %d %s %s", data.event_id, data.module_id, data.new_module_status, data.status_change_date,
           data.status_change_time);
}

MasterStatusEventsRecord read_record_from_status_events_table(FILE *db, int index) {
    int offset = index * sizeof(MasterStatusEventsRecord);
    fseek(db, offset, SEEK_SET);
    MasterStatusEventsRecord record;
    fread(&record, sizeof(MasterStatusEventsRecord), 1, db);
    rewind(db);
    return record;
}

int index_status_events_by_id(FILE *db, int id) {
    int index = -1;
    MasterStatusEventsRecord rec;
    int recs_cnt = get_records_count_in_status_events_table(db);
    for (int i = 0; i < recs_cnt; i++) {
        rec = read_record_from_status_events_table(db, i);
        if (rec.event_id == id) {
            index = i;
            break;
        }
    }
    return index;
}

int delete_records_from_status_events_table_by_event_id(FILE *pfile, int id) {
    int index = index_status_events_by_id(pfile, id);
    int result = -1;
    if (index >= 0) {
        delete_records_from_status_events_table(pfile, index, index);
    }
    return result;
}

// Delete records from file in range from index1 to index2 inclusively
int delete_records_from_status_events_table(FILE *pfile, int index1, int index2) {
    int recs_cnt = get_records_count_in_status_events_table(pfile);
    if (index1 < 0 || index2 < 0 || index1 > recs_cnt || index2 > recs_cnt) {
        return -1;
    }
    if (index1 > index2) {
        return -1;
    }
    int recs_to_delete = index2 - index1 + 1;
    int recs_to_move = recs_cnt - index2 - 1;
    int truncate_size = (recs_cnt - recs_to_delete) * sizeof(MasterStatusEventsRecord);
    if (recs_to_move < 0) {
        return -1;
    }
    int recs_to_move_size = recs_to_move * sizeof(MasterStatusEventsRecord);
    char *buf = malloc(recs_to_move_size);
    if (buf == NULL) {
        return -1;
    }
    fseek(pfile, (index2 + 1) * sizeof(MasterStatusEventsRecord), SEEK_SET);
    fread(buf, recs_to_move_size, 1, pfile);
    fseek(pfile, index1 * sizeof(MasterStatusEventsRecord), SEEK_SET);
    fwrite(buf, recs_to_move_size, 1, pfile);
    fflush(pfile);
    free(buf);
    truncate(MASTER_STATUS_EVENTS_DB_PATH, truncate_size);
    return 0;
}

void print_all_status_events(FILE *pfile) {
    int records_cnt = get_records_count_in_status_events_table(pfile);
    for (int i = 0; i < records_cnt; ++i) {
        MasterStatusEventsRecord data = read_record_from_status_events_table(pfile, i);
        print_status_events_record(data);
        printf("\n");
    }
}

void print_n_status_events(FILE *pfile, int n) {
    int records_cnt = get_records_count_in_status_events_table(pfile);
    if (n > records_cnt) n = records_cnt;
    for (int i = 0; i < n; ++i) {
        MasterStatusEventsRecord data = read_record_from_status_events_table(pfile, i);
        print_status_events_record(data);
        printf("\n");
    }
}

void sort_status_events_records_by_id(FILE *pfile) {
    int rec_count = get_records_count_in_status_events_table(pfile);
    for (int i = 0; i < rec_count; i++)
        for (int j = 0; j < (rec_count - 1) - i; j++) {
            MasterStatusEventsRecord r1 = read_record_from_status_events_table(pfile, j);
            MasterStatusEventsRecord r2 = read_record_from_status_events_table(pfile, j + 1);
            if (r1.event_id > r2.event_id) {
                swap_records_in_status_events_table(pfile, j, j + 1);
            }
        }
}

// Function of mutual transfer of two records in the file by their indexes.
void swap_records_in_status_events_table(FILE *pfile, int record_index1, int record_index2) {
    // Read both records from file to variables
    MasterStatusEventsRecord record1 = read_record_from_status_events_table(pfile, record_index1);
    MasterStatusEventsRecord record2 = read_record_from_status_events_table(pfile, record_index2);

    // Rewrite records in file
    write_record_in_status_events_table(pfile, &record1, record_index2);
    write_record_in_status_events_table(pfile, &record2, record_index1);
}

void write_record_in_status_events_table(FILE *pfile, const MasterStatusEventsRecord *record_to_write,
                                         int index) {
    int offset = index * sizeof(MasterStatusEventsRecord);
    fseek(pfile, offset, SEEK_SET);
    fwrite(record_to_write, sizeof(MasterStatusEventsRecord), 1, pfile);
    fflush(pfile);
    rewind(pfile);
    printf("Record was written in file: %d\n", index);
}

void insert_record_into_status_events_table(FILE *pfile, const MasterStatusEventsRecord *record_to_write) {
    int rec_count = get_records_count_in_status_events_table(pfile);
    write_record_in_status_events_table(pfile, record_to_write, rec_count);
}

void update_record_in_status_events_table_by_id(FILE *pfile, int id,
                                                const MasterStatusEventsRecord *record_to_write) {
    int index = index_status_events_by_id(pfile, id);
    if (index >= 0) write_record_in_status_events_table(pfile, record_to_write, index);
}

int get_last_status_by_module_id(FILE *db, int module_id) {
    int last_status = -1;
    int recs_cnt = get_records_count_in_status_events_table(db);
    for (int i = 0; i < recs_cnt; i++) {
        MasterStatusEventsRecord rec = read_record_from_status_events_table(db, i);
        if (rec.module_id == module_id) {
            last_status = rec.new_module_status;
        }
    }
    return last_status;
}

void set_module_status(FILE *db, int module_id, int new_status) {
    int recs_cnt = get_records_count_in_status_events_table(db);
    int last_rec_id = read_record_from_status_events_table(db, recs_cnt - 1).event_id;
    last_rec_id++;
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char date_str[10 + 1];
    strftime(date_str, 10 + 1, "%d.%m.%Y", tm);
    char time_str[8 + 1];
    strftime(time_str, 8 + 1, "%H:%M:%S", tm);
    MasterStatusEventsRecord rec;
    rec.event_id = last_rec_id;
    rec.module_id = module_id;
    rec.new_module_status = new_status;
    strcpy(rec.status_change_date, date_str);
    strcpy(rec.status_change_time, time_str);
    printf("Event: id %d, module %d, new status %d, date %s, time %s\n", rec.event_id, rec.module_id,
           rec.new_module_status, rec.status_change_date, rec.status_change_time);
    insert_record_into_status_events_table(db, &rec);
    printf("Module %d status changed to %d\n", module_id, new_status);
}
