ALL_C = $(wildcard *.c) $(wildcard *.h)

# -C apple2-hgr2.cfg
# --dump-config apple2

all: $(ALL_C)
	cl65 -v -W error -t apple2 -C apple2-hgr2.cfg -o hello.bin -l hello.list hello.c

clean:
	rm -f *.bin *.list *.o
