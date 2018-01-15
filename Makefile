CC = gcc
CFLAGS = -lpthread -lm -lwiringPi

.c.o:
	$(CC) $(CFLAGS) -c $< 

all: exec

exec: server2.o $(CFLAGS)
	$(CC) -o exec server2.o $(CFLAGS)

clean:
	rm -f exec *.o