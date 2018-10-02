#EXECS = retrogame gamera
EXECS = retrogame
CC    = gcc $(CFLAGS) -Wall -Ofast -fomit-frame-pointer -funroll-loops -s -I. -L. -lads1015
LDFLAGS=

all: $(EXECS)

retrogame: retrogame.c keyTable.h
	$(CC) $< -o $@ $^ $(LDFLAGS)
	strip $@

KEYFILE = /usr/include/linux/input.h
keyTable.h: keyTableGen.sh $(KEYFILE)
	sh $^ >$@

gamera: gamera.c
	$(CC) $< -lncurses -lmenu -lexpat -o $@
	strip $@

install:
	mv $(EXECS) /usr/local/bin

clean:
	rm -f $(EXECS) keyTable.h
