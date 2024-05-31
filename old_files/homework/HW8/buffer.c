#include "buffer.h"

/* the buffer */
buffer_item buffer[BUFFER_SIZE];

int insert_item(buffer_item item) {
    /* insert item into buffer */
    if (in == BUFFER_SIZE) {
        fprintf(stderr, "Error: Buffer is full\n");
        return -1; // Error: Buffer is full
    }
    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    return 0; // Success
}

int remove_item(buffer_item *item) {
    /* remove an object from buffer
       placing it in item */
    if (out == BUFFER_SIZE) {
        fprintf(stderr, "Error: Buffer is empty\n");
        return -1; // Error: Buffer is empty
    }
    *item = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    return 0; // Success
}


