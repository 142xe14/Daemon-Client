CFLAGS = -std=c11 -Wpedantic -Wall -Wextra -Werror -D_XOPEN_SOURCE=600

all : Daemon

Daemon : Daemon.o
		$(CC) $(CFLAGS) Daemon.o -o Daemon

Daemon.o : Daemon.c
		$(CC) $(CFLAGS) -c Daemon.c -o Daemon.o

clean :
		$(RM) *.o Daemon