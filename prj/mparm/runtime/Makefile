KERNEL_SWARM=../toolchain/uclinux-micrel/uClinux-dist/linux-2.4.x/linux
KERNEL_GDB=/usr/local/uClinux-dist/linux-2.4.x/linux

ROMFS_BASE = bin etc
ROMFS_DIRS = dev home lib mnt proc usr var
BUSYBOX_ALIAS = mount umount

all: TargetMem_1.mem

clean:
	for i in $(ROMFS_DIRS); do $(RM) -r romfs/$$i ; done
	for i in $(BUSYBOX_ALIAS); do $(RM) romfs/bin/$$i ; done
	$(RM) romfs/etc/version romfs/tmp romfs/sbin
	$(RM) *.img *.mem *.rom *~ stats*.txt

config-romfs:
	for i in $(ROMFS_DIRS); do [ -d romfs/$$i ] || mkdir -p romfs/$$i; done
	for i in $(BUSYBOX_ALIAS); do ln romfs/bin/busybox romfs/bin/$$i ; done
	for i in `cat devices.txt` ; do touch romfs/dev/@$$i ; done
	$(RM) romfs/tmp romfs/sbin
	ln -sf /var/tmp romfs/tmp
	ln -sf /bin romfs/sbin
	date -R > romfs/etc/version

romfs: config-romfs
	genromfs -v -V "ROMdisk" -f romfs.img -d romfs

TargetMem_1.mem: romfs
	BSS=`arm-elf-objdump --headers $(KERNEL_SWARM) | grep .bss` ; \
	BSSADDR=`set -- $${BSS} ; echo 0x$${4}` ; \
	BSSSIZE=`set -- $${BSS} ; echo 0x$${3}` ; \
	ADDR=`printf "0x%x" $$(( $${BSSADDR} + $${BSSSIZE} )) ` ; \
	echo "BSS=$${BSSADDR},$${BSSSIZE} ADDR=$${ADDR}" ; \
	arm-elf-objcopy --add-section=.romfs=romfs.img \
		--adjust-section-vma=.romfs=$${ADDR} --no-adjust-warnings \
		--set-section-flags=.romfs=alloc,load,data   \
		$(KERNEL_SWARM) image-swarm.img 2> /dev/null
	arm-elf-objcopy -O binary image-swarm.img TargetMem_1.mem

boot.rom: romfs
	arm-elf-objcopy -O binary --remove-section=.romvec \
			--remove-section=.text --remove-section=.ramvec \
			--remove-section=.init \
			--remove-section=.bss --remove-section=.eram \
			$(KERNEL_GDB) image-gdb-data.img
	arm-elf-objcopy -O binary --remove-section=.ramvec \
			--remove-section=.bss --remove-section=.data \
			--remove-section=.eram \
			--set-section-flags=.romvec=CONTENTS,ALLOC,LOAD,READONLY,CODE \
			$(KERNEL_GDB) image-gdb-text.img
	cat image-gdb-text.img image-gdb-data.img romfs.img > image-gdb.mem
	ln -sf romfs.img boot.rom

gdb: boot.rom
	arm-elf-gdb $(KERNEL_GDB)

