CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -O2

TARGET  = comprimidos
OBJS    = main.o problem.o
ROOT_DIR := $(notdir $(CURDIR))
DIST_DIR = lmsr24
PACKAGE  = $(DIST_DIR).tar.gz

.PHONY: all clean dist

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

main.o: main.c problem.h
	$(CC) $(CFLAGS) -c main.c -o $@

problem.o: problem.c problem.h
	$(CC) $(CFLAGS) -c problem.c -o $@

clean:
	rm -f $(OBJS) $(TARGET)

dist: clean
	tar -C $(CURDIR)/.. --exclude-vcs \
	    --exclude='$(ROOT_DIR)/$(PACKAGE)' \
	    --transform='s,^$(ROOT_DIR),$(DIST_DIR),' \
	    -czf /tmp/$(PACKAGE) $(ROOT_DIR)
	mv /tmp/$(PACKAGE) $(PACKAGE)
