#include <x86.h>
#include <error.h>
#include <types.h>
#include <stdio.h>
#include <string.h>
#include <rtc.h>
#include <mmu.h>
#include <memlayout.h>
#include <pmm.h>

size_t npages;			// Amount of physical memory (in pages)
static size_t npages_basemem;	// Amount of base memory (in pages)

// These variables are set in mem_init()
pde_t *kern_pgdir;		// Kernel's initial page directory
struct Page *pages;		// Physical page state array
static struct Page *page_free_list;	// Free list of physical pages
static unsigned int nr_free; // # of free pages in this free list

static void check_page_alloc();
static void check_pgdir();
static void check_kern_pgdir();

/* *
 * Task State Segment:
 *
 * The TSS may reside anywhere in memory. A special segment register called
 * the Task Register (TR) holds a segment selector that points a valid TSS
 * segment descriptor which resides in the GDT. Therefore, to use a TSS
 * the following must be done in function gdt_init:
 *   - create a TSS descriptor entry in GDT
 *   - add enough information to the TSS in memory as needed
 *   - load the TR register with a segment selector for that segment
 *
 * There are several fileds in TSS for specifying the new stack pointer when a
 * privilege level change happens. But only the fields SS0 and ESP0 are useful
 * in our os kernel.
 *
 * The field SS0 contains the stack segment selector for CPL = 0, and the ESP0
 * contains the new ESP value for CPL = 0. When an interrupt happens in protected
 * mode, the x86 CPU will look in the TSS for SS0 and ESP0 and load their value
 * into SS and ESP respectively.
 * */
static struct taskstate ts = {0};

/* *
 * Global Descriptor Table:
 *
 * The kernel and user segments are identical (except for the DPL). To load
 * the %ss register, the CPL must equal the DPL. Thus, we must duplicate the
 * segments for the user and the kernel. Defined as follows:
 *   - 0x0 :  unused (always faults -- for trapping NULL far pointers)
 *   - 0x8 :  kernel code segment
 *   - 0x10:  kernel data segment
 *   - 0x18:  user code segment
 *   - 0x20:  user data segment
 *   - 0x28:  defined for tss, initialized in gdt_init
 * */
static struct segdesc gdt[] = {
    SEG_NULL,
    [SEG_KTEXT] = SEG(STA_X | STA_R, 0x0, 0xFFFFFFFF, DPL_KERNEL),
    [SEG_KDATA] = SEG(STA_W, 0x0, 0xFFFFFFFF, DPL_KERNEL),
    [SEG_UTEXT] = SEG(STA_X | STA_R, 0x0, 0xFFFFFFFF, DPL_USER),
    [SEG_UDATA] = SEG(STA_W, 0x0, 0xFFFFFFFF, DPL_USER),
    [SEG_TSS]   = SEG_NULL,
};

static struct pseudodesc gdt_pd = {
    sizeof(gdt) - 1, (uintptr_t)gdt
};

/* *
 * lgdt - load the global descriptor table register and reset the
 * data/code segement registers for kernel.
 * */
static inline void lgdt(struct pseudodesc *pd)
{
    asm volatile ("lgdt (%0)" :: "r" (pd));
    asm volatile ("movw %%ax, %%gs" :: "a" (USER_DS));
    asm volatile ("movw %%ax, %%fs" :: "a" (USER_DS));
    asm volatile ("movw %%ax, %%es" :: "a" (KERNEL_DS));
    asm volatile ("movw %%ax, %%ds" :: "a" (KERNEL_DS));
    asm volatile ("movw %%ax, %%ss" :: "a" (KERNEL_DS));
    // reload cs
    asm volatile ("ljmp %0, $1f\n 1:\n" :: "i" (KERNEL_CS));
}

