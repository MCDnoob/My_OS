#ifndef OS_MM_MEMLAYOUT_H
#define OS_MM_MEMLAYOUT_H

/* This file contains the definitions for memory management in our OS. */

/* *
 *     Virtual memory map:
 *     4G ------------------> +---------------------------------+ 0xFFFFFFFF
 *                            |         High Memory (*)         | 128M
 *     KERNTOP -------------> +---------------------------------+ 0xF8000000
 *                            |    Remapped Physical Memory     | KMEMSIZE(896M)
 *                            |                                 |
 *     KERNBASE ------------> +---------------------------------+ 0xC0000000(3G)
 *                            |        Invalid Memory (*)       | --/--
 *     USERTOP -------------> +---------------------------------+ 0xB0000000
 *                            |           User stack            |
 *                            +---------------------------------+
 *                            |                                 |
 *                            :                                 :
 *                            |         ~~~~~~~~~~~~~~~~        |
 *                            :                                 :
 *                            |                                 |
 *                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                            |       User Program & Heap       |
 *     UTEXT ---------------> +---------------------------------+ 0x00800000
 *                            |        Invalid Memory (*)       | --/--
 *                            |  - - - - - - - - - - - - - - -  |
 *                            |    User STAB Data (optional)    |
 *     USERBASE, USTAB------> +---------------------------------+ 0x00200000
 *                            |        Invalid Memory (*)       | --/--
 *     0 -------------------> +---------------------------------+ 0x00000000
 * (*) Note: The kernel ensures that "Invalid Memory" is *never* mapped.
 *     "Empty Memory" is normally unmapped, but user programs may map pages
 *     there if desired.
 *
 *
 *    physical memory:
 *     4G -------------  ---> +---------------------------------+ 0xFFFFFFFF
 *                            |           外设映射空间            |
 *                            |                                 |
 *     384M ----------------> +---------------------------------+ 0x20000000
 *                            |           空闲内存~382M          |
 *                            |                                 |
 *     pages end -----------> +---------------------------------+ pages end
 *                            |    npages*sizeof(struct Page)   | -- (768KB)
 *     kpgdir end ----------> +---------------------------------+ kpgdir end
 *                            |           kern_pgdir            | -- PGSIZE
 *     bss end -------------> +---------------------------------+ bss end
 *                            |           kernel code           |
 *     1M ------------------> +---------------------------------+ 0x00100000
 *                            |           BIOS ROM              |
 *     960KB ---------------> +---------------------------------+ 0x000F0000
 *                            |           16位外设,扩展ROMS       |
 *     768KB ---------------> +---------------------------------+ 0x000C0000
 *                            |           VGA显示缓存            |
 *     640KB ---------------> +---------------------------------+ 0x000A0000
 *                            |           bootloader            |
 *     0  ------------------> +---------------------------------+ 0x00000000
 *
 * */

/* All physical memory mapped at this address */
#define KERNBASE            0xC0000000
#define KMEMSIZE            0x38000000                  // the maximum amount of physical memory
#define KERNTOP             (KERNBASE + KMEMSIZE)

#define KSTKPAGE          2                    // # of pages in kernel stack
#define KSTKSIZE   (KSTKPAGE*PGSIZE)           // size of a kernel stack

#define USERTOP             0xB0000000
#define USTACKTOP           USERTOP
#define USTACKPAGE          256                         // # of pages in user stack
#define USTACKSIZE          (USTACKPAGE * PGSIZE)       // sizeof user stack

#define USERBASE            0x00200000
#define UTEXT               0x00800000                  // where user programs generally begin
#define USTAB               USERBASE                    // the location of the user STABS data structure

#define USER_ACCESS(start, end)                     \
(USERBASE <= (start) && (start) < (end) && (end) <= USERTOP)

#define KERN_ACCESS(start, end)                     \
(KERNBASE <= (start) && (start) < (end) && (end) <= KERNTOP)

#ifndef __ASSEMBLER__
/*
 * Page descriptor structures, mapped at UPAGES.
 * Read/write to the kernel, read-only to user programs.
 *
 * Each struct Page stores metadata for one physical page.
 * Is it NOT the physical page itself, but there is a one-to-one
 * correspondence between physical pages and struct PageInfo's.
 * You can map a struct PageInfo * to the corresponding physical address
 * with page2pa() in kern/pmap.h.
 */
struct Page {
	// Next page on the free list.
	struct Page *pp_link;

	// pp_ref is the count of pointers (usually in page table entries)
	// to this page, for pages allocated using page_alloc.
	// Pages allocated at boot time using pmap.c's
	// boot_alloc do not have valid reference count fields.

	uint16_t pp_ref;

	unsigned int capacity;
};
#endif /* not __ASSEMBLER__ */

#endif /* OS_MM_MEMLAYOUT_H */
