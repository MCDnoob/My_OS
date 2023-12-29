
/* Support for reading the NVRAM from the real-time clock. */
#include <x86.h>
#include <rtc.h>

unsigned
mc146818_read(unsigned reg)
{
	outb(IO_RTC, reg);
	return inb(IO_RTC+1);
}

void
mc146818_write(unsigned reg, unsigned datum)
{
	outb(IO_RTC, reg);
	outb(IO_RTC+1, datum);
}

int nvram_read(int r)
{
	return mc146818_read(r) | (mc146818_read(r + 1) << 8);
}
