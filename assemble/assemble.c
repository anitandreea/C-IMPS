#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <paths.h>
#include <assert.h>
#include "rbtree.h"
#include "assemble.h"

void writeFile( char *filename, int *instructions, int fileSize ) {
	FILE *fp = fopen(filename,"wb");
	fwrite(instructions, sizeof(int), fileSize, fp);
	fclose(fp);
}


/*
 *	The idea behind this implementation is that labelTree will contain 
 *	all the references for labels found in the program and also
 *	references to functions which perform assembly of particular instructions.
 *	It makes labelTree of type String -> labelRef (as in assemble.h file)
 *
 *	On the other hand the programTree will contain the every line of program
 *	encoded as struct which will have all the information necessary to form
 *	machine instruction. Therefore programTree is of type int -> aInstruction (as in assemble.h) 
 *	
 *	This makes second pass arguably easier because only thing that has to be done
 *	is to flatten programTree (in pre-order) while executing functions which will be obtained from
 *	labelTree by simple lookup
 */
void parseInputFile(char * fileName, rbTreeNode programTree, rbTreeNode labelTree) {
	FILE *sourceFile = fopen(fileName, "r");
	char readBuffer[160];
	if( sourceFile == NULL ) {
		perror("Error opening file");
	} else {
		while(fgets(readBuffer, sizeof(readBuffer), sourceFile) != NULL) {
			// at this point readBuffer contains current line
			// the way to split it is presented in commented code below
			printf("%s", readBuffer);
		}
	}

/*	char *test = "test string label: fds";
	printf("Splitting test into: %s\n",test);
	char * result = strtok(strdup(test), " ");
	while(result != NULL) {
		printf("%s\n", result);
		result = strtok(NULL, " ");
	}*/
}

int compareInt(void * left, void * right) {
	int x = (int) left;
	int y = (int) right;
	if ( x < y ) {
		return -1;
	} else if ( x > y ) {
		return 1;
	} else {
		assert( x == y);
		return 0;
	}
}

int compareStr(void * left, void * right) {
	char * x = (char *) left;
	char * y = (char *) right;
	int comparisonResult = strcmp(x,y);
	if ( comparisonResult < 0 ) {
		return -1;
	} else if ( comparisonResult > 0 ) {
		return 1;
	} else {
		assert( comparisonResult == 0 );
		return 0;
	}
}

int main(int argc, char *argv[]) {
	if(argc < 3) {
		printf("Wrong arguments");
		exit(1);
	}
	
	printf("%d\n",argc);	
	printf("Input File: %s\nOutput File: %s\n", argv[1], argv[2]);
	
	rbTreeNode programTree = NULL;
	rbTreeNode labelTree = NULL;

	

	parseInputFile(argv[1], programTree, labelTree);	
	free(programTree);
	free(labelTree);
	return 0;
}
