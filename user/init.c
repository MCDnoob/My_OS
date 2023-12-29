
#include <stdio.h>
#include <ulib.h>

int a[10] = {1};
int b[100];

const char* aaa = "aaaa";

int main(void) {
    cprintf("aaaHello world :%s\n", aaa);
    cprintf("I am user init process,pid:%d\n", getpid());
    return 0;
}

