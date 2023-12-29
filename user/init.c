
#include <stdio.h>
#include <ulib.h>

int a[10] = {1};
int b[100];

const char* aaa = "aaaa";
int m = 7;

void test_fork()
{
    int pid;

    cprintf("I am the parent. Forking the child...\n");
    if ((pid = fork()) == 0) {
        cprintf("I am the child.pid:%d,m:%d\n", getpid(), m);
    } else if (pid > 0) {
        m++;
        cprintf("I am parent, fork a child pid %d,m:%d\n",pid, m);
    }
}

int main(void) {
    cprintf("aaaHello world :%s\n", aaa);
    cprintf("I am user init process,pid:%d\n", getpid());
    test_fork();

    return 0;
}

