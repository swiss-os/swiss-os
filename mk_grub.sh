
  MODULES="configfile ext2 fat part_gpt normal
  linux ls boot echo reboot search search_fs_file search_fs_uuid
part_msdos
  search_label help"


../grub/grub-mkimage -v -O x86_64-efi -p "" \
  -d ../grub/grub-core/ \
  -o ./grubx64.efi  -c ./grub.cfg $MODULES
          