/* gdt_init - initialize the default GDT and TSS */
static void gdt_init()
{
#define STS_T32A        0x9         // Available 32-bit TSS
    // set boot kernel stack and default SS0
	ts.ts_esp0 = (uintptr_t)bootstacktop;
    ts.ts_ss0 = KERNEL_DS;

    // initialize the TSS filed of the gdt
    gdt[SEG_TSS] = SEGTSS(STS_T32A, (uintptr_t)&ts, sizeof(ts), DPL_KERNEL);

    // reload all segment registers
    lgdt(&gdt_pd);

    // load the TSS
    ltr(GD_TSS);
}

// --------------------------------------------------------------
// Detect machine's physical memory setup.
// --------------------------------------------------------------

static void i386_detect_memory(void)
{
	size_t basemem, extmem, ext16mem, totalmem;

	// Use CMOS calls to measure available base & extended memory.
	// (CMOS calls return results in kilobytes.)
	basemem = nvram_read(NVRAM_BASELO);
	extmem = nvram_read(NVRAM_EXTLO);
	ext16mem = nvram_read(NVRAM_EXT16LO) * 64;

	// Calculate the number of physical pages available in both base
	// and extended memory.
	if (ext16mem)
		totalmem = 16 * 1024 + ext16mem;
	else if (extmem)
		totalmem = 1 * 1024 + extmem;
	else
		totalmem = basemem;

	if (totalmem*1024 > KMEMSIZE)
		totalmem = KMEMSIZE/1024;

	npages = totalmem / (PGSIZE / 1024);
	npages_basemem = basemem / (PGSIZE / 1024);

	cprintf("Physical memory: %uK available, base = %uK, extended = %uK, npages = %d, npages_basemem = %d\n",
		totalmem, basemem, totalmem - basemem, npages, npages_basemem);
}

// This simple physical memory allocator is used only while JOS is setting
// up its virtual memory system.  page_alloc() is the real allocator.
//
// If n>0, allocates enough pages of contiguous physical memory to hold 'n'
// bytes.  Doesn't initialize the memory.  Returns a kernel virtual address.
//
// If n==0, returns the address of the next free page without allocating
// anything.
//
// If we're out of memory, boot_alloc should panic.
// This function may ONLY be used during initialization,
// before the page_free_list list has been set up.
static void * boot_alloc(uint32_t n)
{
	static char *nextfree;	// virtual address of next byte of free memory
	char *result;

	// Initialize nextfree if this is the first time.
	// 'end' is a magic symbol automatically generated by the linker,
	// which points to the end of the kernel's bss segment:
	// the first virtual address that the linker did *not* assign
	// to any kernel code or global variables.
	if (!nextfree) {
		extern char end[];
		nextfree = ROUNDUP((char *) end, PGSIZE);
	}

	// Allocate a chunk large enough to hold 'n' bytes, then update
	// nextfree.  Make sure nextfree is kept aligned
	// to a multiple of PGSIZE.
	//
	// Lab2-1,your code here

	return result;
}

// --------------------------------------------------------------
// Tracking of physical pages.
// The 'pages' array has one 'struct PageInfo' entry per physical page.
// Pages are reference counted, and free pages are kept on a linked list.
// --------------------------------------------------------------
//
// Initialize page structure and memory free list.
// After this is done, NEVER use boot_alloc again.  ONLY use the page
// allocator functions below to allocate and deallocate physical
// memory via the page_free_list.
//
static void page_init()
{
  // Lab2-1,your code here
  // get virtual address(va) of each page,
  // if va < nextfree,then can't use this page,set pp_ref=1
  // if va >= nextfree,put this page on page_free_list,set pp_ref=0

}

#if 1
struct Page* alloc_pages(size_t n)
{
    assert(n > 0);
    if (n > nr_free) {
        return NULL ;
    }

    struct Page *le, *len, *lep;//len是le的下一个元素
    le = page_free_list;
    lep = NULL;
    //遍历链表找到容量>=n的空闲块le,设置le的前一个元素lep
    while (le != NULL) {
        struct Page *p = le;
        if (p->capacity >= n) {
            // Lab2-2,your code here
            //1.从le分配n页出去，更新le为len,可能需要更新page_free_list

            //2.若p的容量>n,说明le空闲块还有剩余,更新le的容量

            nr_free -= n;
            return p;
        }
        lep = le;
        le = le->pp_link;
    }

