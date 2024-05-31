#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 100

int main() {
    int pipefd[2];
    int reverse_pipefd[2];
    char message[BUFFER_SIZE];
    pid_t pid;

    if (pipe(pipefd) == -1 || pipe(reverse_pipefd) == -1) {
        perror("Something are wrong with pipe");
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        perror("Something are wrong with fork");
        return 1;
    }

    if (pid > 0) { // Parent process
        close(pipefd[0]); // Close reading end of the first pipe
        close(reverse_pipefd[1]); // Close writing end of the second pipe

        printf("Enter a message: ");
        fgets(message, BUFFER_SIZE, stdin);

        // Write the message to the first pipe
        write(pipefd[1], message, strlen(message) + 1);

        // Read the modified message from the second pipe
        read(reverse_pipefd[0], message, BUFFER_SIZE);

        printf("Reversed case message: %s\n", message);

        close(pipefd[1]);
        close(reverse_pipefd[0]);
    } else { // Child process
        close(pipefd[1]); // Close writing end of the first pipe
        close(reverse_pipefd[0]); // Close reading end of the second pipe

        // Read the message from the first pipe
        read(pipefd[0], message, BUFFER_SIZE);

        // Reverse
        for (int i = 0; message[i] != '\0'; i++) {
            if (islower(message[i])) {
                message[i] = toupper(message[i]);
            } else if (isupper(message[i])) {
                message[i] = tolower(message[i]);
            }
        }

        write(reverse_pipefd[1], message, strlen(message) + 1);

        close(pipefd[0]);
        close(reverse_pipefd[1]);
    }

    return 0;
}

