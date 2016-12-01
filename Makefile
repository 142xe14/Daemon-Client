CFLAGS = -std=c11 -Wpedantic -Wall -Wextra -Werror -D_XOPEN_SOURCE=600
LDLIBS = -pthread -lrt

all : Daemon Client

Daemon : Daemon.o

Client : Client.o

Daemon.o : Daemon.c

Client.o : Client.c

clean :
		$(RM) *.o Daemon Client