INC = -I. -I/usr/include
LIB = -lImlib2

CFLAGS = -Wall -Wextra -std=c99 -pedantic $(INC) -Os
LDFLAGS = $(LIB) -s

all: imgSex

imgSex: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) main.c -o $@

install:
	install -m 755 imgSex /usr/local/bin/imgSex

uninstall:
	rm -f /usr/local/bin/imgSex
