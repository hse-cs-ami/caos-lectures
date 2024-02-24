EXE=bare bare_c mycat argcat binpatch

all: $(EXE)

bare: bare.S
	gcc -m32 -static -nostdlib bare.S -o bare

bare_c: bare.c
	gcc -m32 -static -nostdlib bare.c -o bare_c

CFLAGS=-m32

clean:
	rm -f $(EXE)
