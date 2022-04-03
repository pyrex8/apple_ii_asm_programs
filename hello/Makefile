TARGET = $(notdir $(shell pwd))

all:
	ca65 $(TARGET).asm -D BASE=0x0803
	cl65 $(TARGET).o --target none --start-addr 0x0803 -o $(TARGET).bin

clean:
	rm -f *.bin *.o
