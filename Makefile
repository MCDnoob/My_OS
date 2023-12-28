
OBJDIR := obj
OBJDIRS :=

TOOLPREFIX := 

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
NM	:= $(TOOLPREFIX)nm
CFLAGS = -fno-pic -static -fno-builtin -ggdb -gstabs -Wall -m32 -Werror -nostdinc -fno-stack-protector -MD
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
	$(CC)  $(CFLAGS) -O0 -c -o $@ $<

$(OBJDIR)/boot/%.o: boot/%.S
	@echo + as $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/bin/boot: $(BOOT_OBJS)
	@echo + ld bin/boot
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 -o $@.out $^
	$(OBJDUMP) -S $@.out >$@.asm
	$(OBJCOPY) -S -O binary -j .text $@.out $@
	perl boot/sign.pl $@

boot: $(OBJDIR)/bin/boot
	

# How to build the boot disk image
$(OBJDIR)/bin/boot.img: $(OBJDIR)/bin/boot
	@echo + mk $@
	dd if=/dev/zero of=$(OBJDIR)/bin/boot.img~ count=10000 2>/dev/null
	dd if=$(OBJDIR)/bin/boot of=$(OBJDIR)/bin/boot.img~ conv=notrunc 2>/dev/null
	mv $(OBJDIR)/bin/boot.img~ $(OBJDIR)/bin/boot.img

all: $(OBJDIR)/bin/boot.img

QEMU := qemu-system-i386
IMAGES = $(OBJDIR)/bin/boot.img
QEMUOPTS = -drive file=$(OBJDIR)/bin/boot.img,index=0,media=disk,format=raw -serial mon:stdio -m 512

qemu: $(IMAGES)
	$(QEMU) $(QEMUOPTS)
