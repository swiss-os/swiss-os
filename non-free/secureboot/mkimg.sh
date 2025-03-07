

#cp ../../bin/io.sys setup.bin
#sbsign --key MOK.priv --cert MOK.pem setup.bin --output setup.bin.signed
sbsign --key MOK.priv --cert MOK.pem ../../bin/bootx64.efi --output 32gx64.efi.signed
#cp ../../bin/bootx64.efi 32gx64.efi.signed

7z -y x ~/Downloads/debian-12.9.0-amd64-netinst.iso EFI 
7z -y x ~/Downloads/debian-12.9.0-amd64-netinst.iso install.amd
7z -y e ~/Downloads/debian-12.9.0-amd64-netinst.iso dists/bookworm/main/binary-amd64/Release
7z -y e ~/Downloads/debian-12.9.0-amd64-netinst.iso dists/bookworm/main/binary-amd64/Packages.gz


#(cp S60startup initrd/lib/debian-installer.d/S60start-startup) && \
mkdir -p initrd
false && (zcat install.amd/initrd.gz | fakeroot cpio -D initrd -iudm ) && \
(cp cdrom-detect.postinst initrd/var/lib/dpkg/info/) && \
(cp S38lowmemwarn initrd/lib/debian-installer-startup.d/) && \
(cp S59mok initrd/lib/debian-installer-startup.d/S59mok) && \
(cd initrd && find . |  cpio  -o -H newc | gzip -9 > ../initrd.gz)
rm -rf initrd

dd if=/dev/zero of=secure.raw bs=100 count=489856
dd bs=512 seek=95674 skip=95674 count=1 conv=notrunc if=../../bin/32gears.vhd of=secure.raw

/sbin/fdisk secure.raw <<END
o
n
p
1
2048
95674
t
c
a
x
i
1
r
p
w

END

mformat -i secure.raw@@2048s -N 33 -H 2048 -F -v "OS-3o3 SB"

#mkdir tmp
#mcopy -i ~/Downloads/ventoy.disk.img -s ::/ tmp/
#mcopy -i secure.raw@@2048s -s tmp/* ::/
#qemu-img convert -f raw -O vpc secure.raw ../../bin/secure.vhd
#exit

mmd -i secure.raw@@2048s ::EFI
mmd -i secure.raw@@2048s ::EFI/boot
mmd -i secure.raw@@2048s ::EFI/debian
mmd -i secure.raw@@2048s ::install.amd
mmd -i secure.raw@@2048s ::.disk
mmd -i secure.raw@@2048s ::dists
mmd -i secure.raw@@2048s ::grub
mmd -i secure.raw@@2048s ::dists/32Gears
mmd -i secure.raw@@2048s ::dists/32Gears/main
mmd -i secure.raw@@2048s ::dists/32Gears/main/binary-amd64
mmd -i secure.raw@@2048s ::dists/32Gears/main/debian-installer
mmd -i secure.raw@@2048s ::dists/32Gears/main/debian-installer/binary-amd64

#mcopy -i secure.raw@@2048s disk ::efi/boot/os-3o3.txt
mcopy -i secure.raw@@2048s ../../bin/bootx64.efi ::efi/boot/not_x64.efi
#mcopy -i secure.raw@@2048s EFI/boot/bootx64.efi ::efi/boot/bootx64.efi
#mcopy -i secure.raw@@2048s   grubx64_real.efi grub.efi BOOTX64.EFI MokManager.efi ::efi/boot/
mcopy -i secure.raw@@2048s   grub.efi BOOTX64.EFI MokManager.efi ::efi/boot/
#mcopy -i secure.raw@@2048s mmx64.efi.signed ::efi/boot/mmx64.efi
#mcopy -i secure.raw@@2048s shimx64.efi.signed ::efi/boot/shimx64.efi
mcopy -i secure.raw@@2048s 32gx64.efi.signed ::efi/boot/32gx64.efi
mcopy -i secure.raw@@2048s ../../bin/bootx64.efi ::efi/boot/grubx64_real.efi
mcopy -i secure.raw@@2048s grub.cfg ::efi/debian/grub.cfg
mcopy -i secure.raw@@2048s grub.cfg ::grub/grub.cfg
mcopy -i secure.raw@@2048s install.amd/vmlinuz ::install.amd/vmlinuz
mcopy -i secure.raw@@2048s initrd.gz ::install.amd/initrd.gz
mcopy -i secure.raw@@2048s info ::.disk/info
mcopy -i secure.raw@@2048s info ::.disk/base_installable
mcopy -i secure.raw@@2048s Release.txt ::dists/32Gears/Release

mcopy -i secure.raw@@2048s Release ::dists/32Gears/main/binary-amd64/Release
mcopy -i secure.raw@@2048s info ::dists/32Gears/main/binary-amd64/Packages.gz
mcopy -i secure.raw@@2048s info ::dists/32Gears/main/debian-installer/binary-amd64/Packages.gz
mcopy -v -i secure.raw@@2048s mokutil efivarfs.ko MOK.der MOV.pem MOK.priv mok.sh ::dists/32Gears/
#mcopy -v -i secure.raw@@2048s MOK.der ::/passwd_ok.der
mcopy -v -i secure.raw@@2048s ENROLL_THIS_KEY_IN_MOKMANAGER.cer ::/

mdir -i secure.raw@@2048s ::/grub
mdir -i secure.raw@@2048s ::/EFI/boot/

qemu-img convert -f raw -O vpc secure.raw ../../bin/secure.vhd

rm -f ~/.vmware/inventory.vmls

