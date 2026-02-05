
all:
	echo run ./setup.sh

run:
	(cd bin; make -f bootx64.efi.make)
	(cd bin; ../setup.sh)

flash:
	dd if=disk.img of=/dev/sda bs=4096

clean:
	(cd bin; rm -f *.efi *.d *.o *.map *.vhd *.img)

