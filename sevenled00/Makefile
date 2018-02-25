
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m3
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all : sevenled00.bin

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.bc
	rm -f *.opt.s
	rm -f *.norm.s
	rm -f *.hex

#---------------------------------

flash.o : flash.s
	$(ARMGNU)-as $(AOPS) flash.s -o flash.o

sevenled00.o : sevenled00.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m0 -march=armv6-m -c sevenled00.c -o sevenled00.o

sevenled00.bin : flash.ld flash.o sevenled00.o
	$(ARMGNU)-ld -o sevenled00.elf -T flash.ld flash.o sevenled00.o
	$(ARMGNU)-objdump -D sevenled00.elf > sevenled00.list
	$(ARMGNU)-objcopy sevenled00.elf sevenled00.bin -O binary

