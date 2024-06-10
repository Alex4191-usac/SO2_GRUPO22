#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

void sigterm_handler(int signum)
{
    exit(0);
}

int main()
{

    sleep(15);
    signal(SIGTERM, sigterm_handler);

    srand(time(NULL));
    int file = open("practica1.txt", O_RDWR | O_CREAT, 0777);
    if (file < 0)
    {
        perror("Error al abrir el archivo");
        exit(1);
    }

    while (1)
    {
        int random = rand() % 2;
        char buffer[9];

        if (random == 0)
        {
            // Write
            for (int i = 0; i < 8; i++)
            {
                buffer[i] = 'A' + rand() % 26;
            }
            buffer[8] = '\n';
            write(file, buffer, 9);
        }
        else
        {
            // Read
            read(file, buffer, 9);
        }
        sleep(1 + rand() % 3);
    }
    close(file);
    return 0;
}