
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m3
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all : sevenled02.bin

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

sevenled02.o : sevenled02.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m0 -march=armv6-m -c sevenled02.c -o sevenled02.o

sevenled02.bin : flash.ld flash.o sevenled02.o
	$(ARMGNU)-ld -o sevenled02.elf -T flash.ld flash.o sevenled02.o
	$(ARMGNU)-objdump -D sevenled02.elf > sevenled02.list
	$(ARMGNU)-objcopy sevenled02.elf sevenled02.bin -O binary

