main: rbtree.o assemble.o
	gcc -o assemble rbtree.o assemble.o

rbtree.o: rbtree.c
	gcc -c rbtree.c -o rbtree.o

assemble.o: assemble.c
	gcc -c assemble.c -o assemble.o

clean:
	rm -rf assemble assemble.o rbtree.o
