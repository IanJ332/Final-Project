#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define TEST_COUNT 10

int global_variable = 0;
pthread_mutex_t mutex;

void *producer(void *param);
void *consumer(void *param);

int main() {
    pthread_t producerThread, consumerThread;

    // Initialize the mutex
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);
    }

    // Create the producer thread
    if (pthread_create(&producerThread, NULL, producer, NULL) != 0) {
        perror("pthread_create: producer");
        exit(EXIT_FAILURE);
    }

    // Create the consumer thread
    if (pthread_create(&consumerThread, NULL, consumer, NULL) != 0) {
        perror("pthread_create: consumer");
        exit(EXIT_FAILURE);
    }

    // Wait for the threads to finish
    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    // Destroy the mutex
    if (pthread_mutex_destroy(&mutex) != 0) {
        perror("pthread_mutex_destroy");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void *producer(void *param) {
    while (1) {
        // Lock the mutex
        pthread_mutex_lock(&mutex);

        // Production process
        global_variable++;
        printf("Producer: global_variable = %d\n", global_variable);

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);

        // Sleep
        sleep(1);
    }

    return NULL;
}

void *consumer(void *param) {
    while (1) {
        // Lock the mutex
        pthread_mutex_lock(&mutex);

        // Consumption process
        global_variable--;
        printf("Consumer: global_variable = %d\n", global_variable);

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);

        // Sleep
        sleep(1);
    }

    return NULL;
}

