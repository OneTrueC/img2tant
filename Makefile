.POSIX:
.PHONY: clean install uninstall

VERSION = 0.2

PREFIX = /usr/local
BINDIR = ${PREFIX}/bin
MANDIR = ${PREFIX}/share/man/man1

INC = -I. -I/usr/include
LIB = `pkg-config imlib2 --libs || echo -lImlib2`
DEF = -DVERSION=\"${VERSION}\"

CFLAGS = -Wall -Wextra -std=c99 -pedantic ${INC} -Os ${DEF}
LDFLAGS = ${LIB} -s

all: img2tant

img2tant: main.c
	${CC} ${CFLAGS} ${LDFLAGS} main.c -o $@

clean:
	rm -f img2tant

install: img2tant
	cp img2tant ${BINDIR}/img2tant
	sed img2tant.1 -e "s/VERSION/v${VERSION}/" > "${MANDIR}/img2tant.1"

uninstall:
	rm -f ${BINDIR}/img2tant
	rm -f ${MANDIR}/img2tant.1
