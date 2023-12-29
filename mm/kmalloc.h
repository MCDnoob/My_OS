#ifndef OS_MM_KMALLOC_H
#define OS_MM_KMALLOC_H

#include <types.h>

#define KMALLOC_MAX_ORDER       10

void kmalloc_init(void);

void *kmalloc(size_t n);
void kfree(void *objp/*, size_t n*/);

size_t kallocated(void);

void check_kmalloc();

#endif /* OS_MM_KMALLOC_H */

