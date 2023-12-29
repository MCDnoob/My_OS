
#ifndef OS_MM_PMM_H
#define OS_MM_PMM_H

#include <types.h>
#include <mmu.h>
#include <memlayout.h>
#include <assert.h>
#include <kmalloc.h>

extern struct Page* pages;
extern size_t npages;

void pmm_init();
void tlb_invalidate(pde_t *pgdir, uintptr_t va);
struct Page* alloc_page();
void free_page(struct Page *pp);
struct Page* alloc_pages(size_t n);
void free_pages(struct Page *pp, size_t n);

/* This macro takes a kernel virtual address -- an address that points above
 * KERNBASE, where the machine's maximum 256MB of physical memory is mapped --
 * and returns the corresponding physical address.  It panics if you pass it a
 * non-kernel virtual address.
 */
#define PADDR(kva) _paddr(__FILE__, __LINE__, kva)

static inline physaddr_t _paddr(const char *file, int line, void *kva)
{
    uintptr_t m_kva = (uintptr_t)(kva);
    if (m_kva < KERNBASE) {
    	__panic(file, line, "PADDR called with invalid kva %08lx", kva);
    }
    return m_kva - KERNBASE;
}

/* This macro takes a physical address and returns the corresponding kernel
 * virtual address.  It panics if you pass an invalid physical address. */
#define KADDR(pa) _kaddr(__FILE__, __LINE__, pa)

static inline void* _kaddr(const char *file, int line, physaddr_t pa)
{
    size_t m_ppn = PPN(pa);
    if (m_ppn >= npages) {
    	__panic(file, line, "KADDR called with invalid pa %08lx", pa);
    }
    return (void *) (pa + KERNBASE);
}

static inline ppn_t page2ppn(struct Page *page) {
    return page - pages;
}

static inline uintptr_t page2pa(struct Page *page) {
    return page2ppn(page) << PGSHIFT;
}

static inline struct Page* pa2page(uintptr_t pa) {
    if (PPN(pa) >= npages) {
        panic("pa2page called with invalid pa");
    }
    return &pages[PPN(pa)];
}

static inline void * page2kva(struct Page *page) {
    return KADDR(page2pa(page));
}

static inline struct Page * kva2page(void *kva) {
    return pa2page(PADDR(kva));
}

static inline struct Page * pte2page(pte_t pte) {
    if (!(pte & PTE_P)) {
        panic("pte2page called with invalid pte");
    }
    return pa2page(PTE_ADDR(pte));
}

static inline struct Page * pde2page(pde_t pde) {
    return pa2page(PDE_ADDR(pde));
}

static inline int
page_ref(struct Page *page) {
    return page->pp_ref;
}

static inline void
set_page_ref(struct Page *page, int val) {
    page->pp_ref = val;
}

static inline int
page_ref_inc(struct Page *page) {
    page->pp_ref += 1;
    return page->pp_ref;
}

static inline int
page_ref_dec(struct Page *page) {
    page->pp_ref -= 1;
    return page->pp_ref;
}

extern char bootstack[], bootstacktop[];

#endif /* OS_MM_PMM_H */
