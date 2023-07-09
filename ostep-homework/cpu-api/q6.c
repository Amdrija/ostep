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
        printf("calling ls, %d\n", rc);
        char* argv[3];
        argv[0] = "/bin/ls";
        argv[1] = "-a";
        argv[2] = NULL;
        int rc_wait = wait(NULL);
        printf("goodbye rc_wait:%d rc:%d\n", rc_wait, rc);
        execv(argv[0],argv);
    } else {
        int rc_wait = waitpid(rc, NULL, 0); //if we modify the first argument, then it will wait for process with PID of that argument
        printf("goodbye rc_wait:%d rc:%d\n", rc_wait, rc);
    }
    return 0;
}