#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // process a
    int x = 100;
    int rc = fork();
    assert(rc >= 0);
    if (rc == 0) {
        x = 200;
    } else {
        int rc_wait = wait(NULL);
        x = 300;
    }
    printf("%d",x);
    return 0;
}