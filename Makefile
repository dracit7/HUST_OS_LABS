
LIB = lib/log.c lib/log.h \
			lib/sem.c lib/sem.h \
			lib/buffer.c lib/buffer.h \
			lib/ds.c lib/ds.h
OBJ = obj/log.o obj/sem.o obj/buffer.o obj/ds.o
CFLAGS = -I. -lpthread

all: lab1 lab2 lab3
	@echo all set.

lab1: lib src/lab1.c
	@gcc $(CFLAGS) src/lab1.c $(OBJ) -o bin/lab1

lab2: lib src/lab2.c
	@gcc $(CFLAGS) src/lab2.c $(OBJ) -o bin/lab2

lab3: lib src/lab3.c
	@gcc $(CFLAGS) src/lab3.c $(OBJ) -o bin/lab3

lab4: lib src/lab4.c
	@gcc $(CFLAGS) src/lab4.c $(OBJ) -o bin/lab4

lib: $(LIB)
	@gcc $(CFLAGS) -c lib/log.c -o obj/log.o
	@gcc $(CFLAGS) -c lib/sem.c -o obj/sem.o
	@gcc $(CFLAGS) -c lib/buffer.c -o obj/buffer.o
	@gcc $(CFLAGS) -c lib/ds.c -o obj/ds.o

clean:
	rm -f *~
	rm -f obj/*.o
	rm -f bin/*