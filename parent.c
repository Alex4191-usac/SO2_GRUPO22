#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

// Global variables for counting system calls
int total_syscalls = 0;
int read_syscalls = 0;
int write_syscalls = 0;
int seek_syscalls = 0;
pid_t pid1, pid2;

// Function to print the summary of system calls
void print_syscall_summary() {
    printf("\n - - Total number of system calls made by child processes: %d\n", total_syscalls);
    printf("Number of system calls by type:\n");
    printf("  Read: %d\n", read_syscalls);
    printf("  Write: %d\n", write_syscalls);
    printf("  Seek: %d\n", seek_syscalls);
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

    // Create log file
    FILE *logfile = fopen("syscalls.log", "w");
    if (logfile == NULL) {
        perror("Error opening log file");
        exit(1);
    }
    fclose(logfile);

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

    // Execute SystemTap within the parent process
    char command[200];
    sprintf(command, "sudo stap trace.stp %d %d > syscalls.log", pid1, pid2);
    system(command);

    printf("Exited SystemTap\n");

    // Analyze the SystemTap log file to count system calls
    FILE *syscalls_log = fopen("syscalls.log", "r");
    if (syscalls_log == NULL) {
        perror("Error opening system calls log file");
        exit(1);
    }

    char line[100];
    while (fgets(line, sizeof(line), syscalls_log) != NULL) {
        if (strstr(line, "READ")) {
            read_syscalls++;
        } else if (strstr(line, "WRITE")) {
            write_syscalls++;
        } else if (strstr(line, "SEEK")) {
            seek_syscalls++;
        }
        total_syscalls++;
    }

    // Close the log file
    fclose(syscalls_log);

    // Print the counts
    print_syscall_summary();

    // Wait for the child processes to finish
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}