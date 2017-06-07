CC	= gcc
CFLAGS	= -Wall -g -I/usr/include/ncurses
LDFLAGS	= -lncurses -pthread
PROGS	= othello

all: $(PROGS)

othello: main.o othello.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f *.o *~ $(PROGS)
