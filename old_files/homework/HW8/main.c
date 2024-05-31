#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "buffer.h"

#define RAND_DIVISOR 100000000
#define TRUE 1

pthread_mutex_t mutex;
pthread_cond_t full, empty;

void *producer(void *param) {
    buffer_item item;
    while (TRUE) {
        usleep(rand() / RAND_DIVISOR);
        item = rand();
        pthread_mutex_lock(&mutex);
        if (insert_item(item))
            fprintf(stderr, "Error in inserting item\n");
        else
            printf("Producer produced %d\n", item);
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
    }
}

void *consumer(void *param) {
    buffer_item item;
    while (TRUE) {
        usleep(rand() / RAND_DIVISOR);
        pthread_mutex_lock(&mutex);
        while (in == out)
            pthread_cond_wait(&full, &mutex);
        if (remove_item(&item))
            fprintf(stderr, "Error in removing item\n");
        else
            printf("Consumer consumed %d\n", item);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[]) {
    int sleep_time, producer_threads, consumer_threads;
    pthread_t *producers, *consumers;
    int i;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s sleep_time producer_threads consumer_threads\n", argv[0]);
        return -1;
    }

    srand(time(NULL));

    sleep_time = atoi(argv[1]);
    producer_threads = atoi(argv[2]);
    consumer_threads = atoi(argv[3]);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);

    producers = malloc(sizeof(pthread_t) * producer_threads);
    consumers = malloc(sizeof(pthread_t) * consumer_threads);

    for (i = 0; i < producer_threads; i++)
        pthread_create(&producers[i], NULL, producer, NULL);
    for (i = 0; i < consumer_threads; i++)
        pthread_create(&consumers[i], NULL, consumer, NULL);

    sleep(sleep_time);

    for (i = 0; i < producer_threads; i++)
        pthread_cancel(producers[i]);
    for (i = 0; i < consumer_threads; i++)
        pthread_cancel(consumers[i]);

    free(producers);
    free(consumers);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    return 0;
}

