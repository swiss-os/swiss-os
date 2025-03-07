:<<"::CMDGOTO"
@echo off
goto :CMDENTRY
::CMDGOTO

echo "========== Linux ${SHELL} ================="

./parted.exe --boot ./mbr.bin  mkpart 0x80,0x0c,32,95641 ./32gears.vhd
./mkdosfs.exe --boot ./vbr.bin  --blocks 47820 --offset 32  -n "OS-3o3" ./32gears.vhd	
cp ../src/i386/32gears.vmx  ./32gears.vmx
cp ../src/amd64/64gears.vmx  ./64gears.vmx
cp ../non-free/secureboot/vmware.org vmware.vmx

./vhd_cp.exe ../bin/io.sys vhd://../bin/32gears.vhd:/io.sys 


./vhd_cp.exe ../non-free/rpifirmware/boot/LICENSE.broadcom vhd://../bin/32gears.vhd:/LICENSE.broadcom
./vhd_cp.exe ../non-free/rpifirmware/boot/COPYING.linux vhd://../bin/32gears.vhd:/COPYING.linux
./vhd_cp.exe ../non-free/rpifirmware/boot/bootcode.bin vhd://../bin/32gears.vhd:/bootcode.bin
./vhd_cp.exe ../non-free/rpifirmware/boot/start4db.elf vhd://../bin/32gears.vhd:/start4db.elf
./vhd_cp.exe ../non-free/rpifirmware/boot/fixup4db.dat vhd://../bin/32gears.vhd:/fixup4db.dat
./vhd_cp.exe ../src/rpi400/cmdline.txt vhd://../bin/32gears.vhd:/cmdline.txt
./vhd_cp.exe ../non-free/rpifirmware/boot/overlays/disable-bt.dtbo vhd://../bin/32gears.vhd:/overlays/disable-bt.dtbo
./vhd_cp.exe ../non-free/rpifirmware/boot/overlays/uart0.dtbo vhd://../bin/32gears.vhd:/overlays/uart0.dtbo
./vhd_cp.exe ../non-free/rpifirmware/boot/overlays/uart1.dtbo vhd://../bin/32gears.vhd:/overlays/uart1.dtbo
	
./vhd_cp.exe ../src/rpi400/config.txt vhd://../bin/32gears.vhd:/config.txt
./vhd_cp.exe ../bin/kernel8.img vhd://../bin/32gears.vhd:/kernel8.img


./vhd_cp.exe ../bin/bootx64.efi vhd://../bin/32gears.vhd:/efi/boot/grubx64_real.efi
./vhd_cp.exe ../non-free/secureboot/grub.efi vhd://../bin/32gears.vhd:/efi/boot/grub.efi
./vhd_cp.exe ../non-free/secureboot/BOOTX64.EFI vhd://../bin/32gears.vhd:/efi/boot/BOOTX64.EFI
./vhd_cp.exe ../non-free/secureboot/MokManager.efi vhd://../bin/32gears.vhd:/efi/boot/MokManager.efi
./vhd_cp.exe ../non-free/secureboot/COPYING vhd://../bin/32gears.vhd:/COPYING.ventoy
./vhd_cp.exe ../non-free/secureboot/ENROLL_THIS_KEY_IN_MOKMANAGER.cer vhd://../bin/32gears.vhd:/ENROLL_THIS_KEY_IN_MOKMANAGER.cer

exit $?
:CMDENTRY

echo ============= Windows %COMSPEC% ============

.\parted --boot .\mbr.bin  mkpart 0x80,0x0b,32,95641 .\32gears.vhd
.\mkdosfs --boot .\vbr.bin  --blocks 47820 --offset 32  -n "OS-3o3" .\32gears.vhd	

if exist .\32gears.vhd (
	echo vhd...
) else (
	exit -1
)
copy ..\src\i386\32gears.vmx  .\32gears.vmx
copy ..\src\amd64\64gears.vmx  .\64gears.vmx

.\vhd_cp ../bin/io.sys vhd://../bin/32gears.vhd:/io.sys 
.\vhd_cp ../bin/bootx64.efi vhd://../bin/32gears.vhd:/efi/boot/bootx64.efi
.\vhd_cp ../bin/bootia32.efi vhd://../bin/32gears.vhd:/efi/boot/bootia32.efi


.\vhd_cp ../non-free/rpifirmware/boot/LICENSE.broadcom vhd://../bin/32gears.vhd:/LICENSE.broadcom
.\vhd_cp ../non-free/rpifirmware/boot/COPYING.linux vhd://../bin/32gears.vhd:/COPYING.linux
.\vhd_cp ../non-free/rpifirmware/boot/bootcode.bin vhd://../bin/32gears.vhd:/bootcode.bin
.\vhd_cp ../non-free/rpifirmware/boot/start4db.elf vhd://../bin/32gears.vhd:/start4db.elf
.\vhd_cp ../non-free/rpifirmware/boot/fixup4db.dat vhd://../bin/32gears.vhd:/fixup4db.dat
.\vhd_cp ../src/rpi400/cmdline.txt vhd://../bin/32gears.vhd:/cmdline.txt
.\vhd_cp ../non-free/rpifirmware/boot/overlays/disable-bt.dtbo vhd://../bin/32gears.vhd:/overlays/disable-bt.dtbo
.\vhd_cp ../non-free/rpifirmware/boot/overlays/uart0.dtbo vhd://../bin/32gears.vhd:/overlays/uart0.dtbo
.\vhd_cp ../non-free/rpifirmware/boot/overlays/uart1.dtbo vhd://../bin/32gears.vhd:/overlays/uart1.dtbo
	
.\vhd_cp ../src/rpi400/config.txt vhd://../bin/32gears.vhd:/config.txt
.\vhd_cp ../bin/kernel8.img vhd://../bin/32gears.vhd:/kernel8.img

exit 0


