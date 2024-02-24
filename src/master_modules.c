#include "master_modules.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "index.h"
#include "master_levels.h"
#include "master_status_events.h"
#include "shared.h"

void build_modules_index(FILE *db) {
    FILE *indexfile = fopen(MASTER_MODULES_IDX_PATH, "wb");
    fclose(indexfile);
    indexfile = fopen(MASTER_MODULES_IDX_PATH, "rb+");
    int recs_cnt = get_records_count_in_modules_table(db);
    for (int i = 0; i < recs_cnt; i++) {
        MasterModulesRecord rec = read_record_from_modules_table(db, i);
        IndexRecord index_record;
        index_record.record_id = rec.module_id;
        index_record.index = i;
        int offset = i * sizeof(IndexRecord);
        fseek(db, offset, SEEK_SET);
        fwrite(&index_record, sizeof(IndexRecord), 1, indexfile);
        fflush(db);
    }
    fclose(indexfile);
    sort_index_records_by_id(MASTER_MODULES_IDX_PATH);
}

MasterModulesRecord select_module_by_id(FILE *db, int id) {
    MasterModulesRecord module = {-1, "", -1, -1, -1};
    int recs_cnt = get_records_count_in_modules_table(db);
    for (int i = 0; i < recs_cnt; i++) {
        MasterModulesRecord rec = read_record_from_modules_table(db, i);
        if (rec.module_id == id) {
            module = rec;
            break;
        }
    }
    return module;
}

int get_records_count_in_modules_table(FILE *db) {
    return get_file_size_in_bytes(db) / sizeof(MasterModulesRecord);
}

void print_modules_record(MasterModulesRecord data) {
    printf("%d %-30s %d %d %d", data.module_id, data.module_name, data.module_level, data.module_cell,
           data.deleted_flag);
}

MasterModulesRecord read_record_from_modules_table(FILE *db, int index) {
    int offset = index * sizeof(MasterModulesRecord);
    fseek(db, offset, SEEK_SET);
    MasterModulesRecord record;
    fread(&record, sizeof(MasterModulesRecord), 1, db);
    rewind(db);
    return record;
}

int index_modules_by_id(FILE *db, int id) {
    int index = -1;
    MasterModulesRecord rec;
    int recs_cnt = get_records_count_in_modules_table(db);
    for (int i = 0; i < recs_cnt; i++) {
        rec = read_record_from_modules_table(db, i);
        if (rec.module_id == id) {
            index = i;
            break;
        }
    }
    return index;
}

int delete_records_from_modules_table_by_module_id(FILE *pfile, int id) {
    int index = index_modules_by_id(pfile, id);
    int result = -1;
    if (index >= 0) {
        delete_records_from_modules_table(pfile, index, index);
    }
    return result;
}
// Delete records from file in range from index1 to index2 inclusively
int delete_records_from_modules_table(FILE *pfile, int index1, int index2) {
    int recs_cnt = get_records_count_in_modules_table(pfile);
    if (index1 < 0 || index2 < 0 || index1 > recs_cnt || index2 > recs_cnt) {
        return -1;
    }
    if (index1 > index2) {
        return -1;
    }
    int recs_to_delete = index2 - index1 + 1;
    int recs_to_move = recs_cnt - index2 - 1;
    int truncate_size = (recs_cnt - recs_to_delete) * sizeof(MasterModulesRecord);
    if (recs_to_move < 0) {
        return -1;
    }
    int recs_to_move_size = recs_to_move * sizeof(MasterModulesRecord);
    char *buf = malloc(recs_to_move_size);
    if (buf == NULL) {
        return -1;
    }
    fseek(pfile, (index2 + 1) * sizeof(MasterModulesRecord), SEEK_SET);
    fread(buf, recs_to_move_size, 1, pfile);
    fseek(pfile, index1 * sizeof(MasterModulesRecord), SEEK_SET);
    fwrite(buf, recs_to_move_size, 1, pfile);
    fflush(pfile);
    free(buf);
    truncate(MASTER_MODULES_DB_PATH, truncate_size);
    return 0;
}

void print_all_modules(FILE *pfile) {
    int records_cnt = get_records_count_in_modules_table(pfile);
    for (int i = 0; i < records_cnt; ++i) {
        MasterModulesRecord data = read_record_from_modules_table(pfile, i);
        print_modules_record(data);
        printf("\n");
    }
}

