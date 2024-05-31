#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 10

int list[SIZE] = {9, 7, 5, 3, 1, 2, 4, 6, 8, 10};
int sortedList[SIZE];

void *sort(void *arg) {
    int start = *((int *)arg);
    int end = start + SIZE / 2;
    for (int i = start; i < end; i++) {
        for (int j = start; j < end - 1; j++) {
            if (list[j] > list[j + 1]) {
                int temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
    pthread_exit(NULL);
}

void *merge(void *arg) {
    int start = *((int *)arg);
    int end = start + SIZE / 4;
    int mid = start + SIZE / 2;
    int i = start, j = mid, k = start;
    while (i < mid && j < end) {
        if (list[i] < list[j]) {
            sortedList[k++] = list[i++];
        } else {
            sortedList[k++] = list[j++];
        }
    }
    while (i < mid) {
        sortedList[k++] = list[i++];
    }
    while (j < end) {
        sortedList[k++] = list[j++];
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t sortThreads[4];
    pthread_t mergeThreads[3];
    int threadArgs[4] = {0, SIZE / 4, SIZE / 2, 3 * SIZE / 4};

    for (int i = 0; i < 4; i++) {
        pthread_create(&sortThreads[i], NULL, sort, &threadArgs[i]);
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(sortThreads[i], NULL);
    }

    for (int i = 0; i < 3; i++) {
        pthread_create(&mergeThreads[i], NULL, merge, &threadArgs[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(mergeThreads[i], NULL);
    }

    printf("Sorted List:\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", sortedList[i]);
    }
    printf("\n");

    return 0;
}

