PROJECT = mp_client_nonet
CC = gcc
CFLAGS = -std=c18 -Wall -Isrc -D_GNU_SOURCE
LDFLAGS = -lncurses

RM = rm -f
MKDIR = mkdir -p
RMDIR = rm -rf

SRCS = $(shell find src -name '*.c' | grep -P '.*\.c$$')
OBJS = $(patsubst src/%.c,bin/intermed/%.o,$(SRCS))
DEPS = $(patsubst src/%.c,bin/intermed/%.d,$(SRCS))

.PHONY: all clean run

all: $(PROJECT)

run: all
	./$(PROJECT)

clean:
	$(RMDIR) bin
	$(MKDIR) bin/intermed/kbhit

$(PROJECT): $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

-include $(DEPS)

bin/intermed/%.o: src/%.c Makefile
	$(CC) -MMD -MP -c $< -o $@ $(CFLAGS) $(LDFLAGS)
