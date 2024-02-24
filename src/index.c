#include "index.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "master_levels.h"
#include "master_modules.h"
#include "master_status_events.h"
#include "shared.h"

int get_records_count_in_index(FILE *db) { return (get_file_size_in_bytes(db) / sizeof(IndexRecord)); }

// void print_index_record(IndexRecord data) { printf("%d %d", data.record_id, data.index); }

IndexRecord read_record_from_index(FILE *db, int index) {
    int offset = index * sizeof(IndexRecord);
    fseek(db, offset, SEEK_SET);
    IndexRecord record;
    fread(&record, sizeof(IndexRecord), 1, db);
    rewind(db);
    return record;
}

/*void print_index(char *index_path) {
    FILE *pfile = fopen(index_path, "rb");
    int records_cnt = get_records_count_in_index(pfile);
    for (int i = 0; i < records_cnt; ++i) {
        IndexRecord data = read_record_from_index(pfile, i);
        print_index_record(data);
        printf("\n");
    }
    fclose(pfile);
}*/

void sort_index_records_by_id(char *index_path) {
    FILE *pfile = fopen(index_path, "rb+");
    int rec_count = get_records_count_in_index(pfile);
    for (int i = 0; i < rec_count; i++)
        for (int j = 0; j < (rec_count - 1) - i; j++) {
            IndexRecord r1 = read_record_from_index(pfile, j);
            IndexRecord r2 = read_record_from_index(pfile, j + 1);
            if (r1.record_id > r2.record_id) {
                swap_records_in_index(pfile, j, j + 1);
                fflush(pfile);
            }
        }
    fclose(pfile);
}

void swap_records_in_index(FILE *pfile, int record_index1, int record_index2) {
    IndexRecord record1 = read_record_from_index(pfile, record_index1);
    IndexRecord record2 = read_record_from_index(pfile, record_index2);
    write_record_in_index(pfile, &record1, record_index2);
    write_record_in_index(pfile, &record2, record_index1);
}

void write_record_in_index(FILE *pfile, const IndexRecord *record_to_write, int index) {
    int offset = index * sizeof(IndexRecord);
    fseek(pfile, offset, SEEK_SET);
    fwrite(record_to_write, sizeof(IndexRecord), 1, pfile);
    fflush(pfile);
    rewind(pfile);
}

int binary_search_index_by_id(char *index_path, int id) {
    FILE *index = fopen(index_path, "rb+");
    int low = 0;
    int high = get_records_count_in_index(index);
    while (low <= high) {
        int mid = (low + high) / 2;

        fseek(index, mid * sizeof(IndexRecord), SEEK_SET);

        IndexRecord rec;
        fread(&rec, sizeof(IndexRecord), 1, index);
        // printf("L: %d M: %d R: %d Id: %d Ind: %d\n", low, mid, high, rec.record_id, rec.index);
        if (rec.record_id == id) {
            fclose(index);
            return rec.record_id;
        } else if (rec.record_id < id) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    fclose(index);
    return -1;
}