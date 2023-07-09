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
    int x = 100;
    char buf[1];
    int fd = open("./q2.txt", O_RDWR | O_CREAT);
    int rc = fork();
    assert(rc >= 0);
    if (rc == 0) {
        x = 65;
    } else {
        //int rc_wait = wait(NULL);
        x = 66;
    }
    buf[0] = x;
    write(fd, buf, 1);
    printf("%d",fd);
    return 0;
}