#ifndef OS_SYS_X86_H
#define OS_SYS_X86_H

#include <types.h>

static inline uint8_t inb(uint16_t port) __attribute__((always_inline));
static inline void insl(uint32_t port, void *addr, int cnt) __attribute__((always_inline));
static inline void outb(uint16_t port, uint8_t data) __attribute__((always_inline));
static inline void outw(uint16_t port, uint16_t data) __attribute__((always_inline));
static inline void outsl(uint32_t port, const void *addr, int cnt) __attribute__((always_inline));
static inline void outl(int port, uint32_t data) __attribute__((always_inline));
static inline uint32_t inl(int port) __attribute__((always_inline));


static inline uint8_t inb(uint16_t port)
{
    uint8_t data;
    asm volatile ("inb %1, %0" : "=a" (data) : "d" (port) : "memory");
    return data;
}

static inline void insl(uint32_t port, void *addr, int cnt)
{
    asm volatile (
        "cld;"
        "repne; insl;"
        : "=D" (addr), "=c" (cnt)
        : "d" (port), "0" (addr), "1" (cnt)
        : "memory", "cc");
}

static inline void outb(uint16_t port, uint8_t data)
{
    asm volatile ("outb %0, %1" :: "a" (data), "d" (port) : "memory");
}

static inline void outw(uint16_t port, uint16_t data)
{
    asm volatile ("outw %0, %1" :: "a" (data), "d" (port) : "memory");
}

static inline void outsl(uint32_t port, const void *addr, int cnt)
{
    asm volatile (
        "cld;"
        "repne; outsl;"
        : "=S" (addr), "=c" (cnt)
        : "d" (port), "0" (addr), "1" (cnt)
        : "memory", "cc");
}

static inline void outl(int port, uint32_t data)
{
	asm volatile("outl %0,%w1" : : "a" (data), "d" (port));
}

static inline uint32_t inl(int port)
{
	uint32_t data;
	asm volatile("inl %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

#endif /* OS_SYS_X86_H */

