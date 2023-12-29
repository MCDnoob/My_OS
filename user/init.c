
#include <stdio.h>
#include <ulib.h>

int a[10] = {1};
int b[100];

const char* aaa = "aaaa";
int m = 7;
int magic = 0x4534;

void test_fork()
{
    int pid;
    int code;

    cprintf("I am the parent. Forking the child...\n");
    if ((pid = fork()) == 0) {
        cprintf("I am the child.pid:%d,m:%d\n", getpid(), m);
        exit(magic);
    } else if (pid > 0) {
        m++;
        cprintf("I am parent, fork a child pid %d,m:%d\n",pid, m);
        cprintf("I am the parent, waiting now..\n");
        if ((waitpid(pid, &code) == 0 && code == magic)
                &&(waitpid(pid, &code) != 0 && wait() != 0)) {
            cprintf("waitpid %d ok.\n", pid);
        } else {
            cprintf("waitpid %d failed.\n", pid);
        }
        //fork again
        if ((pid = fork()) == 0) {
            cprintf("I am the childx.pid:%d,m:%d\n", getpid(), m);
            exit(magic);
        } else if (pid > 0) {
            m++;
            cprintf("I am parent, fork a childx pid %d,m:%d\n",pid, m);
            cprintf("I am the parent, waitingx now..\n");
            if (wait() == 0 && wait() != 0) {
                cprintf("waitpidx %d ok.\n", pid);
            } else {
                cprintf("waitpidx %d failed.\n", pid);
            }
        }
    }
}

int main(void) {
    cprintf("aaaHello world :%s\n", aaa);
    cprintf("I am user init process,pid:%d\n", getpid());
    test_fork();

    while (wait() == 0);
    return 0;
}

