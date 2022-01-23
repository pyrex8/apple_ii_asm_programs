
ALL_C = $(wildcard *.c) $(wildcard *.h)

all: $(ALL_C)
	cl65 -Oirs -Cl -v -W error -t apple2 -C apple2-hgr2.cfg -o prelude_to_sabotage.bin -l prelude_to_sabotage.list prelude_to_sabotage.c

clean:
	rm -f *.bin *.list *.o
