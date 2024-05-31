#include <stdio.h>

int main() {
    int array[] = {1, 2, 3, 4, 5};

    // Calculate the total size of the array
    size_t totalSize = sizeof(array);

    // Calculate the number of elements in the array
    size_t numElements = sizeof(array) / sizeof(int);

    printf("Total size of array: %zu bytes\n", totalSize);
    printf("Number of elements in array: %zu\n", numElements);

    return 0;
}

