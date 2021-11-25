SHELL := bash
.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables --no-builtin-rules

CC = gcc
CFLAGS += -std=c99
LDFLAGS ?=

# If DEBUG is set, compile with gdb flags
ifdef DEBUG
CFLAGS += -g -ggdb
endif

ifdef STATIC
CFLAGS += -static
endif

# Warnings
WARNINGS = -Wall -Wextra -Wpedantic -Wconversion -Wformat=2 \
	-Wformat-nonliteral -Winit-self -Wmissing-include-dirs -Wnested-externs \
	-Wno-unused-parameter -Wold-style-definition -Wredundant-decls -Wshadow \
	-Wstrict-prototypes -Wwrite-strings

# GCC warnings that Clang doesn't provide:
ifeq ($(CC),gcc)
		WARNINGS += -Wjump-misses-init -Wlogical-op
endif

H_FILES = img.h
_O_FILES = png.o test.o
_LIB_O_FILES = png.o

BUILD_DIR = target

LIB_NAME = libimg
LIB_DIR = $(BUILD_DIR)/lib

ANALYSIS_DIR = $(BUILD_DIR)/analysis

O_DIR = $(BUILD_DIR)/obj
O_FILES = $(patsubst %,$(O_DIR)/%,$(_O_FILES))
LIB_O_FILES = $(patsubst %,$(O_DIR)/%,$(_LIB_O_FILES))

EXEC = run_tests

LIBRARIES = -lpng16 -lz -lm

.PHONY: run
run: $(EXEC)
	./$(EXEC)


$(O_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -fPIC -c -o $@ $< $(CFLAGS)

$(EXEC): $(O_FILES)
	@mkdir -p $(@D)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBRARIES)

colors.png read_write.png: $(EXEC)
	./$(EXEC)

.PHONY: lib
lib: $(LIB_DIR)/$(LIB_NAME).a $(LIB_DIR)/$(LIB_NAME).so

$(LIB_DIR)/%.a: $(LIB_O_FILES)
	@mkdir -p $(@D)
	ar rcs $@ $^

$(LIB_DIR)/%.so: $(LIB_O_FILES)
	@mkdir -p $(@D)
	gcc -shared -o $@ $^

.PHONY: tar
tar: img-$(shell cat VERSION).tar.gz img-$(shell cat VERSION).tar.gz.sig

img-%.tar.gz: Makefile $(_O_FILES:%.o=%.c) $(H_FILES) LICENSE.txt
	tar -czvf $@ $^

%.sig: %
	gpg --output $@ --detach-sig $^

.PHONY: install
install: lib
	install $(LIB_DIR)/$(LIB_NAME).a $(LIB_DIR)/$(LIB_NAME).so $(DESTDIR)/usr/lib/
	install $(H_FILES) $(DESTDIR)/usr/include/

ANALYSIS_DIR = $(BUILD_DIR)/analysis
VALGRIND_DEP = $(BUILD_DIR)/.valgrind
$(VALGRIND_DEP):
	@command -v valgrind >/dev/null 2>&1 || { echo >&2 "valgrind not found, aborting analyze"; exit 1; }
	@touch $@

$(ANALYSIS_DIR)/memcheck.out: $(EXEC) $(VALGRIND_DEP)
	@mkdir -p $(@D)
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file="$@" ./$<

$(ANALYSIS_DIR)/callgrind.out: $(EXEC) $(VALGRIND_DEP)
	@mkdir -p $(@D)
	valgrind --tool=callgrind --callgrind-out-file="$@" ./$<

%callgrind.dot: %callgrind.out
	gprof2dot -f callgrind --root=main -o $@ $<

%.png: %.dot
	dot -Tpng -o $@ $<

.PHONY: analyze
analyze: $(ANALYSIS_DIR)/memcheck.out $(ANALYSIS_DIR)/callgrind.out $(ANALYSIS_DIR)/callgrind.png

INOTIFY_DEP = $(BUILD_DIR)/.inotify
$(INOTIFY_DEP):
	@command -v inotifywait >/dev/null 2>&1 || { echo >&2 "inotifywait not found, aborting watch"; exit 1; }
	@touch $@

.PHONY: watch
watch: run $(INOTIFY_DEP)
	@command -v inotifywait >/dev/null 2>&1 || { echo >&2 "inotifywait not found, aborting watch"; exit 1; }
	feh colors.png &
	while true; do \
		clear; \
		make run; \
		inotifywait -qre modify .; \
	done

.PHONY: clean
clean:
	rm -f $(EXEC)
	rm -rf $(BUILD_DIR)
