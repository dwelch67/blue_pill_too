
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m3
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all : sevenled03.bin

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

sevenled03.o : sevenled03.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m0 -march=armv6-m -c sevenled03.c -o sevenled03.o

sevenled03.bin : flash.ld flash.o sevenled03.o
	$(ARMGNU)-ld -o sevenled03.elf -T flash.ld flash.o sevenled03.o
	$(ARMGNU)-objdump -D sevenled03.elf > sevenled03.list
	$(ARMGNU)-objcopy sevenled03.elf sevenled03.bin -O binary