    return NULL ;
}

void free_pages(struct Page *base, size_t n)
{
    assert(n > 0);

    struct Page *le, *lep, *basep;//base插入到le之前,lep代表le的前一个元素,basep代表base插入后它的前一个元素
    struct Page *leprehead = NULL;//le的上一个空闲块的头部，basep的头部
    le = page_free_list;
    lep = NULL;

    // Lab2-2,your code here
    //1.找到在链表中的插入位置le，设置le,lep,leprehead

    //设置basep
    basep = lep;
    //2.循环把base~base+n插入到le之前,更新lep
    struct Page * p;

    //设置base容量
    base->capacity = n;
    //3.如果base插入后与后续块le连续，更新base和le容量

    //4.如果base插入后与前面的块basep连续，更新base和basep的头部leprehead的容量


    //若basep==null,设置page_free_list = base
    if(basep==NULL) page_free_list = base;

    nr_free += n;
}
#else
struct Page* alloc_pages(size_t n)
{
  struct Page *result,*left,*right;
  int m;

  //local_intr_save(intr_flag);
  result = page_free_list;
  left = NULL;
  right = page_free_list;
  m = n;

  if (0 == n ||page_free_list < pages || &pages[npages] < page_free_list) {
      return NULL;
  }

  while(n-- > 0) {
      if (right < pages || &pages[npages] < right) {
          cprintf("not memory to alloc %d pages\n", m);
          return NULL;
      }
      left = right;
      right = right->pp_link;
  }

  page_free_list = right;
  left->pp_link = NULL;
  // local_intr_restore(intr_flag);
  return result;
}

void free_pages(struct Page *pp, size_t n)
{
  struct Page *m = NULL;

  //local_intr_save(intr_flag);
  if (NULL == pp || 0 == n)
      panic("page_free: NULL == pp\n");

  m = pp;

  while(--n > 0) {
      if (NULL == m || m->pp_ref != 0)
          panic("page_free: mm->pp_ref is nonzero or m:0x%x\n", m);
      m = m->pp_link;
  }
  if (NULL == m || m->pp_link != NULL)
      panic("page_free: m->pp_link != NULL wrong free num\n");
  if (m->pp_ref != 0)
      panic("page_free: pp->pp_ref is nonzero\n");

  m->pp_link = page_free_list;
  page_free_list = pp;
  // local_intr_restore(intr_flag);
}
#endif

struct Page* alloc_page()
{
    return alloc_pages(1);
}

void free_page(struct Page *pp)
{
    free_pages(pp, 1);
}

//get_pte - get page table addr of va.
pte_t * get_pte(pde_t *pgdir, uintptr_t va, bool create)
{
    // Lab2-1,your code here
    // use pgdir,PDX,PTE_P,alloc_page(),page2pa(),memset,KADDR,pa | PTE_U | PTE_W | PTE_P,KADDR,PDE_ADDR,PTX
    return 0;
}

//get_page - get related Page struct for linear address la using PDT pgdir
struct Page *get_pte_page(pde_t *pgdir, uintptr_t va, pte_t **ptep_store)
{
    pte_t *ptep = get_pte(pgdir, va, 0);
    if (ptep_store != NULL) {
        *ptep_store = ptep;
    }
    if (ptep != NULL && (*ptep & PTE_P)) {
        return pte2page(*ptep);
    }
    return NULL;
}

//page_remove_pte - free an Page sturct which is related virtual address va
//                - and clean(invalidate) pte which is related virtual address va
//note: PT is changed, so the TLB need to be invalidate
static void page_remove_pte(pde_t *pgdir, uintptr_t va, pte_t *ptep)
{
    // Lab2-1,your code here
    // use PTE_P,pte2page,page_ref_dec,free_page,tlb_invalidate()

}

//page_remove - free an Page which is related virtual address va and has an validated pte
void page_remove(pde_t *pgdir, uintptr_t va)
{
    pte_t *ptep = get_pte(pgdir, va, 0);
    if (ptep != NULL) {
        page_remove_pte(pgdir, va, ptep);
    }
}

