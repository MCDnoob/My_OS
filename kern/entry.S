#include <mmu.h>

.text

.globl		_start
_start:
	#movw	$0x1234,0x472			# warm boot

	# Clear the frame pointer register (EBP)
	# so that once we get into debugging C code,
	# stack backtraces will be terminated properly.
	movl	$0x0,%ebp			# nuke frame pointer

	# Set the stack pointer
	movl	$(bootstacktop),%esp

	# now to C code
	call	kern_init

	# Should never get here, but in case we do, just spin.
spin:	jmp	spin

.data
# boot stack
	.p2align	PGSHIFT		# force page alignment
	.globl		bootstack
bootstack:
	.space		KSTKSIZE
	.globl		bootstacktop
bootstacktop:
