
OBJDIR := obj
OBJDIRS :=

TOOLPREFIX := 

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
NM	:= $(TOOLPREFIX)nm
CFLAGS = -fno-pic -static -fno-builtin -ggdb -gstabs -Wall -m32 -nostdinc -fno-stack-protector -MD
LDFLAGS = -m elf_i386 -nostdlib

# Make sure that 'all' is the first target
all:

clean:
	rm -rf $(OBJDIR)

######## for bootloader build
OBJDIRS += boot
BOOT_OBJS := $(OBJDIR)/boot/boot.o $(OBJDIR)/boot/main.o

$(OBJDIR)/boot/%.o: boot/%.c
	@echo + cc -O0 $<
	@mkdir -p $(@D)
	$(CC)  $(CFLAGS) -Isys/ -Os -c -o $@ $<

$(OBJDIR)/boot/%.o: boot/%.S
	@echo + as $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -Isys/ -c -o $@ $<

$(OBJDIR)/bin/boot: $(BOOT_OBJS)
	@echo + ld bin/boot
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 -o $@.out $^
	$(OBJDUMP) -S $@.out >$@.asm
	$(OBJCOPY) -S -O binary -j .text $@.out $@
	perl boot/sign.pl $@
	
######## for kernel build

OBJDIRS += kern

KERN_CFLAGS := $(CFLAGS)
KERN_LDFLAGS := $(LDFLAGS) -T kern/kernel.ld

KERN_INCLUDE	:= sys/ \

KERN_CFLAGS += $(addprefix -I,$(KERN_INCLUDE))

# entry.S must be first, so that it's the first code in the text segment!!!
#
# We also snatch the use of a couple handy source files
# from the lib directory, to avoid gratuitous code duplication.
KERN_SRCFILES := kern/entry.S \
		 kern/init.c \

# Only build files if they exist.
KERN_SRCFILES := $(wildcard $(KERN_SRCFILES))

KERN_OBJFILES := $(patsubst %.c, $(OBJDIR)/%.o, $(KERN_SRCFILES))
KERN_OBJFILES := $(patsubst %.S, $(OBJDIR)/%.o, $(KERN_OBJFILES))

# How to build kernel object files
$(OBJDIR)/kern/%.o: kern/%.c
	@echo + cc $<
	@mkdir -p $(@D)
	$(CC) $(KERN_CFLAGS) -c -o $@ $<

$(OBJDIR)/kern/%.o: kern/%.S
	@echo + as $<
	@mkdir -p $(@D)
	$(CC)  $(KERN_CFLAGS) -c -o $@ $<

# How to build the kernel itself
$(OBJDIR)/bin/kernel: $(KERN_OBJFILES)  kern/kernel.ld
	@echo + ld $@
	@mkdir -p $(@D)
	$(LD) -o $@ $(KERN_LDFLAGS) $(KERN_OBJFILES)
	$(OBJDUMP) -S $@ > $@.asm
	$(NM) -n $@ > $@.sym

# How to build the kernel disk image
$(OBJDIR)/bin/kernel.img: $(OBJDIR)/bin/kernel $(OBJDIR)/bin/boot
	@echo + mk $@
	dd if=/dev/zero of=$(OBJDIR)/bin/kernel.img~ count=10000 2>/dev/null
	dd if=$(OBJDIR)/bin/boot of=$(OBJDIR)/bin/kernel.img~ conv=notrunc 2>/dev/null
	dd if=$(OBJDIR)/bin/kernel of=$(OBJDIR)/bin/kernel.img~ seek=1 conv=notrunc 2>/dev/null
	mv $(OBJDIR)/bin/kernel.img~ $(OBJDIR)/bin/kernel.img

all: $(OBJDIR)/bin/kernel.img

QEMU := qemu-system-i386
IMAGES = $(OBJDIR)/bin/kernel.img
QEMUOPTS = -drive file=$(OBJDIR)/bin/kernel.img,index=0,media=disk,format=raw -serial mon:stdio -m 512

qemu: $(IMAGES)
	$(QEMU) $(QEMUOPTS)

qemu-gdb: $(IMAGES)
	$(QEMU) $(QEMUOPTS) -S -s

qemu-tgdb: $(IMAGES)
	$(QEMU) $(QEMUOPTS) -S -s  &
	sleep 2
	$(TERMINAL)  -e "gdb -q -x gdbinit"

# This magic automatically generates makefile dependencies
# for header files included from C source files we compile,
# and keeps those dependencies up-to-date every time we recompile.
# See 'mergedep.pl' for more information.
$(OBJDIR)/.deps: $(foreach dir, $(OBJDIRS), $(wildcard $(OBJDIR)/$(dir)/*.d))
	@mkdir -p $(@D)
	perl mergedep.pl $@ $^

-include $(OBJDIR)/.deps