//page_insert - build the map of phy addr of an Page with the addr va
// paramemters:
//  pgdir: the kernel virtual base address of PDT
//  page:  the Page which need to map
//  va:    the virtual address need to map
//  perm:  the permission of this Page which is setted in related pte
// return value: always 0
//note: PT is changed, so the TLB need to be invalidate
int page_insert(pde_t *pgdir, struct Page *page, uintptr_t va, uint32_t perm)
{
    pte_t *ptep = get_pte(pgdir, va, 1);
    if (ptep == NULL) {
        return -E_NO_MEM;
    }
    page_ref_inc(page);
    if (*ptep & PTE_P) {
        struct Page *p = pte2page(*ptep);
        if (p == page) {
            page_ref_dec(page);
        } else {
            page_remove_pte(pgdir, va, ptep);
        }
    }
    *ptep = page2pa(page) | PTE_P | perm;
    tlb_invalidate(pgdir, va);
    return 0;
}

// invalidate a TLB entry, but only if the page tables being
// edited are the ones currently in use by the processor.
void tlb_invalidate(pde_t *pgdir, uintptr_t va)
{
    if (rcr3() == PADDR(pgdir)) {
        invlpg((void *)va);
    }
}

// Map [va, va+size) of virtual address space to physical [pa, pa+size)
// in the page table rooted at pgdir.  Size is a multiple of PGSIZE, and
// va and pa are both page-aligned.
// Use permission bits perm|PTE_P for the entries.
//
// This function is only intended to set up the ``static'' mappings
// above UTOP. As such, it should *not* change the pp_ref field on the
// mapped pages.
//
static void boot_map_region(pde_t *pgdir, uintptr_t va, size_t size, uintptr_t pa, uint32_t perm)
{
    assert(PGOFF(va) == PGOFF(pa));
    int n = ROUNDUP(size + PGOFF(va), PGSIZE) / PGSIZE;
    va = ROUNDDOWN(va, PGSIZE);
    pa = ROUNDDOWN(pa, PGSIZE);
    // Lab2-1,your code here
    // get_pte of each va,set pa and PTE_P|perm to pte

}

void pmm_init()
{
	uint32_t cr0;
	i386_detect_memory();

	//panic("");

	// create initial page directory.
	kern_pgdir = (pde_t *)boot_alloc(PGSIZE);
	memset(kern_pgdir, 0, PGSIZE);

	// Allocate an array of npages 'struct PageInfo's and store it in 'pages'.
	// The kernel uses this array to keep track of physical pages: for
	// each physical page, there is a corresponding struct PageInfo in this
	// array.  'npages' is the number of physical pages in memory.  Use memset
	// to initialize all fields of each struct PageInfo to 0.
	pages = (struct Page *)boot_alloc(npages * sizeof(struct Page));
	memset(pages, 0, npages * sizeof(struct Page));

	// Now that we've allocated the initial kernel data structures, we set
	// up the list of free physical pages. Once we've done so, all further
	// memory management will go through the page_* functions. In
	// particular, we can now map memory using boot_map_region
	// or page_insert
	page_init();
	check_page_alloc();
	check_kmalloc();

	check_pgdir();

    // map all physical memory to linear memory with base linear addr KERNBASE
    //linear_addr KERNBASE~KERNBASE+KMEMSIZE = phy_addr 0~KMEMSIZE
    //But shouldn't use this map until enable_paging() & gdt_init() finished.
    boot_map_region(kern_pgdir, KERNBASE, npages*4096, 0, PTE_W);

	// Switch from the minimal entry page directory to the full kern_pgdir
	// page table we just created.	Our instruction pointer should be
	// somewhere between KERNBASE and KERNBASE+4MB right now, which is
	// mapped the same way by both page tables.
	//
	// If the machine reboots at this point, you've probably set up your
	// kern_pgdir wrong.
	lcr3(PADDR(kern_pgdir));
	// entry.S set the really important flags in cr0 (including enabling
	// paging).  Here we configure the rest of the flags that we care about.
	cr0 = rcr0();
	cr0 |= CR0_PE|CR0_PG|CR0_AM|CR0_WP|CR0_NE|CR0_MP;
	cr0 &= ~(CR0_TS|CR0_EM);
	lcr0(cr0);

    check_kern_pgdir();

    gdt_init();
}


