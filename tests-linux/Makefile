#
# Copyright (C) 2004 Droids Corporation
# http://www.droids-corp.org/
#
# important parts of this code were taken from the Linux source code,
# Copyright (C) 1996 Linus Torvalds (file linux/mm/remap.c). See
# http://www.kernel.org/
#
# $Id: Makefile,v 1.2 2004-06-21 17:49:35 lalet Exp $
#

CC = gcc
CFLAGS = -D MODVERSIONS -I /usr/src/linux/include -c -Wall
MODULES = find_fb.o

main: $(MODULES)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~
