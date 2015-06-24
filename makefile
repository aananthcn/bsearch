# Author: Aananth C N
# Date: 23 June 2015
# License: GPLv2
# Email: caananth@visteon
#

TARGET = bsearch

FILES = bsearch.c commands.c

CFLAGS = -g

LFLAGS = -lncurses

all:
	gcc ${FILES} ${CFLAGS} -o ${TARGET} ${LFLAGS}
