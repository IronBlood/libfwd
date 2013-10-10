CC=gcc
FLAGS=-O2 -Wall -ggdb -D_GNU_SOURCE
LDFLAGS=-lfcgi -lghthash -ljson-c -lfwd -pthread

PROGNAME=multiple-threaded.fcgi
CFILE=main.c threadpool.c
COBJS=$(CFILE:.c=.o)

.c.o:
	$(CC) $(FLAGS) -c $*.c
all: $(PROGNAME)

$(PROGNAME): $(COBJS)
	$(CC) --static -o $@ $^ $(LDFLAGS)

clean:
	rm $(PROGNAME) $(COBJS)
