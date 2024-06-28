build
gcc -o client SocketsClient.c
gcc -o server SocketsServer.c -pthread

Run 
 ./server 9595
 ./client 127.0.0.1 9696
