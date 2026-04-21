CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -O2

TARGET  = comprimidos
OBJS    = main.o problem.o
ROOT_DIR := $(notdir $(CURDIR))
PACKAGE  = lsmr24.tar.gz

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
	tar -C $(CURDIR) --exclude-vcs \
	    --exclude='$(PACKAGE)' \
	    -czf /tmp/$(PACKAGE) .
	mv /tmp/$(PACKAGE) $(PACKAGE)
