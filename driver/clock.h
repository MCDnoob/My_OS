#ifndef OS_DRIVER_CLOCK_H
#define OS_DRIVER_CLOCK_H

#include <types.h>

extern volatile size_t ticks;

void clock_init(void);

#endif /* !OS_DRIVER_CLOCK_H */

