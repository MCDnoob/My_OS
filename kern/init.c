#include <stdio.h>
#include <string.h>
#include <console.h>
#include <pmm.h>
#include <trap.h>

void monitor(void)
{
	char *buf;

	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			cprintf("get buf:%s.\n", buf);
	}
}

void kern_init(void)
{
	extern char edata[], end[];

	// Before doing anything else, complete the ELF loading process.
	// Clear the uninitialized global data (BSS) section of our program.
	// This ensures that all static/global variables start out zero.
	memset(edata, 0, end - edata);

	// Initialize the console.
	// Can't call cprintf until after we do this!
	cons_init();

	cprintf("wind_os version %d running!\n", 1);

	pmm_init();

	idt_init();

	monitor();

}
