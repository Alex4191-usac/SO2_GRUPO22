#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

// Global variables for counting system calls
int total_syscalls = 0;
int open_syscalls = 0;
int read_syscalls = 0;
int write_syscalls = 0;
pid_t pid1, pid2;

// Function to print the summary of system calls
void print_syscall_summary() {
    printf("\n - - Total number of system calls made by child processes: %d\n", total_syscalls);
    printf("Number of system calls by type:\n");
    printf("  Read: %d\n", read_syscalls);
    printf("  Write: %d\n", write_syscalls);
}

int analyze_syscalls_file()
{

    // Abrir el archivo syscall.log
    FILE *syscalls_log = fopen("syscalls.log", "r");
    if (syscalls_log == NULL)
    {
        perror("\nError al abrir el archivo de log de llamadas al sistema");
        exit(1);
    }

    printf("\nAnalizando reporte de llamadas al sistema.");
    // posiscionando puntero
    fseek(syscalls_log, 0, SEEK_SET);
    char line[100];
    while (fgets(line, sizeof(line), syscalls_log) != NULL)
    {
        if (strstr(line, "READ"))
        {
            read_syscalls++;
        }
        else if (strstr(line, "WRITE"))
        {
            write_syscalls++;
        }
        else if (strstr(line, "OPEN"))
        {
            open_syscalls++;
        }
        total_syscalls++;
    }

    printf("\nAnalisis del reporte finalizado");
    // Cerrar el archivo de log
    fclose(syscalls_log);
    return 0;
}

void clean_file(char filename[])
{
    int closed;
    char message[200];

    closed = fclose(fopen(filename, "w"));
    if (closed != 0)
    {
        sprintf(message, "\nError al abrir el archivo %s", filename);
        perror(message);
        exit(1);
    }
}

// Signal handler for SIGINT (Ctrl + C)
void sigint_handler(int signum) {
    kill(pid1, SIGTERM);
    kill(pid2, SIGTERM);
    exit(0);
}

int main() {
    // Setup the signal handler for SIGINT using sigaction
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Error setting up signal handler");
        exit(1);
    }

    // Imprimiendo PID del proceso padre
    int parent_pid = getpid();
    printf("\nEjecutando proceso Padre con PID: %d", parent_pid);

    // Limpiando / creando archivos
    clean_file("syscalls.log");
    clean_file("practica1.txt");

    // Create child processes
    pid1 = fork();
    if (pid1 == 0) {
        // Code for the first child
        execl("./child", "child", NULL);
        perror("Error executing child process 1");
        exit(1);
    } else if (pid1 < 0) {
        perror("Error creating child process 1");
        exit(1);
    }
    printf("\n-> Proceso hijo 1 con PID: %d", pid1);

    pid2 = fork();
    if (pid2 == 0) {
        // Code for the second child
        execl("./child", "child", NULL);
        perror("Error executing child process 2");
        exit(1);
    } else if (pid2 < 0) {
        perror("Error creating child process 2");
        exit(1);
    }
    printf("\n-> Proceso hijo 2 con PID: %d\n", pid2);

    // Ejecutar SystemTap dentro del proceso padre
    printf("\n >> Ejecutando SystemTap\n");
    fflush(stdout);

    // Execute SystemTap within the parent process
    char command[200];
    sprintf(command, "sudo stap trace.stp %d %d > syscalls.log", pid1, pid2);
    system(command);

    printf("\n >> Saliendo del SystemTap \n");

    // Print the counts
    print_syscall_summary();

    // Wait for the child processes to finish
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
