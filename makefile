GPPPARAMS = -m32 -g -fno-use-cxa-atexit -nostdlib -nodefaultlibs -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fno-pie -Wall -Wextra -Iinclude -w -trigraphs -fconcepts -fno-stack-protector -O0 -c -std=c++2a -fno-common -ffreestanding -nostdinc -fno-strict-aliasing -fno-stack-protector -fno-omit-frame-pointer
ASPARAMS =  --32 -g
LDPARAMS = -melf_i386 -static -g

AS=as
CC=g++
LD=ld

OBJECTS = Object/kernel/loader.o \
					Object/kernel/system/iqr.o \
					Object/kernel/system/interruptstubs.o \
					Object/kernel/kernel.o	\
					Object/kernel/system/outputstream.o

Object/%.o:source/%.cpp
	$(CC) $(GPPPARAMS) -o $@ -c $<

Object/%.o : source/%.s
	$(AS) $(ASPARAMS) -o $@ $<

Object/kernel/mykernel.bin: linker.ld $(OBJECTS)
	$(LD) $(LDPARAMS) -T $< -o $@ $(OBJECTS)

mykernel.iso: Object/kernel/mykernel.bin
	(mkdir iso)
	(mkdir iso/boot)
	(mkdir iso/boot/grub)
	(cp $< iso/boot/)
	echo 'set timeout=0' >> iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

run:clean mykernel.iso
ifeq ($(echo $(VBoxManage.exe list runningvms | grep "My Operating System")),)
	 VirtualBoxVM.exe --startvm "My Operating System"
else
	VBoxManage.exe controlvm --debug "My Operating System" poweroff soft
	 VirtualBoxVM.exe --startvm "My Operating System"
endif

rebuild:clean mykernel.iso

objdump:clean Object/kernel/mykernel.bin
	objdump  -e -D -x -S -s -g -t Object/kernel/mykernel.bin -Mintel,i386 -m i386 > obj.dump

install:
    sudo apt-update -y || sudo apt upgrade || sudo apt-get intall as gcc g++ make grub-lagacy ld -y --fix-missing


.PHONY:clean
clean:
	(rm -rf iso) || true
	(rm $(OBJECTS) *.iso Object/kernel/mykernel.bin) || true
