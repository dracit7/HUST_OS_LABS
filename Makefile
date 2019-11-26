
LIB = lib/log.c lib/log.h
OBJ = obj/log.o
CFLAGS = -I.

lab1: lib src/lab1.c
	@gcc $(CFLAGS) src/lab1.c $(OBJ) -o bin/lab1
	@bin/lab1

lib: $(LIB)
	@gcc $(CFLAGS) -c lib/log.c -o obj/log.o

clean:
	rm -f *~
	rm -f obj/*.o
	rm -f bin/*