VERSION = 0.1

PREFIX = /usr/local
BINDIR = ${PREFIX}/bin

INC = -I. -I/usr/include
LIB = -lImlib2
DEF = -DVERSION=\"${VERSION}\"

CFLAGS = -Wall -Wextra -std=c99 -pedantic ${INC} -Os ${DEF}
LDFLAGS = ${LIB} -s

all: img2sext

img2sext: main.c
	${CC} ${CFLAGS} ${LDFLAGS} main.c -o $@

install: img2sext
	cp img2sext ${BINDIR}/img2sext

uninstall:
	rm -f ${BINDIR}/img2sext
