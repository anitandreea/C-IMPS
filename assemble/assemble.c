#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <paths.h>
#include <assert.h>
#include <stdarg.h>
#include "rbtree.h"
#include "assemble.h"

#define DEBUG 1

const char * Iinst = "addi,subi,muli,lw,sw,beq,bne,blt,bgt,ble,bge";
const char * Rinst = "add,sub,mul,jr";
const char * Jinst = "jmp,jal";

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
 *	It makes labelTree of type String -> labelRef (as in assemble.h file)
 *
 */
void parseInputFile(char * fileName, rbTreeNode programTree, rbTreeNode labelTree) {
	FILE *sourceFile = fopen(fileName, "r");
	char readBuffer[160];
	char * instSplit = "$, ";
	if( sourceFile == NULL ) {
		perror("Error opening file");
	} else {
		int currentLine = 0;
		while(fgets(readBuffer, sizeof(readBuffer), sourceFile) != NULL) {
			aInstruction assemblyInst = (aInstruction)malloc(sizeof(struct assemblyInstruction)); 
			char * readToken = strtok(readBuffer, instSplit);
			if(strstr(readToken, ":") != NULL) {
				assemblyInst->label = (char *)malloc(sizeof(char)*strlen(readToken)-1);
				strncpy(assemblyInst->label, readToken, strlen(readToken)-1);
				LOG_DEBUG("%s\n",assemblyInst->label);
				labelRef labelLine = (labelRef)malloc(sizeof(union labelReference));
				labelLine->line = currentLine;
				rbInsert(labelTree, (void *)assemblyInst->label, (void *)labelLine, compareStr);
				readToken = strtok(NULL, instSplit);
			}
			if(strstr(Rinst, readToken) != NULL) {
				strcpy(assemblyInst->opcode, readToken); 
				LOG_DEBUG("R type instruction: %s\n", assemblyInst->opcode);
				assemblyInst->r1 = atoi(strtok(NULL, instSplit));				
				LOG_DEBUG("r1: %d\n", assemblyInst->r1);
				assemblyInst->r2 = atoi(strtok(NULL, instSplit));				
				LOG_DEBUG("r2: %d\n", assemblyInst->r2);
				assemblyInst->r3 = atoi(strtok(NULL, instSplit));
				LOG_DEBUG("r3: %d\n", assemblyInst->r3);
			} else if(strstr(Iinst, readToken) != NULL) {
				strcpy(assemblyInst->opcode, readToken); 
				LOG_DEBUG("I type instruction: %s\n", assemblyInst->opcode);	
				assemblyInst->r1 = atoi(strtok(NULL, instSplit));
				LOG_DEBUG("r1: %d\n", assemblyInst->r1);
				assemblyInst->r2 = atoi(strtok(NULL, instSplit));
				LOG_DEBUG("r2: %d\n", assemblyInst->r2);
				readToken = strtok(NULL, instSplit);
				assemblyInst->address = (char *)malloc(sizeof(char)*strlen(readToken));
				strcpy(assemblyInst->address, readToken);
				LOG_DEBUG("immediate: %s\n", assemblyInst->address);
			} else if(strstr(Jinst, readToken) != NULL) {
				strcpy(assemblyInst->opcode, readToken); 
				LOG_DEBUG("J type instruction: %s\n", assemblyInst->opcode);
				readToken = strtok(NULL, instSplit);
				assemblyInst->address = (char *)malloc(sizeof(char)*strlen(readToken));
				strcpy(assemblyInst->address, readToken);
				LOG_DEBUG("address: %s\n", assemblyInst->address);
			} else if(strstr(".skip", readToken) != NULL) { 
				strcpy(assemblyInst->opcode, readToken); 
				LOG_DEBUG(".skip instruction\n");	
				assemblyInst->r1 = atoi(strtok(NULL, instSplit));				
				currentLine += assemblyInst->r1-1;
				LOG_DEBUG("skip value: %d\n", assemblyInst->r1);
			} else if(strstr(".fill", readToken) != NULL) {
				strcpy(assemblyInst->opcode, readToken); 
				LOG_DEBUG(".fill instruction\n");	
				assemblyInst->r1 = atoi(strtok(NULL, instSplit));				
				LOG_DEBUG("fill value: %d\n", assemblyInst->r1);
			} else {
				strcpy(assemblyInst->opcode, readToken); 
				LOG_DEBUG("halt instruction\n");	
			}
			++currentLine;
		}
	}
	fclose(sourceFile);
}

int main(int argc, char *argv[]) {
	if(argc < 3) {
		printf("Wrong arguments");
		exit(1);
	}
	
	LOG_DEBUG("%d\n",argc);	
	LOG_DEBUG("Input File: %s\nOutput File: %s\n", argv[1], argv[2]);
	
	rbTreeNode programTree = NULL;
	rbTreeNode labelTree = NULL;

	

	parseInputFile(argv[1], programTree, labelTree);	
	free(programTree);
	free(labelTree);
	return 0;
}
