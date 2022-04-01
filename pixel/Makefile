ALL_C = $(wildcard *.c) $(wildcard *.h)

all: $(ALL_C)
	cl65 -v -W error -t apple2 -C apple2-hgr2.cfg -o pixel.bin -l pixel.list pixel.c

clean:
	rm -f *.bin *.list *.o
