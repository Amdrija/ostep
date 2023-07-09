#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {

    int mbs = atoi(argv[1]);
    int size = mbs * 1024;
    printf("%d\n", getpid());
    char* array = (char*)malloc(sizeof(char) * size);
    char buf[232000];
    for(int i = 0;; i = (i + 1) % size) {
        array[i] = i % 128;
        buf[i%100000] = 1;
    };
    free(array);

    return 0;
}