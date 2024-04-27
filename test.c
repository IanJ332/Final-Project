#define TEST_COUNT 10

int main() {
    pthread_t producerThread, consumerThread;
    int i;

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

