default: all

all: server client

server: echo_server.c
	gcc -o echo_server echo_server.c

client: echo_client.c
	gcc -o echo_client echo_client.c

clean:
	rm echo_server echo_client