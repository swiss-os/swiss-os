
all:
	echo run ./setup.sh

flash:
	dd if=disk.img of=/dev/sda bs=4096

clean:
	rm -f vmlinuz config disk.img disk.vhd grubx64_real.efi initramfs.img symvers.xz System.map

