#ifndef OS_USER_LIB_SYSCALL_H
#define OS_USER_LIB_SYSCALL_H

int sys_putc(int c);
int sys_fork();
int sys_exit(int error_code);
int sys_wait(int pid, int *store);

#endif /* !OS_USER_LIB_SYSCALL_H */

