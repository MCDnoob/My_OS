#ifndef OS_PROCESS_SCHED_H
#define OS_PROCESS_SCHED_H

#include <proc.h>

void schedule(void);
void wakeup_proc(struct proc_struct *proc);

#endif /* !OS_PROCESS_SCHED_H */

