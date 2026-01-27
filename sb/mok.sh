#! /bin/sh

echo Registering Secure Boot Machine Owner Key
echo -ne "\n\n\n\n"

logger -t "mok.sh" "1"
insmod /cdrom/dists/32Gears/efivarfs.ko
logger -t "mok.sh" "2"
mount -t efivarfs none /sys/firmware/efi/efivars
echo -ne "."
logger -t "mok.sh" "3"

sleep 2
logger -t "mok.sh" "4"
if ! /cdrom/dists/32Gears/mokutil --test-key /cdrom/passwd_ok.der ; then
	(echo -ne "ok\n"; sleep 1; echo -ne "ok\n"; sleep 1) | /cdrom/dists/32Gears/mokutil --delete /cdrom/passwd_ok.der
	logger -t "mok.sh" "5 delete MOK"
else 
	(echo -ne "ok\n"; sleep 1; echo -ne "ok\n";sleep 3) | /cdrom/dists/32Gears/mokutil --import /cdrom/passwd_ok.der
	logger -t "mok.sh" "6 import MOK"
fi

logger -t "mok.sh" "7"
echo -ne "."
 

echo done
echo -ne "\n\n\n\n"
sync

logger -t "mok.sh" "DONE!!!"

sleep 8
/sbin/reboot -nf
/sbin/shutdown -now

