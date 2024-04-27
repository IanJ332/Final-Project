#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// 全局变量
int global_variable = 0;
pthread_mutex_t mutex;

// 函数声明
void *producer(void *param);
void *consumer(void *param);

int main() {
    pthread_t producerThread, consumerThread;

    // 初始化互斥锁
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);
    }

    // 创建生产者线程
    if (pthread_create(&producerThread, NULL, producer, NULL) != 0) {
        perror("pthread_create: producer");
        exit(EXIT_FAILURE);
    }

    // 创建消费者线程
    if (pthread_create(&consumerThread, NULL, consumer, NULL) != 0) {
        perror("pthread_create: consumer");
        exit(EXIT_FAILURE);
    }

    // 等待线程结束
    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    // 销毁互斥锁
    if (pthread_mutex_destroy(&mutex) != 0) {
        perror("pthread_mutex_destroy");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void *producer(void *param) {
    while (1) {
        // 加锁
        pthread_mutex_lock(&mutex);

        // 生产过程
        global_variable++;

        // 解锁
        pthread_mutex_unlock(&mutex);

        // 休眠
        sleep(1);
    }

    return NULL;
}

void *consumer(void *param) {
    while (1) {
        // 加锁
        pthread_mutex_lock(&mutex);

        // 消费过程
        global_variable--;

        // 解锁
        pthread_mutex_unlock(&mutex);

        // 休眠
        sleep(1);
    }

    return NULL;
}

