CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -O2
LDFLAGS =

TARGET  = comprimidos
OBJS    = main.o problem.o
ROOT_DIR := $(notdir $(CURDIR))
PACKAGE  = $(ROOT_DIR).tar.gz

.PHONY: all clean dist

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

main.o: main.c problem.h
	$(CC) $(CFLAGS) -c main.c -o $@

problem.o: problem.c problem.h
	$(CC) $(CFLAGS) -c problem.c -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(PACKAGE)

dist: clean
	tar -C .. --exclude-vcs \
	    --exclude='$(ROOT_DIR)/$(PACKAGE)' \
	    -czf $(PACKAGE) $(ROOT_DIR)

