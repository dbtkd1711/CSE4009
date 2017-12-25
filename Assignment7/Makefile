all : server client1 client2 client3

client1 : client1.c
	gcc -o client1 client1.c -pthread
client2 : client2.c
	gcc -o client2 client2.c -pthread
client3 : client3.c
	gcc -o client3 client3.c -pthread
server : server.c
	gcc -o server server.c

clean :
	rm server client1 client2 client3

