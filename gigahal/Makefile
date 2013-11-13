
# Library
LIB_SOURCES=$(wildcard src/*.c)
LIB_INCLUDES=$(wildcard include/*.h) $(wildcard src/*.h)
LIB_OUT=bin/libgigahal.so

# Tests
TEST_SOURCES=$(wildcard test/*.c)

#=============================================================================#

WARNINGS=-Wall -Wextra -Wshadow -pedantic -Werror \
         -Wstrict-prototypes -Wmissing-prototypes \
         -Wno-unused-parameter -Wno-unused-function \
         -Wno-unused-variable

CC=gcc
CFLAGS=$(WARNINGS) -std=c99 -Isrc -march=corei7 -fPIC -m32
DEBUG_CFLAGS=-g -DGH_DEBUG
NONDEBUG_CFLAGS=-O3

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS:=$(CFLAGS) $(DEBUG_CFLAGS)
else
	CFLAGS:=$(CFLAGS) $(NONDEBUG_CFLAGS)
endif

LIB_OBJECTS=$(LIB_SOURCES:%.c=out/%.o)
TEST_OBJECTS=$(patsubst test/%.c,bin/%,$(TEST_SOURCES))

#=============================================================================#

all: $(LIB_OUT) $(TEST_OBJECTS)

$(LIB_OUT): $(LIB_OBJECTS)
	mkdir -p bin
	$(CC) -o $@ $(LIB_OBJECTS) -shared -m32

$(LIB_OBJECTS): out/%.o: %.c $(LIB_INCLUDES)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJECTS): bin/%: test/%.c $(LIB_OUT)
	$(CC) $(CFLAGS) -Lbin -lgigahal $< -o $@

clean:
	rm -rf bin
	rm -rf out

production: $(LIB_OUT)
	make DEBUG=0
	cp -f bin/libgigahal.so pygigahal/gigahal

.PHONY: clean docs
