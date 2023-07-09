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
    int rc = fork();
    assert(rc >= 0);
    if (rc == 0) {
        char* argv[3];
        argv[0] = "/bin/ls";
        argv[1] = "-a";
        argv[2] = NULL;
        execv(argv[0],argv);
    } else {
        int rc_wait = wait(NULL);
        printf("goodbye \n");
    }
    return 0;
}