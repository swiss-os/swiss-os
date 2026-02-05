CC=clang
BUILDDIR=.

LDLIBS= 
CFLAGS= -Wall \
                -ansi \
                -D__I386__ \
                -D__EFI__ \
                -target \
                i686-unknown-windows \
                -m32 \
                -nostdlib \
                -nostdinc \
                -nostartfiles \
                -fno-builtin \
                -fuse-ld=lld \
                -mno-red-zone \
                -Wl,-subsystem:efi_application,-entry:efi_main 

SRCS= \
                ../src/i386/bootia32.S \
                ../src/i386/efi32.c \
                ../src/klib.c \
                ../src/term.c

all: bootia32.efi 
	@echo "done"

bootia32.efi: $(SRCS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@



