#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbtree.h"


void writeFile( char *filename, int *instructions, int fileSize ) {
	FILE *fp = fopen(filename,"wb");
	fwrite(instructions, sizeof(int), fileSize, fp);
	fclose(fp);
}



int main(int argc, char *argv[]) {
	printf("%d\n",argc);	
	printf("Input File: %s\n Output File: %s\n", argv[1], argv[2]);
}
