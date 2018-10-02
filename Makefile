#EXECS = retrogame gamera
EXECS = retrogame
CC    = gcc $(CFLAGS) -static -Wall -Ofast -fomit-frame-pointer -funroll-loops -s
LDFLAGS=-I./Adafruit_ADS1X15_Linux -L./Adafruit_ADS1X15_Linux -lads1015

all: $(EXECS)

retrogame: retrogame.c keyTable.h
	$(CC) $< $(LDFLAGS) -o $@
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
