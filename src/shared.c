#include "shared.h"

#include <stdio.h>

#include "master_levels.h"
#include "master_modules.h"
#include "master_status_events.h"

void SELECT(FILE *db, enum db_table db_table, int id) {
    switch (db_table) {
        case MODULES:
            if (id == -1)
                print_all_modules(db);
            else
                print_n_modules(db, id);
            break;
        case LEVELS:
            if (id == -1)
                print_all_levels(db);
            else
                print_n_levels(db, id);
            break;
        case STATUS_EVENTS:
            if (id == -1)
                print_all_status_events(db);
            else
                print_n_status_events(db, id);
            break;
        default:
            break;
    }
}

int INSERT(FILE *db, enum db_table db_table, void *entity) {
    switch (db_table) {
        case MODULES:
            insert_record_into_modules_table(db, (MasterModulesRecord *)entity);
            break;
        case LEVELS:
            insert_record_into_levels_table(db, (MasterLevelsRecord *)entity);
            break;
        case STATUS_EVENTS:
            insert_record_into_status_events_table(db, (MasterStatusEventsRecord *)entity);
            break;
        default:
            break;
    }
    return 0;
}

int DELETE(FILE *db, enum db_table db_table, int id) {
    switch (db_table) {
        case MODULES:
            delete_records_from_modules_table_by_module_id(db, id);
            break;
        case LEVELS:
            delete_records_from_levels_table_by_level_number(db, id);
            break;
        case STATUS_EVENTS:
            delete_records_from_status_events_table_by_event_id(db, id);
            break;
        default:
            break;
    }
    return 0;
}

int UPDATE(FILE *db, enum db_table db_table, int id, void *entity) {
    switch (db_table) {
        case MODULES:
            update_record_in_modules_table_by_id(db, id, (MasterModulesRecord *)entity);
            break;
        case LEVELS:
            update_record_in_levels_table_by_id(db, id, (MasterLevelsRecord *)entity);
            break;
        case STATUS_EVENTS:
            update_record_in_status_events_table_by_id(db, id, (MasterStatusEventsRecord *)entity);
            break;
        default:
            break;
    }
    return 0;
}

int get_file_size_in_bytes(FILE *pfile) {
    int size = 0;
    rewind(pfile);
    fseek(pfile, 0, SEEK_END);
    size = ftell(pfile);
    rewind(pfile);
    return size;
}

void lock_AI(FILE *dbmod, FILE *dblev, FILE *dbstatevents) {
    int records_cnt = get_records_count_in_modules_table(dbmod);
    for (int i = 0; i < records_cnt; ++i) {
        MasterModulesRecord data = read_record_from_modules_table(dbmod, i);
        if (get_last_status_by_module_id(dbstatevents, data.module_id) == 1 && data.module_id != 0) {
            // printf("Module ID: %d\n", data.module_id);
            set_module_status(dbstatevents, data.module_id, 0);
            set_module_deleted_flag_by_id(dbmod, data.module_id);
        }
        if (data.module_level == 1 && data.module_cell == 1 && data.module_id != 0)
            move_module_to_level_cell(dbmod, dblev, data.module_id, 2, 1);
    }
    set_module_status(dbstatevents, 0, 0);
    set_module_status(dbstatevents, 0, 1);
    set_module_status(dbstatevents, 0, 20);
    move_module_to_level_cell(dbmod, dblev, 0, 1, 1);

    set_protection_flag_on_level(dblev, 1, 1);
    printf("\n\nModules on level 1 - Cell 1:\n");
    print_all_modules_by_level_and_cell(dbmod, 1, 1);
}

void print_main_menu(void) {
    print_separator();
    printf("Please choose one operation:\n");
    printf("1. SELECT\n");
    printf("2. INSERT\n");
    printf("3. UPDATE\n");
    printf("4. DELETE\n");
    printf("5. Get all active additional modules (last module status is 1)\n");
    printf("6. Delete modules by ids\n");
    printf("7. Set protected mode for module by id\n");
    printf("8. Move module by id to specified memory level and cell\n");
    printf("9. Set protection flag of the specified memory level\n");
    printf("10. Lock AI in protected mode\n");
    printf("11. Binary search in index tables\n");
    printf("-1. Exit\n");
    print_separator();
}

void print_dbselect_menu(void) {
    print_separator();
    printf("Please choose DB or -1 to return to menu:\n");
    printf("-1. ____^____\n");
    printf("1. MODULES\n");
    printf("2. LEVELS\n");
    printf("3. STATUS EVENTS\n");
    print_separator();
}

void clear_screen() { printf("\e[1;1H\e[2J"); }

void print_separator() { printf("\n|---------------------------------------------------------------|\n"); }