#ifndef OS_DRIVER_CONSOLE_H
#define OS_DRIVER_CONSOLE_H

void cons_init(void);
void cons_putc(int c);
int cons_getc(void);
void serial_intr(void);
void kbd_intr(void);

#endif /* OS_DRIVER_CONSOLE_H */

