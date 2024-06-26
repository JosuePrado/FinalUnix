#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MESSAGE_SIZE 256

void error_handling(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[MESSAGE_SIZE];
    int str_len;

    if (argc != 3)
    {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    strcpy(message, "getInfo");
    write(sock, message, strlen(message));

    str_len = read(sock, message, MESSAGE_SIZE - 1);
    if (str_len == -1)
        error_handling("read() error");

    message[str_len] = '\0';
    printf("Message from server: \n%s \n", message);

    close(sock);
    return 0;
}
