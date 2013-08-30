CC=gcc
FLAGS=-O2 -Wall -ggdb -D_GNU_SOURCE
LDFLAGS=-lfcgi -lghthash -ljson-c -lfwd

PROGNAME=helloworld.fcgi
CFILE=main.c
COBJS=$(CFILE:.c=.o)

.c.o:
	$(CC) $(FLAGS) -c $*.c
all: $(PROGNAME)

$(PROGNAME): $(COBJS)
	$(CC) --static -o $@ $^ $(LDFLAGS)

clean:
	rm $(PROGNAME) $(COBJS)
