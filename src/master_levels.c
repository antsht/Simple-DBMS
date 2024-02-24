#include "master_levels.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "index.h"
#include "master_modules.h"
#include "master_status_events.h"
#include "shared.h"

void build_levels_index(FILE *db) {
    FILE *indexfile = fopen(MASTER_LEVELS_IDX_PATH, "wb");
    int recs_cnt = get_records_count_in_levels_table(db);
    for (int i = 0; i < recs_cnt; i++) {
        MasterLevelsRecord rec = read_record_from_levels_table(db, i);
        IndexRecord index_record;
        index_record.record_id = rec.level_number;
        index_record.index = i;
        fwrite(&index_record, sizeof(IndexRecord), 1, indexfile);
    }
    fclose(indexfile);
    sort_index_records_by_id(MASTER_LEVELS_IDX_PATH);
    // print_index(MASTER_LEVELS_IDX_PATH);
}

MasterLevelsRecord select_level_by_id(FILE *db, int id) {
    MasterLevelsRecord level = {-1, -1, -1};
    int recs_cnt = get_records_count_in_levels_table(db);
    for (int i = 0; i < recs_cnt; i++) {
        MasterLevelsRecord rec = read_record_from_levels_table(db, i);
        if (rec.level_number == id) {
            level = rec;
            break;
        }
    }
    return level;
}

int index_level_by_id(FILE *db, int id) {
    int index = -1;
    MasterLevelsRecord rec;
    int recs_cnt = get_records_count_in_levels_table(db);
    for (int i = 0; i < recs_cnt; i++) {
        rec = read_record_from_levels_table(db, i);
        if (rec.level_number == id) {
            index = i;
            break;
        }
    }
    return index;
}

int get_records_count_in_levels_table(FILE *db) {
    return get_file_size_in_bytes(db) / sizeof(MasterLevelsRecord);
}

void print_levels_record(MasterLevelsRecord data) {
    printf("%d %d %d", data.level_number, data.cells_on_level, data.protection_flag);
}

MasterLevelsRecord read_record_from_levels_table(FILE *db, int index) {
    int offset = index * sizeof(MasterLevelsRecord);
    fseek(db, offset, SEEK_SET);
    MasterLevelsRecord record;
    fread(&record, sizeof(MasterLevelsRecord), 1, db);
    rewind(db);
    return record;
}

int delete_records_from_levels_table_by_level_number(FILE *pfile, int level_number) {
    int index = index_level_by_id(pfile, level_number);
    int result = -1;
    if (index >= 0) {
        delete_records_from_levels_table(pfile, index, index);
    }
    return result;
}

// Delete records from file in range from index1 to index2 inclusively
int delete_records_from_levels_table(FILE *pfile, int index1, int index2) {
    int recs_cnt = get_records_count_in_levels_table(pfile);
    if (index1 < 0 || index2 < 0 || index1 > recs_cnt || index2 > recs_cnt) {
        return -1;
    }
    if (index1 > index2) {
        return -1;
    }
    int recs_to_delete = index2 - index1 + 1;
    int recs_to_move = recs_cnt - index2 - 1;
    int truncate_size = (recs_cnt - recs_to_delete) * sizeof(MasterLevelsRecord);
    if (recs_to_move < 0) {
        return -1;
    }
    int recs_to_move_size = recs_to_move * sizeof(MasterLevelsRecord);
    char *buf = malloc(recs_to_move_size);
    if (buf == NULL) {
        return -1;
    }
    fseek(pfile, (index2 + 1) * sizeof(MasterLevelsRecord), SEEK_SET);
    fread(buf, recs_to_move_size, 1, pfile);
    fseek(pfile, index1 * sizeof(MasterLevelsRecord), SEEK_SET);
    fwrite(buf, recs_to_move_size, 1, pfile);
    fflush(pfile);
    free(buf);
    truncate(MASTER_LEVELS_DB_PATH, truncate_size);
    return 0;
}

void print_all_levels(FILE *pfile) {
    int records_cnt = get_records_count_in_levels_table(pfile);
    for (int i = 0; i < records_cnt; ++i) {
        MasterLevelsRecord data = read_record_from_levels_table(pfile, i);
        print_levels_record(data);
        printf("\n");
    }
}

void print_n_levels(FILE *pfile, int n) {
    int records_cnt = get_records_count_in_levels_table(pfile);
    if (n > records_cnt) n = records_cnt;
    for (int i = 0; i < n; ++i) {
        MasterLevelsRecord data = read_record_from_levels_table(pfile, i);
        print_levels_record(data);
        printf("\n");
    }
}

void sort_levels_records_by_id(FILE *pfile) {
    int rec_count = get_records_count_in_levels_table(pfile);
    for (int i = 0; i < rec_count; i++)
        for (int j = 0; j < (rec_count - 1) - i; j++) {
            MasterLevelsRecord r1 = read_record_from_levels_table(pfile, j);
            MasterLevelsRecord r2 = read_record_from_levels_table(pfile, j + 1);
            if (r1.level_number > r2.level_number) {
                swap_records_in_levels_table(pfile, j, j + 1);
            }
        }
}

// Function of mutual transfer of two records in the file by their indexes.
void swap_records_in_levels_table(FILE *pfile, int record_index1, int record_index2) {
    // Read both records from file to variables
    MasterLevelsRecord record1 = read_record_from_levels_table(pfile, record_index1);
    MasterLevelsRecord record2 = read_record_from_levels_table(pfile, record_index2);

    // Rewrite records in file
    write_record_in_levels_table(pfile, &record1, record_index2);
    write_record_in_levels_table(pfile, &record2, record_index1);
}

void write_record_in_levels_table(FILE *pfile, const MasterLevelsRecord *record_to_write, int index) {
    int offset = index * sizeof(MasterLevelsRecord);
    fseek(pfile, offset, SEEK_SET);
    fwrite(record_to_write, sizeof(MasterLevelsRecord), 1, pfile);
    fflush(pfile);
    rewind(pfile);
}

void insert_record_into_levels_table(FILE *pfile, const MasterLevelsRecord *record_to_write) {
    if (index_level_by_id(pfile, record_to_write->level_number) == -1) {
        int rec_count = get_records_count_in_levels_table(pfile);
        write_record_in_levels_table(pfile, record_to_write, rec_count);
    }
}

void update_record_in_levels_table_by_id(FILE *pfile, int id, const MasterLevelsRecord *record_to_write) {
    int index = index_level_by_id(pfile, id);
    if (index >= 0) {
        write_record_in_levels_table(pfile, record_to_write, index);
        printf("Record with id %d was updated\n", id);
    } else {
        printf("Record with id %d does not exist\n", id);
    }
}

void set_protection_flag_on_level(FILE *pfile, int level_number, int flag) {
    MasterLevelsRecord rec = select_level_by_id(pfile, level_number);
    if (rec.level_number != -1) {
        rec.protection_flag = flag;
        update_record_in_levels_table_by_id(pfile, level_number, &rec);
        printf("Protection flag for level %d was set to %d\n", level_number, flag);
    } else {
        printf("Level %d does not exist\n", level_number);
    }
}