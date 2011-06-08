#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include "rbtree.h"
#include "assemble.h"
#include "list.h"

#define DEBUG 1

const char * Iinst = "addi,subi,muli,lw,sw,beq,bne,blt,bgt,ble,bge";
const char * Rinst = "add,sub,mul,jr";
const char * Jinst = "jmp,jal";

aInstruction createInstruction() {
	aInstruction assemblyInst = (aInstruction)malloc(sizeof(struct assemblyInstruction)); 
	assemblyInst->r1 = 0;
	assemblyInst->r2 = 0;
	assemblyInst->r3 = 0;
	assemblyInst->line = 0;
	return assemblyInst;
}

void LOG_DEBUG( char *format, ... ) {
	//Debugging mode prints instructions and object file code.
	if(DEBUG == 1) {
		va_list arguments;
		va_start( arguments, format);
		vprintf(format, arguments);
		va_end(arguments);
	}
}

void writeFile( char *filename, int *instructions, int fileSize ) {
	FILE *fp = fopen(filename,"wb");
	fwrite(instructions, sizeof(int), fileSize, fp);
	fclose(fp);
}

int compareInt(void * left, void * right) {
	int x = *(int *) left;
	int y = *(int *) right;
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

/*
 *	The idea behind this implementation is that labelTree will contain 
 *	all the references for labels found in the program and also
 *	references to functions which perform assembly of particular instructions.
 *	It makes labelTree of type String -> int (as in assemble.h file)
 *
 */
listItem parseInputFile(char * fileName, rbTree labelTree) {
	listItem pList = NULL;
	FILE *sourceFile = fopen(fileName, "r");
	char readBuffer[160];
	char * instSplit = "$, ";
	if( sourceFile == NULL ) {
		perror("Error opening file");
	} else {
		int currentLine = 0;
		while(fgets(readBuffer, sizeof(readBuffer), sourceFile) != NULL) {
			aInstruction assemblyInst = createInstruction(); 
			assemblyInst->line = currentLine;
			char * readToken = strtok(readBuffer, instSplit);
			if(strstr(readToken, ":") != NULL) {
				assemblyInst->label = (char *)malloc(sizeof(char)*strlen(readToken)-1);
				strncpy(assemblyInst->label, readToken, strlen(readToken)-1);
				rbInsert(labelTree, (void *)assemblyInst->label, (void *)&currentLine, compareStr);
				readToken = strtok(NULL, instSplit);
			}
			if(strstr(Rinst, readToken) != NULL) {
				strcpy(assemblyInst->opcode, readToken); 
				assemblyInst->r1 = atoi(strtok(NULL, instSplit));				
				assemblyInst->r2 = atoi(strtok(NULL, instSplit));				
				assemblyInst->r3 = atoi(strtok(NULL, instSplit));
			} else if(strstr(Iinst, readToken) != NULL) {
				strcpy(assemblyInst->opcode, readToken); 
				assemblyInst->r1 = atoi(strtok(NULL, instSplit));
				assemblyInst->r2 = atoi(strtok(NULL, instSplit));
				readToken = strtok(NULL, instSplit);
				assemblyInst->address = (char *)malloc(sizeof(char)*strlen(readToken));
				strcpy(assemblyInst->address, readToken);
			} else if(strstr(Jinst, readToken) != NULL) {
				strcpy(assemblyInst->opcode, readToken); 
				readToken = strtok(NULL, instSplit);
				assemblyInst->address = (char *)malloc(sizeof(char)*strlen(readToken));
				strcpy(assemblyInst->address, readToken);
			} else if(strstr(".skip", readToken) != NULL) { 
				strcpy(assemblyInst->opcode, readToken); 
				assemblyInst->r1 = atoi(strtok(NULL, instSplit));				
				currentLine += assemblyInst->r1-1;
			} else if(strstr(".fill", readToken) != NULL) {
				strcpy(assemblyInst->opcode, readToken); 
				assemblyInst->r1 = atoi(strtok(NULL, instSplit));				
			} else {
				strcpy(assemblyInst->opcode, readToken); 
			}
			++currentLine;
			pList =	lInsertTail(assemblyInst, pList);
		}
	}
	fclose(sourceFile);
	return pList;
}

int main(int argc, char *argv[]) {
	if(argc < 3) {
		printf("Wrong arguments");
		exit(1);
	}
	
	rbTree labelTree = rbTreeCreate();

	listItem pList = parseInputFile(argv[1], labelTree);	

	printList(pList);
	printTree(labelTree);
	
	freeList(pList);
	free(labelTree);
	return 0;
}
