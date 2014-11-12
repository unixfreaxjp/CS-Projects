default: all

all: server client

server: lserver.c
	gcc -o lserver lserver.c

client: lclient.c
	gcc -o lclient lclient.c

clean:
	rm lclient lserver