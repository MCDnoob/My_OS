
#include <syscall.h>
#include <stdio.h>
#include <ulib.h>

int getpid()
{
    // Lab4-2,your code here
    return 0;;
}

int fork()
{
    return sys_fork();
}

void exit(int error_code)
{
    sys_exit(error_code);
    cprintf("BUG: exit failed.\n");
    while (1);
}

int wait()
{
    return sys_wait(0, NULL );
}

int waitpid(int pid, int *store)
{
    return sys_wait(pid, store);
}


