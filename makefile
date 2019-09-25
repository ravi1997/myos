GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fno-pie -ggdb -Wall -Wextra -Iinclude -O3 -std=c++17
ASPARAMS = --32

OBJECTS = Object/kernel/loader.o \
					Object/kernel/system/iqr.o \
					Object/kernel/kernel.o

Object/%.o:source/%.cpp
	g++ $(GPPPARAMS) -o $@ -c $<

Object/%.o : source/%.s
	as $(ASPARAMS) -o $@ $<

Object/kernel/mykernel.bin: linker.ld $(OBJECTS)
	ld $(LDPARAMS) -T $< -o $@ $(OBJECTS)

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
	(taskkill /IM VirtualBoxVM.exe /F && sleep 1)||true
	VirtualBoxVM --dbg --startvm "My Operating System" &

.PHONY:clean
clean:
	(rm -rf iso) || true
	(rm $(OBJECTS) Object/kernel/*.bin) || true
