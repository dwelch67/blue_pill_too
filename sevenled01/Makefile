
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m3
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all : sevenled01.bin

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

sevenled01.o : sevenled01.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m0 -march=armv6-m -c sevenled01.c -o sevenled01.o

sevenled01.bin : flash.ld flash.o sevenled01.o
	$(ARMGNU)-ld -o sevenled01.elf -T flash.ld flash.o sevenled01.o
	$(ARMGNU)-objdump -D sevenled01.elf > sevenled01.list
	$(ARMGNU)-objcopy sevenled01.elf sevenled01.bin -O binary

