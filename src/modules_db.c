#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "index.h"
#include "master_levels.h"
#include "master_modules.h"
#include "master_status_events.h"
#include "shared.h"

void main_menu(FILE *db_mod, FILE *db_lev, FILE *db_stat);
void sub_menu_select(FILE *db_mod, FILE *db_lev, FILE *db_stat);
void sub_menu_insert(FILE *db_mod, FILE *db_lev, FILE *db_stat);
void sub_menu_update(FILE *db_mod, FILE *db_lev, FILE *db_stat);
void sub_menu_delete(FILE *db_mod, FILE *db_lev, FILE *db_stat);
void sub_menu_binary_search_in_index();

int main(void) {
    clear_screen();
    FILE *db_mod = fopen(MASTER_MODULES_DB_PATH, "rb+");
    FILE *db_lev = fopen(MASTER_LEVELS_DB_PATH, "rb+");
    FILE *db_stat = fopen(MASTER_STATUS_EVENTS_DB_PATH, "rb+");

    build_status_events_index(db_stat);
    build_levels_index(db_lev);
    build_modules_index(db_mod);

    main_menu(db_mod, db_lev, db_stat);

    fclose(db_mod);
    fclose(db_lev);
    fclose(db_stat);
    return 0;
}

void main_menu(FILE *db_mod, FILE *db_lev, FILE *db_stat) {
    int selected = 0;
    while (selected != -1) {
        print_main_menu();
        if (scanf("%d", &selected) == 1 && selected >= -1 && selected <= 11) {
            switch (selected) {
                case 1: {
                    sub_menu_select(db_mod, db_lev, db_stat);
                    break;
                }
                case 2: {
                    sub_menu_insert(db_mod, db_lev, db_stat);
                    break;
                }
                case 3: {
                    sub_menu_update(db_mod, db_lev, db_stat);
                    break;
                }
                case 4: {
                    sub_menu_delete(db_mod, db_lev, db_stat);
                    break;
                }
                case 5: {
                    print_all_modules_with_status_1(db_mod, db_stat);
                    break;
                }
                case 6: {
                    getchar();
                    char id_to_delete[5000] = {0};
                    fgets(id_to_delete, 5000, stdin);
                    if (strlen(id_to_delete) > 1) {
                        char *istr = NULL;
                        char *sep = " \n";
                        istr = strtok(id_to_delete, sep);
                        while (istr != NULL) {
                            int id = atoi(istr);
                            set_module_deleted_flag_by_id(db_mod, id);
                            istr = strtok(NULL, sep);
                        }
                    }

                    build_modules_index(db_mod);
                    break;
                }
                case 7: {
                    int module_id_to_set_status = -1;
                    printf("Enter the module id: ");
                    if (scanf("%d", &module_id_to_set_status) == 1 && module_id_to_set_status >= 0) {
                        set_module_status(db_stat, module_id_to_set_status, 0);
                        set_module_status(db_stat, module_id_to_set_status, 1);
                        set_module_status(db_stat, module_id_to_set_status, 20);
                    }
                    build_status_events_index(db_stat);
                    break;
                }
                case 8: {
                    int module_id_to_move = -1;
                    int new_level = -1;
                    int new_cell = -1;
                    printf("Enter the module id new_level new_cell: ");
                    if (scanf("%d %d %d", &module_id_to_move, &new_level, &new_cell) == 3 &&
                        module_id_to_move >= 0 && new_level >= 0 && new_cell >= 0) {
                        move_module_to_level_cell(db_mod, db_lev, module_id_to_move, new_level, new_cell);
                    }
                    build_modules_index(db_mod);
                    break;
                }
                case 9: {
                    int level_id_to_protect = -1;
                    printf("Enter the level: ");
                    if (scanf("%d", &level_id_to_protect) == 1 && level_id_to_protect >= 0) {
                        set_protection_flag_on_level(db_lev, level_id_to_protect, 1);
                    }
                    build_levels_index(db_lev);
                    break;
                }
                case 10: {
                    lock_AI(db_mod, db_lev, db_stat);
                    build_status_events_index(db_stat);
                    build_levels_index(db_lev);
                    build_modules_index(db_mod);
                    break;
                }
                case 11: {
                    sub_menu_binary_search_in_index();
                }
                case 0: {
                    break;
                }
                case -999: {
                    select_status_event_by_id(NULL, -1);
                    sort_levels_records_by_id(NULL);
                    sort_modules_records_by_id(NULL);
                    sort_status_events_records_by_id(NULL);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}

void sub_menu_binary_search_in_index() {
    int sub = 0;
    while (sub != -1) {
        printf("\nSelect the table to search in: \n");
        print_dbselect_menu();
        if (scanf("%d", &sub) == 1 && sub > -1 && sub <= 3) {
            printf("Enter the record id to search for: ");
            int rec_id = -1;
            if (scanf("%d", &rec_id) == 1 && rec_id >= 0) {
                int src_res = -1;
                switch (sub) {
                    case 1:
                        src_res = binary_search_index_by_id(MASTER_MODULES_IDX_PATH, rec_id);
                        if (src_res != -1)
                            printf("Record index id: %d\n\n", src_res);
                        else
                            printf("There is no record with ID: %d\n\n", rec_id);

                        break;
                    case 2:
                        src_res = binary_search_index_by_id(MASTER_LEVELS_IDX_PATH, rec_id);
                        if (src_res != -1)
                            printf("Record index id: %d\n\n", src_res);
                        else
                            printf("There is no record with ID: %d\n\n", rec_id);
                        break;
                    case 3:
                        src_res = binary_search_index_by_id(MASTER_STATUS_EVENTS_IDX_PATH, rec_id);
                        if (src_res != -1)
                            printf("Record index id: %d\n\n", src_res);
                        else
                            printf("There is no record with ID: %d\n\n", rec_id);
                        break;

                    default:
                        break;
                }
            }
        }
    }
}

void sub_menu_select(FILE *db_mod, FILE *db_lev, FILE *db_stat) {
    int sub = 0;
    while (sub != -1) {
        print_dbselect_menu();
        if (scanf("%d", &sub) == 1 && sub > -1 && sub <= 3) {
            printf("\nInsert the number of records or 0 to output all of them: ");
            int recs_to_show = -1;
            scanf("%d", &recs_to_show);
            if (recs_to_show < 1) recs_to_show = -1;
            switch (sub) {
                case 1:
                    SELECT(db_mod, MODULES, recs_to_show);
                    break;
                case 2:
                    SELECT(db_lev, LEVELS, recs_to_show);
                    break;
                case 3:
                    SELECT(db_stat, STATUS_EVENTS, recs_to_show);
                    break;

                default:
                    break;
            }
        }
    }
}

void sub_menu_insert(FILE *db_mod, FILE *db_lev, FILE *db_stat) {
    int sub = 0;
    while (sub != -1) {
        print_dbselect_menu();
        if (scanf("%d", &sub) == 1 && sub > -1 && sub <= 3) {
            switch (sub) {
                case 1:
                    MasterModulesRecord rec1 = {-1, "", -1, -1, -1};
                    printf("\nEnter new record: \n");
                    printf("Module_id: ");
                    scanf("%d", &rec1.module_id);
                    printf("Module_name: ");
                    fgets(rec1.module_name, 30, stdin);
                    rec1.module_name[strlen(rec1.module_name) - 1] = '\0';
                    printf("Module_level: ");
                    scanf("%d", &rec1.module_level);
                    printf("Module_cell: ");
                    scanf("%d", &rec1.module_cell);
                    printf("Del_flag: ");
                    scanf("%d", &rec1.deleted_flag);

                    if (rec1.module_id >= 0 && rec1.module_level >= 0 && rec1.module_cell &&
                        (rec1.deleted_flag == 0 || rec1.deleted_flag == 1) && strlen(rec1.module_name) > 0)
                        INSERT(db_mod, MODULES, (void *)(&rec1));
                    build_modules_index(db_mod);
                    break;
                case 2:
                    MasterLevelsRecord rec2 = {-1, -1, -1};
                    printf("\nEnter new record: \nLevel_number Cells_on_level Protection_flag:\n");
                    if (scanf("%d %d %d", &rec2.level_number, &rec2.cells_on_level, &rec2.protection_flag) ==
                            3 &&
                        rec2.cells_on_level > 0 && rec2.protection_flag >= 0 && rec2.protection_flag <= 1 &&
                        rec2.cells_on_level >= 0)
                        INSERT(db_lev, LEVELS, (void *)(&rec2));
                    build_levels_index(db_lev);
                    break;
                case 3:
                    MasterStatusEventsRecord rec3 = {-1, -1, -1, "", ""};
                    printf(
                        "\nEnter new record: \nEvent_id Module_id New_module_status Status_change_date "
                        "Status_change_time:\n");
                    if (scanf("%d %d %d %10s %8s", &rec3.event_id, &rec3.module_id, &rec3.new_module_status,
                              rec3.status_change_date, rec3.status_change_time) == 5)
                        INSERT(db_stat, STATUS_EVENTS, (void *)(&rec3));
                    build_status_events_index(db_stat);
                    break;
                default:
                    break;
            }
        }
    }
}

void sub_menu_update(FILE *db_mod, FILE *db_lev, FILE *db_stat) {
    int sub = 0;
    while (sub != -1) {
        print_dbselect_menu();
        if (scanf("%d", &sub) == 1 && sub > -1 && sub <= 3) {
            switch (sub) {
                case 1:
                    printf("\nEnter Module_ID to update: ");
                    int module_id;
                    if (scanf("%d", &module_id) == 1 && module_id >= 0) {
                        MasterModulesRecord rec1 = {-1, "", -1, -1, -1};
                        printf("Enter new record: \n");
                        printf("Module_id: ");
                        scanf("%d", &rec1.module_id);
                        printf("Module_name: ");
                        fgets(rec1.module_name, 30, stdin);
                        rec1.module_name[strlen(rec1.module_name) - 1] = '\0';
                        printf("Module_level: ");
                        scanf("%d", &rec1.module_level);
                        printf("Module_cell: ");
                        scanf("%d", &rec1.module_cell);
                        printf("Del_flag: ");
                        scanf("%d", &rec1.deleted_flag);

                        if (rec1.module_id >= 0 && rec1.module_level >= 0 && rec1.module_cell &&
                            (rec1.deleted_flag == 0 || rec1.deleted_flag == 1) &&
                            strlen(rec1.module_name) > 0)
                            UPDATE(db_mod, MODULES, rec1.module_id, (void *)(&rec1));
                        build_modules_index(db_mod);
                    }
                    break;
                case 2:
                    printf("\nEnter Level_ID to update: ");
                    int level_id;
                    if (scanf("%d", &level_id) == 1 && level_id >= 0) {
                        MasterLevelsRecord rec2 = {-1, -1, -1};
                        printf(
                            "\nEnter new record. Format: \nLevel_number Cells_on_level Protection_flag:\n");
                        if (scanf("%d %d %d", &rec2.level_number, &rec2.cells_on_level,
                                  &rec2.protection_flag) == 3 &&
                            rec2.cells_on_level > 0 && rec2.protection_flag >= 0 &&
                            rec2.protection_flag <= 1 && rec2.cells_on_level >= 0)
                            INSERT(db_lev, LEVELS, (void *)(&rec2));
                        build_levels_index(db_lev);
                    }
                    break;
                case 3:
                    printf("\nEnter Event_ID to update: ");
                    int event_id;
                    if (scanf("%d", &event_id) == 1 && event_id >= 0) {
                        MasterStatusEventsRecord rec3 = {-1, -1, -1, "", ""};
                        printf(
                            "\nEnter new record. Format: \nEvent_id Module_id New_module_status "
                            "Status_change_date "
                            "Status_change_time:\n");
                        if (scanf("%d %d %d %10s %8s", &rec3.event_id, &rec3.module_id,
                                  &rec3.new_module_status, rec3.status_change_date,
                                  rec3.status_change_time) == 5)
                            INSERT(db_stat, STATUS_EVENTS, (void *)(&rec3));
                        build_status_events_index(db_stat);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
void sub_menu_delete(FILE *db_mod, FILE *db_lev, FILE *db_stat) {
    int sub = 0;
    while (sub != -1) {
        print_dbselect_menu();
        if (scanf("%d", &sub) == 1 && sub > -1 && sub <= 3) {
            switch (sub) {
                case 1:
                    printf("\nEnter Module_ID to delete: ");
                    int module_id;
                    if (scanf("%d", &module_id) == 1 && module_id >= 0) {
                        DELETE(db_mod, MODULES, module_id);
                        build_modules_index(db_mod);
                        break;
                        case 2:
                            printf("\nEnter Level_ID to delete: ");
                            int level_id;
                            if (scanf("%d", &level_id) == 1 && level_id > 0) DELETE(db_lev, LEVELS, level_id);
                            build_levels_index(db_lev);
                            break;
                        case 3:
                            printf("\nEnter Event_ID to delete: ");
                            int event_id;
                            if (scanf("%d", &event_id) == 1 && event_id > 0)
                                DELETE(db_stat, STATUS_EVENTS, event_id);
                            build_status_events_index(db_stat);
                            break;
                        default:
                            break;
                    }
            }
        }
    }
}