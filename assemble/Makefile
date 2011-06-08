main: rbtree.o assemble.o list.o
	gcc -o assemble rbtree.o assemble.o list.o

rbtree.o: rbtree.c
	gcc -Wall -c rbtree.c -o rbtree.o

assemble.o: assemble.c
	gcc -Wall -c assemble.c -o assemble.o

list.o: list.c
	gcc -Wall -c list.c -o list.o

clean:
	rm -f assemble assemble.o rbtree.o list.o
