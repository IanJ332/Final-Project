#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Structure to pass arguments to the thread
typedef struct {
    int *sequence;
    int length;
} ThreadArgs;

// Function to generate Fibonacci sequence
void *generateFibonacci(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int length = args->length;
    int *sequence = args->sequence;

    if (length > 0) {
        sequence[0] = 0;
    }
    if (length > 1) {
        sequence[1] = 1;
    }
    for (int i = 2; i < length; i++) {
        sequence[i] = sequence[i - 1] + sequence[i - 2];
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        return 1;
    }

    int length = atoi(argv[1]);
    if (length <= 0) {
        fprintf(stderr, "Please enter a positive integer.\n");
        return 1;
    }

    int *sequence = (int *)malloc(length * sizeof(int));
    if (!sequence) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    pthread_t thread;
    ThreadArgs args = { sequence, length };

    if (pthread_create(&thread, NULL, generateFibonacci, &args)) {
        fprintf(stderr, "Thread creation failed.\n");
        return 1;
    }

    if (pthread_join(thread, NULL)) {
        fprintf(stderr, "Thread join failed.\n");
        return 1;
    }

    printf("Generated Fibonacci Sequence:\n");
    for (int i = 0; i < length; i++) {
        printf("%d ", sequence[i]);
    }
    printf("\n");

    free(sequence);

    return 0;
}

