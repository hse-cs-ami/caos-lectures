all: greeter

greeter: main.o libgreet.so
	$(CC) $< -o $@ -L. -lgreet

libgreet.so: greet.c
	$(CC) -fPIC $< -shared -o $@
