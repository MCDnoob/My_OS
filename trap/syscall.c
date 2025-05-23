#include <unistd.h>
#include <proc.h>
#include <syscall.h>
#include <trap.h>
#include <stdio.h>
#include <pmm.h>
#include <assert.h>

static int sys_exec(uint32_t arg[])
{
    const char *name = (const char *)arg[0];
    size_t len = (size_t)arg[1];
    unsigned char *binary = (unsigned char *)arg[2];
    size_t size = (size_t)arg[3];
    return do_execve(name, len, binary, size);
}

static int sys_putc(uint32_t arg[])
{
    int c = (int) arg[0];
    cputchar(c);
    return 0;
}

static int sys_fork(uint32_t arg[])
{
    struct trapframe *tf = current->tf;
    uintptr_t stack = tf->tf_esp;
    return do_fork(0, stack, tf);
}

static int sys_exit(uint32_t arg[])
{
    int error_code = (int) arg[0];
    return do_exit(error_code);
}

static int sys_wait(uint32_t arg[])
{
    int pid = (int) arg[0];
    int *store = (int *) arg[1];
    return do_wait(pid, store);
}

static int (*syscalls[])(uint32_t arg[]) = {
        [SYS_exec]              sys_exec,
        [SYS_putc]              sys_putc,
        [SYS_fork]              sys_fork,
        [SYS_exit]              sys_exit,
        [SYS_wait]              sys_wait,
};

#define NUM_SYSCALLS        ((sizeof(syscalls)) / (sizeof(syscalls[0])))

void syscall(struct trapframe *tf)
{
    uint32_t arg[5];
    int num = tf->tf_regs.reg_eax;
    if (num >= 0 && num < NUM_SYSCALLS) {
        if (syscalls[num] != NULL) {
            arg[0] = tf->tf_regs.reg_edx;
            arg[1] = tf->tf_regs.reg_ecx;
            arg[2] = tf->tf_regs.reg_ebx;
            arg[3] = tf->tf_regs.reg_edi;
            arg[4] = tf->tf_regs.reg_esi;
            tf->tf_regs.reg_eax = syscalls[num](arg);
            return ;
        }
    }
    print_trapframe(tf);
    panic("undefined syscall %d, pid = %d, name = %s.\n",
            num, current->pid, current->name);
}

