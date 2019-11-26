
LIB = lib/log.c lib/log.h lib/sem.c lib/sem.h
OBJ = obj/log.o obj/sem.o
CFLAGS = -I. -lpthread

lab1: lib src/lab1.c
	@gcc $(CFLAGS) src/lab1.c $(OBJ) -o bin/lab1
	@bin/lab1

lab2: lib src/lab2.c
	@gcc $(CFLAGS) src/lab2.c $(OBJ) -o bin/lab2
	@bin/lab2

lib: $(LIB)
	@gcc $(CFLAGS) -c lib/log.c -o obj/log.o
	@gcc $(CFLAGS) -c lib/sem.c -o obj/sem.o

clean:
	rm -f *~
	rm -f obj/*.o
	rm -f bin/*