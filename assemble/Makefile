main: rbtree.o assemble.o 
	gcc -o assemble rbtree.o assemble.o

rbtree.o: rbtree.c
	gcc -Wall -c rbtree.c -o rbtree.o

assemble.o: assemble.c
	gcc -Wall -c assemble.c -o assemble.o

clean:
	rm -f assemble assemble.o rbtree.o fifo.o
