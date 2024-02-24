BUILD_PATH=../bin/
CC=gcc
CFLAGS=-g -std=gnu99 -Wall -Wextra -Werror 
EXTRAFLAGS=-fsanitize=leak -fsanitize=address -fsanitize=undefined -fsanitize=unreachable -fno-sanitize-recover -fstack-protector 

all: check rebuild

build: clean

rebuild: clean_all build_db build_db_extra clean

build_db: clean modules_db.c shared.c master_levels.c master_modules.c master_status_events.c index.c
	$(CC) $(CFLAGS) modules_db.c shared.c master_levels.c master_modules.c master_status_events.c index.c -o  $(BUILD_PATH)dbms

build_db_extra: clean modules_db.c shared.c master_levels.c master_modules.c master_status_events.c index.c
	$(CC) $(CFLAGS) $(EXTRAFLAGS) modules_db.c shared.c master_levels.c master_modules.c master_status_events.c index.c -o $(BUILD_PATH)dbms_extra


clean_all:
	rm -rf *.o *.a *.so $(BUILD_PATH)*
	
clean:
	rm -rf *.o *.a

check:
	clang-format -i *.c
	clang-format -i *.h
	clang-format -n *.c
	clang-format -n *.h
	cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

run:
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes $(BUILD_PATH)dbms

run_no_valgrind:
	$(BUILD_PATH)Qdbms_extra