// --------------------------------------------------------------
// Checking functions.
// --------------------------------------------------------------

//
// Check that the pages on the page_free_list are reasonable.
//
static void check_page_alloc()
{
    struct Page *pp, *pp0, *pp1, *pp2, *pps, *pp3, *pp4, *pp5;
    int nfree;
    struct Page *fl;
    int bfree;
    char *c;
    int i;

    if (!pages)
        panic("'pages' is a null pointer!");

    // check number of free pages
    for (pp = page_free_list, nfree = 0; pp; pp = pp->pp_link)
        ++nfree;

    // should be able to allocate three pages
    pp0 = pp1 = pp2 = 0;
    assert((pp0 = alloc_page()));
    assert((pp1 = alloc_page()));
    assert((pp2 = alloc_page()));

    assert(pp0);
    assert(pp1 && pp1 != pp0);
    assert(pp2 && pp2 != pp1 && pp2 != pp0);
    assert(page2pa(pp0) < npages*PGSIZE);
    assert(page2pa(pp1) < npages*PGSIZE);
    assert(page2pa(pp2) < npages*PGSIZE);

    assert((pps = alloc_pages(10)));
    free_pages(pps, 10);
    pps = 0;

    // temporarily steal the rest of the free pages
    fl = page_free_list;
    page_free_list = 0;
    bfree = nr_free;
    nr_free = 0;

    // should be no free memory
    assert(!alloc_page());

    // free and re-allocate?
    free_page(pp0);
    free_page(pp1);
    free_page(pp2);
    pp0 = pp1 = pp2 = 0;
    assert(!alloc_pages(4));
    assert((pps = alloc_pages(3)));
    assert(!alloc_page());
    free_pages(pps, 3);
    pps = 0;
    assert((pp0 = alloc_page()));
    assert((pp1 = alloc_page()));
    assert((pp2 = alloc_page()));
    assert(pp0);
    assert(pp1 && pp1 != pp0);
    assert(pp2 && pp2 != pp1 && pp2 != pp0);
    assert(!alloc_page());

    // test flags
    memset(page2kva(pp0), 1, PGSIZE);
    free_page(pp0);
    assert((pp = alloc_page()));
    assert(pp && pp0 == pp);
    c = page2kva(pp);
    memset(c, 0, PGSIZE);
    for (i = 0; i < PGSIZE; i++)
        assert(c[i] == 0);

    // give free list back
    page_free_list = fl;
    nr_free = bfree;

    // free the pages we took
    free_page(pp0);
    free_page(pp1);
    free_page(pp2);

    //test first-fit
    pp0 = pp1 = pp2 = pp3 = pp4 = pp5 = 0;
    assert((pp0 = alloc_pages(1)));
    assert((pp1 = alloc_pages(2)));
    assert((pp2 = alloc_pages(2)));
    assert((pp3 = alloc_page()));
    assert((pp4 = alloc_page()));
    free_page(pp0);
    free_pages(pp2, 2);

    assert((pp5 = alloc_pages(3)));
    free_pages(pp5, 3);

    free_page(pp3);
    assert((pp5 = alloc_pages(3)));
    free_pages(pp5, 3);

    free_pages(pp1, 2);
    assert((pp5 = alloc_pages(3)));
    free_pages(pp5, 3);

    free_page(pp4);

    // number of free pages should be the same
    for (pp = page_free_list; pp; pp = pp->pp_link)
        --nfree;
    assert(nfree == 0);

    cprintf("check_page_alloc() succeeded!\n");
}

