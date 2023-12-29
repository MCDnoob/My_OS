
#include <syscall.h>
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


