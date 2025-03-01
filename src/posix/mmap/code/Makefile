CFLAGS=-g

dyn: dyn.c libfun.so fun.h
	gcc dyn.c -o dyn -L. -lfun

libfun.so: fun.c fun.h
	gcc -shared fun.c -o libfun.so

dlopen: dlopen.c
	gcc dlopen.c -o dlopen -ldl