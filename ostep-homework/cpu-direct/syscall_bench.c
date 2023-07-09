#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    char inbuf[1];
    struct timeval *t1, *t2;
    t1 = (struct timeval*)malloc(sizeof(struct timeval));
    t2 = (struct timeval*)malloc(sizeof(struct timeval));
    int n = 100000;
    int sum = 0;
    for(int i = 0; i < n; i++) {
        gettimeofday(t1, NULL);
        read(0, inbuf, 0);
        gettimeofday(t2, NULL);
        sum += t2->tv_usec - t1->tv_usec;
    }
    printf("%lf\n", sum * 1.0 / n);
    free(t1);
    free(t2);
    return 0;
}