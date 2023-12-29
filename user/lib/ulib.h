#ifndef OS_USER_LIB_ULIB_H
#define OS_USER_LIB_ULIB_H

int getpid();
int fork();
void exit(int error_code);
int wait();
int waitpid(int pid, int *store);

#endif /* !OS_USER_LIB_ULIB_H */

