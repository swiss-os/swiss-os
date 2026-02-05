CC=clang
BUILDDIR=.
SOURCEDIR=../src

LDLIBS= 
CFLAGS= -Wall \
                -ansi \
                -D__AMD64__ \
                -D__EFI__ \
                -target \
		x86_64-unknown-windows \
                -ffreestanding \
		-nostdinc \
                -mno-red-zone \
                -MMD 

SRCS= \
                ../src/klib.c \
                ../src/efios.c \
                ../src/efi64.c \
                ../src/term.c 

OBJS=$(subst $(SOURCEDIR),$(BUILDDIR),$(SRCS:.c=.o))


all: bootx64.efi 
	@echo "done"

bootx64.efi: bootx64.o $(OBJS)
	lld-link -map:bootx64.map -entry:efi_main -nodefaultlib:libcmt -nodefaultlib:oldnames -subsystem:efi_application -out:$@ $^ 


bootx64.o:  ../src/bootx64.S
	$(CC) -c $(CFLAGS) $< -o $@


./%.o:  $(SOURCEDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@




