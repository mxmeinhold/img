CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

H_FILES = img.h
_O_FILES = img.o main.o
_LIB_O_FILES = img.o

BUILD_DIR = target

LIB_NAME = libimg
LIB_DIR = $(BUILD_DIR)/lib

ANALYSIS_DIR = $(BUILD_DIR)/analysis

O_DIR = $(BUILD_DIR)/obj
O_FILES = $(patsubst %,$(O_DIR)/%,$(_O_FILES))
LIB_O_FILES = $(patsubst %,$(O_DIR)/%,$(_O_FILES))

EXEC = print_colors

LIBRARIES = -lpng

$(O_DIR)/%.o: %.c $(H_FILES)
	@mkdir -p $(@D)
	$(CC) -fPIC -c -o $@ $< $(CFLAGS)

main: $(O_FILES)
	$(CC) -o $(EXEC) $(O_FILES) $(CFLAGS) $(LIBRARIES)

run: main
	./$(EXEC)

lib: $(LIB_O_FILES)
	@mkdir -p $(LIB_DIR)
	ar rcs $(LIB_DIR)/$(LIB_NAME).a $(LIB_O_FILES)
	gcc -shared -o $(LIB_DIR)/$(LIB_NAME).so $(LIB_O_FILES)
	@echo "Using sudo to install library files to /usr/lib/ and call ldconfig"
	sudo cp $(LIB_DIR)/$(LIB_NAME).a $(LIB_DIR)/$(LIB_NAME).so /usr/lib/
	sudo cp $(H_FILES) /usr/include/
	sudo ldconfig

analyze: main
	@command -v valgrind >/dev/null 2>&1 || { echo >&2 "valgrind not found, aborting analyze"; exit 1; }
	@mkdir -p $(ANALYSIS_DIR)
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file="$(ANALYSIS_DIR)/memcheck.out" ./$(EXEC)
	@cat $(ANALYSIS_DIR)/memcheck.out
	valgrind --tool=callgrind --callgrind-out-file="$(ANALYSIS_DIR)/callgrind.out" ./$(EXEC)
	gprof2dot -f callgrind $(ANALYSIS_DIR)/callgrind.out --root=main | dot -Tpng -o $(ANALYSIS_DIR)/callgrind.png


watch:
	@command -v inotifywait >/dev/null 2>&1 || { echo >&2 "inotifywait not found, aborting watch"; exit 1; }
	while true; do \
		clear; \
		make run; \
		inotifywait -qre modify .; \
	done


.PHONY: clean
clean:
	rm -f $(EXEC)
	rm -rf $(BUILD_DIR)
