#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>

#define MAX_CONNECTION 50
#define MESSAGE_SIZE 256

int server_socket_fd;

char *getInfo()
{
    return "Mike's service, v0.1";
}

char *getNumberOfPartitions()
{
    FILE *fp;
    char path[MESSAGE_SIZE];
    static char result[MESSAGE_SIZE];

    fp = popen("lsblk -l | grep part", "r");
    if (fp == NULL)
    {
        perror("Error to execute the command");
        exit(EXIT_FAILURE);
    }

    result[0] = '\0';
    while (fgets(path, sizeof(path), fp) != NULL)
    {
        strcat(result, path);
    }

    if (pclose(fp) == -1)
    {
        perror("Error to close the command");
        exit(EXIT_FAILURE);
    }

    return result;
}

char *getCurrentKernelVersion()
{
    FILE *fp;
    char path[MESSAGE_SIZE];
    static char result[MESSAGE_SIZE];

    fp = popen("uname -r", "r");
    if (fp == NULL)
    {
        perror("Error to execute the command");
        exit(EXIT_FAILURE);
    }

    if (fgets(result, sizeof(result), fp) == NULL)
    {
        perror("Error to read the command output");
        exit(EXIT_FAILURE);
    }

    if (pclose(fp) == -1)
    {
        perror("Error to close the command");
        exit(EXIT_FAILURE);
    }

    return result;
}

char *sshdRunning()
{
    FILE *fp;
    char path[1035];
    char result[100];
    bool isListener = false;

    fp = popen("ps ax | grep [s]shd", "r");
    if (fp == NULL)
    {
        perror("Error to execute the command");
        exit(EXIT_FAILURE);
    }

    while (fgets(path, sizeof(path), fp) != NULL)
    {
        if (strstr(path, "sshd"))
        {
            isListener = true;
            break;
        }
    }

    if (pclose(fp) == -1)
    {
        errorHandling("Error to close the command");
    }

    if (isListener)
    {
        strcpy(result, "true");
    }
    else
    {
        strcpy(result, "false");
    }

    return result;
}

void errorHandling(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void clientManager(int client_socket_fd)
{
    char buffer[MESSAGE_SIZE];
    int read_status = read(client_socket_fd, buffer, MESSAGE_SIZE - 1);
    if (read_status > 0)
    {
        buffer[read_status] = '\0';
        if (strcmp(buffer, "getInfo") == 0)
        {
            write(client_socket_fd, getInfo(), strlen(getInfo()));
        }
        else if (strcmp(buffer, "getNumberOfPartitions") == 0)
        {
            write(client_socket_fd, getNumberOfPartitions(), strlen(getNumberOfPartitions()));
        }
        else if (strcmp(buffer, "getCurrentKernelVersion") == 0)
        {
            write(client_socket_fd, getCurrentKernelVersion(), strlen(getCurrentKernelVersion()));
        }
        else if (strcmp(buffer, "sshdRunning") == 0)
        {
            write(client_socket_fd, sshdRunning(), strlen(sshdRunning()));
        }
        else
        {
            write(client_socket_fd, "Unknown command", 15);
        }
    }
    close(client_socket_fd);
}

void *threadMain(void *arg)
{
    int client_socket_fd = *((int *)arg);
    free(arg);
    clientManager(client_socket_fd);
    return NULL;
}

void sigint_handler(int sig)
{
    close(server_socket_fd);
    printf("\nServer shut down gracefully.\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;
    int *client_socket_fd;
    pthread_t t_id;

    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == -1)
    {
        errorHandling("Error at socket init.");
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(9595);

    if (bind(server_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        errorHandling("Error at bind");

    if (listen(server_socket_fd, MAX_CONNECTION) == -1)
        errorHandling("Error at listening");

    signal(SIGINT, sigint_handler);

    while (1)
    {
        client_addr_size = sizeof(client_addr);
        client_socket_fd = malloc(sizeof(int));
        *client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_addr, &client_addr_size);
        if (*client_socket_fd == -1)
        {
            free(client_socket_fd);
            perror("Error at accept");
            continue;
        }
        pthread_create(&t_id, NULL, threadMain, (void *)client_socket_fd);
        pthread_detach(t_id);
    }

    close(server_socket_fd);
    return 0;
}
