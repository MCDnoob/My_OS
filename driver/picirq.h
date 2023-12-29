#ifndef OS_DRIVER_PICIRQ_H
#define OS_DRIVER_PICIRQ_H

void pic_init(void);
void pic_enable(unsigned int irq);

#define IRQ_OFFSET      32

#endif /* !OS_DRIVER_PICIRQ_H */

