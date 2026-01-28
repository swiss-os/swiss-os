sudo cp /boot/*-`uname -r`* .
sudo chmod a+rw *-`uname -r`*
mv -f config-* config
mv -f vmlinuz-* vmlinuz
mv -f initramfs-* initramfs.img
mv -f symvers-* symvers.xz
mv -f System.map-* System.map

sudo chmod a+rw grub*.efi

rm -rf cpio 
mkdir -p cpio && cd cpio || exit -1;
cat ../initramfs.img | cpio -idmv
find . -print0 | cpio --null -ov --format=newc > ../initramfs.img.new
cd ..

#if false; then
if true; then
rm -rf ram 
mkdir -p ram && cd ram || exit -1;
/usr/lib/dracut/skipcpio ../initramfs.img | zstdcat | cpio -idmv
find . -print > ../initramfs.ls.txt
cp -rf ../new/* .
cd .. 
fi

cd ram || exit -1
find . -print0 | cpio --null -ov --format=newc | zstd >> ../initramfs.img.new
cd .. 


dd if=/dev/zero of=disk.img bs=1024k seek=7000 count=0
sudo parted -s -f disk.img mklabel msdos

/sbin/fdisk ./disk.img <<END
o
n
p
1
2048
14333952
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

mkdosfs -n OS303 --offset 2048 disk.img

mkdir -p ./mnt

guestmount -a ./disk.img -m /dev/sda1 mnt
cd mnt
mkdir EFI
mkdir EFI/boot
mkdir EFI/debian
mkdir boot
mkdir grub
mkdir boot/grub
mkdir boot/grub2
cp ../config boot/
cp ../vmlinuz boot/
cp ../initramfs.img.new boot/initramfs.img
cp ../symvers.xz boot/
cp ../System.map boot/

echo `uname -r` > boot/UPSTREAM.txt

cp ../sb/COPYING EFI/boot/COPYING.ventoy
cp ../sb/grub.efi ../sb/BOOTX64.EFI ../sb/MokManager.efi EFI/boot/
cp ../grubx64.efi EFI/boot/grubx64_real.efi 
#cp ../grub.cfg grub/
cp ../grub.cfg EFI/boot/
cp ../grub.cfg EFI/debian/
#cp ../grub.cfg boot/grub/
#cp ../grub.cfg boot/grub2/
cp ../sb/ENROLL_THIS_KEY_IN_MOKMANAGER.cer .
cd ..
guestunmount mnt 

sync
sleep 1
qemu-img convert -f raw -O vpc disk.img disk.vhd

qemu-system-x86_64 -drive file=disk.vhd -bios /usr/share/edk2/ovmf/OVMF_CODE.fd -m 4096M -monitor vc:1024x768

