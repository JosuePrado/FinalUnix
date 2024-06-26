#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

void getInfo()
{
    printf("Mike's service, v0.1");
}

void getNumberOfPartitions()
{
    const char *kernel_command = "lsblk -l | grep part";

    int result = system(kernel_command);
    if (result == -1)
    {
        perror("Error to excecute the command");
    }
}

void getCurrentKernelVersion()
{
    const char *kernel_command = "uname -r";
    int result = system(kernel_command);

    if (result == -1)
    {
        perror("Error to excecute the command");
    }
}

bool sshdRuning()
{
    return true;
}

int main(int argc, char **argv)
{
    FILE *fp;
    char path[1035];

    // Ejecutar el comando "ls -la" y abrir un flujo de lectura
    fp = popen("ps ax | grep sshd", "r");
    if (fp == NULL)
    {
        perror("Error al ejecutar el comando");
        return EXIT_FAILURE;
    }

    // Leer la salida del comando línea por línea
    while (fgets(path, sizeof(path), fp) != NULL)
    {
        printf("%s", path);
    }

    // Cerrar el flujo
    if (pclose(fp) == -1)
    {
        perror("Error al cerrar el flujo");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
    // getNumberOfPartitions();
    // getCurrentKernelVersion();
    // return 0;
}
