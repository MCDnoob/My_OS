#ifndef OS_TRAP_TRAP_H
#define OS_TRAP_TRAP_H

#include <types.h>

/* Trap Numbers */

/* Processor-defined: */
#define T_DIVIDE                0   // divide error
#define T_DEBUG                 1   // debug exception
#define T_NMI                   2   // non-maskable interrupt
#define T_BRKPT                 3   // breakpoint
#define T_OFLOW                 4   // overflow
#define T_BOUND                 5   // bounds check
#define T_ILLOP                 6   // illegal opcode
#define T_DEVICE                7   // device not available
#define T_DBLFLT                8   // double fault
// #define T_COPROC             9   // reserved (not used since 486)
#define T_TSS                   10  // invalid task switch segment
#define T_SEGNP                 11  // segment not present
#define T_STACK                 12  // stack exception
#define T_GPFLT                 13  // general protection fault
#define T_PGFLT                 14  // page fault
// #define T_RES                15  // reserved
#define T_FPERR                 16  // floating point error
#define T_ALIGN                 17  // aligment check
#define T_MCHK                  18  // machine check
#define T_SIMDERR               19  // SIMD floating point error

//#define T_SYSCALL               0x80 // SYSCALL, ONLY FOR THIS PROJ

/* Hardware IRQ numbers. We receive these as (IRQ_OFFSET + IRQ_xx) */
#define IRQ_OFFSET              32  // IRQ 0 corresponds to int IRQ_OFFSET

#define IRQ_TIMER               0
#define IRQ_KBD                 1
#define IRQ_COM1                4
#define IRQ_IDE1                14
#define IRQ_IDE2                15
#define IRQ_ERROR               19
#define IRQ_SPURIOUS            31

/* Gate descriptors for interrupts and traps */
struct gatedesc {
    unsigned gd_off_15_0 : 16;      // low 16 bits of offset in segment
    unsigned gd_ss : 16;            // segment selector
    unsigned gd_args : 5;           // # args, 0 for interrupt/trap gates
    unsigned gd_rsv1 : 3;           // reserved(should be zero I guess)
    unsigned gd_type : 4;           // type(STS_{TG,IG32,TG32})
    unsigned gd_s : 1;              // must be 0 (system)
    unsigned gd_dpl : 2;            // descriptor(meaning new) privilege level
    unsigned gd_p : 1;              // Present
    unsigned gd_off_31_16 : 16;     // high bits of offset in segment
};

/* System segment type bits */
#define STS_T16A        0x1         // Available 16-bit TSS
#define STS_LDT         0x2         // Local Descriptor Table
#define STS_T16B        0x3         // Busy 16-bit TSS
#define STS_CG16        0x4         // 16-bit Call Gate
#define STS_TG          0x5         // Task Gate / Coum Transmitions
#define STS_IG16        0x6         // 16-bit Interrupt Gate
#define STS_TG16        0x7         // 16-bit Trap Gate
#define STS_T32A        0x9         // Available 32-bit TSS
#define STS_T32B        0xB         // Busy 32-bit TSS
#define STS_CG32        0xC         // 32-bit Call Gate
#define STS_IG32        0xE         // 32-bit Interrupt Gate
#define STS_TG32        0xF         // 32-bit Trap Gate

/* *
 * Set up a normal interrupt/trap gate descriptor
 *   - istrap: 1 for a trap (= exception) gate, 0 for an interrupt gate
 *   - sel: Code segment selector for interrupt/trap handler
 *   - off: Offset in code segment for interrupt/trap handler
 *   - dpl: Descriptor Privilege Level - the privilege level required
 *          for software to invoke this interrupt/trap gate explicitly
 *          using an int instruction.
 * */
#define SETGATE(gate, istrap, sel, off, dpl) {               \
        (gate).gd_off_15_0 = (uint32_t)(off) & 0xffff;      \
        (gate).gd_ss = (sel);                                \
        (gate).gd_args = 0;                                 \
        (gate).gd_rsv1 = 0;                                 \
        (gate).gd_type = (istrap) ? STS_TG32 : STS_IG32;    \
        (gate).gd_s = 0;                                    \
        (gate).gd_dpl = (dpl);                              \
        (gate).gd_p = 1;                                    \
        (gate).gd_off_31_16 = (uint32_t)(off) >> 16;        \
    }

/* Set up a call gate descriptor */
#define SETCALLGATE(gate, ss, off, dpl) {                   \
        (gate).gd_off_15_0 = (uint32_t)(off) & 0xffff;      \
        (gate).gd_ss = (ss);                                \
        (gate).gd_args = 0;                                 \
        (gate).gd_rsv1 = 0;                                 \
        (gate).gd_type = STS_CG32;                          \
        (gate).gd_s = 0;                                    \
        (gate).gd_dpl = (dpl);                              \
        (gate).gd_p = 1;                                    \
        (gate).gd_off_31_16 = (uint32_t)(off) >> 16;        \
    }

/* registers as pushed by pushal */
struct pushregs {
    uint32_t reg_edi;
    uint32_t reg_esi;
    uint32_t reg_ebp;
    uint32_t reg_oesp;          /* Useless */
    uint32_t reg_ebx;
    uint32_t reg_edx;
    uint32_t reg_ecx;
    uint32_t reg_eax;
};

struct trapframe {
    struct pushregs tf_regs;
    uint16_t tf_gs;
    uint16_t tf_padding0;
    uint16_t tf_fs;
    uint16_t tf_padding1;
    uint16_t tf_es;
    uint16_t tf_padding2;
    uint16_t tf_ds;
    uint16_t tf_padding3;
    uint32_t tf_trapno;
    /* below here defined by x86 hardware */
    uint32_t tf_err;
    uintptr_t tf_eip;
    uint16_t tf_cs;
    uint16_t tf_padding4;
    uint32_t tf_eflags;
    /* below here only when crossing rings, such as from user to kernel */
    uintptr_t tf_esp;
    uint16_t tf_ss;
    uint16_t tf_padding5;
} __attribute__((packed));

void idt_init();
void print_trapframe(struct trapframe *tf);
void print_regs(struct pushregs *regs);
bool trap_in_kernel(struct trapframe *tf);

#endif /* !OS_TRAP_TRAP_H */

