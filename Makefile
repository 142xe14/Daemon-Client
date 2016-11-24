CFLAGS = -std=c11 -Wpedantic -Wall -Wextra -Werror -D_XOPEN_SOURCE=600

all : Daemon Client

Daemon : Daemon.o
		$(CC) $(CFLAGS) Daemon.o -o Daemon

Client : Client.o
		$(CC) $(CFLAGS) Client.o -o Client

Daemon.o : Daemon.c
		$(CC) $(CFLAGS) -c Daemon.c -o Daemon.o

Client.o : Client.c
		$(CC) $(CFLAGS) -c Client.c -o Client.o

clean :
		$(RM) *.o Daemon Client