void print_all_modules_by_level_and_cell(FILE *pfile, int level, int cell) {
    int records_cnt = get_records_count_in_modules_table(pfile);
    for (int i = 0; i < records_cnt; ++i) {
        MasterModulesRecord data = read_record_from_modules_table(pfile, i);
        if (data.module_level == level && data.module_cell == cell) {
            print_modules_record(data);
            printf("\n");
        }
    }
}

void print_n_modules(FILE *pfile, int n) {
    int records_cnt = get_records_count_in_modules_table(pfile);
    if (n > records_cnt) n = records_cnt;
    for (int i = 0; i < n; ++i) {
        MasterModulesRecord data = read_record_from_modules_table(pfile, i);
        print_modules_record(data);
        printf("\n");
    }
}

void sort_modules_records_by_id(FILE *pfile) {
    int rec_count = get_records_count_in_modules_table(pfile);
    for (int i = 0; i < rec_count; i++)
        for (int j = 0; j < (rec_count - 1) - i; j++) {
            MasterModulesRecord r1 = read_record_from_modules_table(pfile, j);
            MasterModulesRecord r2 = read_record_from_modules_table(pfile, j + 1);
            if (r1.module_id > r2.module_id) {
                swap_records_in_modules_table(pfile, j, j + 1);
            }
        }
}

// Function of mutual transfer of two records in the file by their indexes.
void swap_records_in_modules_table(FILE *pfile, int record_index1, int record_index2) {
    // Read both records from file to variables
    MasterModulesRecord record1 = read_record_from_modules_table(pfile, record_index1);
    MasterModulesRecord record2 = read_record_from_modules_table(pfile, record_index2);

    // Rewrite records in file
    write_record_in_modules_table(pfile, &record1, record_index2);
    write_record_in_modules_table(pfile, &record2, record_index1);
}

void write_record_in_modules_table(FILE *pfile, const MasterModulesRecord *record_to_write, int index) {
    int offset = index * sizeof(MasterModulesRecord);
    fseek(pfile, offset, SEEK_SET);
    fwrite(record_to_write, sizeof(MasterModulesRecord), 1, pfile);
    fflush(pfile);
    rewind(pfile);
}

void insert_record_into_modules_table(FILE *pfile, const MasterModulesRecord *record_to_write) {
    if (index_modules_by_id(pfile, record_to_write->module_id) == -1) {
        int rec_count = get_records_count_in_modules_table(pfile);
        write_record_in_modules_table(pfile, record_to_write, rec_count);
    }
}

void update_record_in_modules_table_by_id(FILE *pfile, int id, const MasterModulesRecord *record_to_write) {
    int index = index_modules_by_id(pfile, id);
    if (index >= 0) write_record_in_modules_table(pfile, record_to_write, index);
}

void print_all_modules_with_status_1(FILE *db, FILE *status_db) {
    int records_cnt = get_records_count_in_modules_table(db);
    for (int i = 0; i < records_cnt; ++i) {
        MasterModulesRecord data = read_record_from_modules_table(db, i);
        if (get_last_status_by_module_id(status_db, data.module_id) == 1)
            printf("Module ID: %d\n", data.module_id);
    }
}

void set_module_deleted_flag_by_id(FILE *db, int id) {
    MasterModulesRecord data = select_module_by_id(db, id);
    data.deleted_flag = 1;
    update_record_in_modules_table_by_id(db, id, &data);
    printf("Module ID: %d DELETED flag is set\n", data.module_id);
}

void move_module_to_level_cell(FILE *db, FILE *leveldb, int module_id, int level, int cell) {
    MasterLevelsRecord rec = select_level_by_id(leveldb, level);
    if (cell <= rec.cells_on_level && rec.protection_flag == 0) {
        int index = index_modules_by_id(db, module_id);
        if (index >= 0) {
            MasterModulesRecord data = read_record_from_modules_table(db, index);
            data.module_level = level;
            data.module_cell = cell;
            update_record_in_modules_table_by_id(db, module_id, &data);
            printf("Module ID: %d is moved to level %d cell %d\n", data.module_id, data.module_level,
                   data.module_cell);
        }
    } else
        printf("ERROR: cell number is out of range or level is protected\n");
}
