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
const char * Binst = "beq,bne,blt,bgt,ble,bge";

const char * OpCode[18] = {"halt", "add", "addi", "sub", "subi", "mul", 
	"muli", "lw", "sw", "beq", "bne", "blt", "bgt", "ble", "bge", 
	"jmp", "jr", "jal"};

int findOpcode(char * Opcode) {
	int i;
	for(i = 0; i<18; i++) {
		if(OpCode[i] == Opcode) {
			return i;
		}
	}
	return 18;
}

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
			unsigned char label = 0;
			aInstruction assemblyInst = createInstruction(); 
			assemblyInst->line = currentLine;
			char * readToken = strtok(readBuffer, instSplit);
			if(strstr(readToken, ":") != NULL) {
				assemblyInst->label = (char *)malloc(sizeof(char)*strlen(readToken)-1);
				strncpy(assemblyInst->label, readToken, strlen(readToken)-1);
				readToken = strtok(NULL, instSplit);
				label = 1;
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
			if(label == 1) {
				rbInsert(labelTree, (void *)assemblyInst->label, (void *)assemblyInst, compareStr);
			}
		}
	}
	fclose(sourceFile);
	return pList;
}

void assemble( char * outFile, listItem pList, rbTree labelTree ) {
	FILE *fp = fopen(outFile,"ab");
	while(pList != NULL) {
		aInstruction aInst = pList->assemblyInstruction;
		int inst = 0;
		if(strstr(".fill", aInst->opcode) != NULL) {	
			inst = aInst->r1;
			fwrite(&inst, sizeof(int), 1, fp);
		} else if(strstr(".skip", aInst->opcode) != NULL) {
			int i;
			for(i = aInst->r1; i>0; i--) {
				fwrite(&inst, sizeof(int), 1, fp);
			}	
		} else if(strstr(Rinst, aInst->opcode) != NULL) {
			int opNum = findOpcode(aInst->opcode);
			inst = ( opNum << 26 ) + (aInst->r1 << 19 ) + (aInst->r2 << 14) + (aInst->r3 << 9);
			fwrite(&inst, sizeof(int), 1, fp);
		} else if(strstr(Jinst, aInst->opcode) != NULL) {

		} else if(strstr(Binst, aInst->opcode) != NULL) {
		
		} else if (strstr(Iinst, aInst->opcode) != NULL && strstr(Binst, aInst->opcode) == NULL) {
			
		} else {
			fwrite(&inst, sizeof(int), 1, fp);
		}
		pList = pList->next;
	}
	fclose(fp);
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

	assemble( argv[2], pList, labelTree );

	freeList(pList);
	free(labelTree);
	return 0;
}
