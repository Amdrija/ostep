#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    // process a
    int p[2];
    pipe(p);
    int rc = fork();
    assert(rc >= 0);
    if (rc == 0) {
        printf("child1 pid: %d waiting to receive message on %d\n", getpid(), p[0]);
        char inbuf[2];
        read(p[0], inbuf, 1);
        inbuf[1] = '\0';
        printf("child1 pid: %d message: %s\n", getpid(), inbuf);
    } else {
        int rc = fork();
        if (rc == 0) {
            printf("child2 pid: %d writing message on %d\n", getpid(), p[1]);
            write(p[1], "A", 1);
        }
    }
    return 0;
}