static void check_pgdir()
{
	int nfree;
	struct Page *pp;
    assert(npages <= KMEMSIZE / PGSIZE);
    assert(kern_pgdir != NULL && (uint32_t)PGOFF(kern_pgdir) == 0);
    assert(get_pte_page(kern_pgdir, 0x0, NULL) == NULL);

	// check number of free pages
	for (pp = page_free_list, nfree = 0; pp; pp = pp->pp_link)
		++nfree;

    struct Page *p1, *p2;
    p1 = alloc_page();
    assert(page_insert(kern_pgdir, p1, 0x0, 0) == 0);

    pte_t *ptep;
    assert((ptep = get_pte(kern_pgdir, 0x0, 0)) != NULL);
    assert(pte2page(*ptep) == p1);
    assert(page_ref(p1) == 1);

    ptep = &((pte_t *)KADDR(PDE_ADDR(kern_pgdir[0])))[1];
    assert(get_pte(kern_pgdir, PGSIZE, 0) == ptep);

    p2 = alloc_page();
    assert(page_insert(kern_pgdir, p2, PGSIZE, PTE_U | PTE_W) == 0);
    assert((ptep = get_pte(kern_pgdir, PGSIZE, 0)) != NULL);
    assert(*ptep & PTE_U);
    assert(*ptep & PTE_W);
    assert(kern_pgdir[0] & PTE_U);
    assert(page_ref(p2) == 1);

    assert(page_insert(kern_pgdir, p1, PGSIZE, 0) == 0);
    assert(page_ref(p1) == 2);
    assert(page_ref(p2) == 0);
    assert((ptep = get_pte(kern_pgdir, PGSIZE, 0)) != NULL);
    assert(pte2page(*ptep) == p1);
    assert((*ptep & PTE_U) == 0);

    page_remove(kern_pgdir, 0x0);
    assert(page_ref(p1) == 1);
    assert(page_ref(p2) == 0);

    page_remove(kern_pgdir, PGSIZE);
    assert(page_ref(p1) == 0);
    assert(page_ref(p2) == 0);

    //assert(page_ref(pde2page(kern_pgdir[0])) == 1);
    free_page(pde2page(kern_pgdir[0]));
    kern_pgdir[0] = 0;

	// number of free pages should be the same
	for (pp = page_free_list; pp; pp = pp->pp_link)
		--nfree;
	assert(nfree == 0);

    cprintf("check_pgdir() succeeded!\n");
}

static void check_kern_pgdir()
{
	int nfree;
	struct Page *pp;
    pte_t *ptep;
    int i;

	// check number of free pages
	for (pp = page_free_list, nfree = 0; pp; pp = pp->pp_link)
		++nfree;

    for (i = 0; i < npages*PGSIZE; i += PGSIZE) {
        assert((ptep = get_pte(kern_pgdir, (uintptr_t)KADDR(i), 0)) != NULL);
        assert(PTE_ADDR(*ptep) == i);
    }

    assert(kern_pgdir[0] == 0);

    struct Page *p;
    p = alloc_page();
    assert(page_insert(kern_pgdir, p, 0x100, PTE_W) == 0);
    assert(page_ref(p) == 1);
    assert(page_insert(kern_pgdir, p, 0x100 + PGSIZE, PTE_W) == 0);
    assert(page_ref(p) == 2);

    const char *str = "Hello world!!";
    strcpy((void *)0x100, str);
    assert(strcmp((void *)0x100, (void *)(0x100 + PGSIZE)) == 0);

    *(char *)(page2kva(p) + 0x100) = '\0';
    assert(strlen((const char *)0x100) == 0);

    page_remove(kern_pgdir, 0x100);
    assert(page_ref(p) == 1);

    page_remove(kern_pgdir, 0x100 + PGSIZE);
    assert(page_ref(p) == 0);

    free_page(pde2page(kern_pgdir[0]));
    kern_pgdir[0] = 0;

	// number of free pages should be the same
	for (pp = page_free_list; pp; pp = pp->pp_link)
		--nfree;
	assert(nfree == 0);

    cprintf("check_kern_pgdir() succeeded!\n");
}

