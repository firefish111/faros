.DEFAULT_GOAL := os.bin
.PHONY: qemu bochs clean

boot.bin: boot32.asm
	nasm $^ -f bin -o $@

entry.o: entry.asm
	nasm $^ -f elf -o $@

kernel.o: kernel.c $(wildcard *.h)
	gcc -ffreestanding -m32 -c $< -o $@

kernel.entry.o: link.ld entry.o kernel.o
	ld -o $@ -melf_i386 -T $^

kernel.bin: kernel.entry.o
	objcopy -O binary $^ $@

os.bin: boot.bin kernel.bin
	cat $^ > $@

qemu: os.bin
	$@-system-i386 -fda $< -boot a

bochs: os.bin .bochsrc
	$@ -q

clean:
	rm -f *.bin *.o