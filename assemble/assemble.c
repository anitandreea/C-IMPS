#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <ctype.h>
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

void LOG_DEBUG( char *format, ... ) {
	//Debugging mode prints instructions and object file code.
	if(DEBUG == 1) {
		va_list arguments;
		va_start( arguments, format);
		vprintf(format, arguments);
		va_end(arguments);
	}
}

void freeTreeHelper( rbTreeNode node ) {
	if( node == NULL ) return;
	freeTreeHelper( node->left );
	freeTreeHelper( node->right );
	free(node);
}

void freeTree( rbTree tree ) {
	if( tree->root == NULL ) return;
	freeTreeHelper(tree->root);
	free(tree);
}

int findOpcode(char * Opcode) {
	int i;
	for(i = 0; i<18; i++) {
		if(strcmp(OpCode[i], Opcode) == 0) {
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
			unsigned char isEmpty = 1;
			aInstruction assemblyInst = createInstruction(); 
			assemblyInst->line = currentLine;
			char * readToken = strtok(readBuffer, instSplit);
			if(strstr(readToken, ":") != NULL) {
				assemblyInst->label = (char *)malloc(sizeof(char)*strlen(readToken)-1);
				strncpy(assemblyInst->label, readToken, strlen(readToken)-1);
				readToken = strtok(NULL, instSplit);
				label = 1;
			}
			strcpy(assemblyInst->opcode, readToken); 
			if(strstr(Rinst, readToken) != NULL) {
				assemblyInst->r1 = atoi(strtok(NULL, instSplit));				
				assemblyInst->r2 = atoi(strtok(NULL, instSplit));				
				assemblyInst->r3 = atoi(strtok(NULL, instSplit));
				isEmpty = 0;
			} else if(strstr(Iinst, readToken) != NULL) {
				assemblyInst->r1 = atoi(strtok(NULL, instSplit));
				assemblyInst->r2 = atoi(strtok(NULL, instSplit));
				readToken = strtok(NULL, instSplit);
				assemblyInst->address = (char *)malloc(sizeof(char)*strlen(readToken));
				strncpy(assemblyInst->address, readToken, isspace(readToken[strlen(readToken)-1]) ? strlen(readToken)-1 : strlen(readToken) );
				isEmpty = 0;
			} else if(strstr(Jinst, readToken) != NULL) {
				readToken = strtok(NULL, instSplit);
				assemblyInst->address = (char *)malloc(sizeof(char)*strlen(readToken));
 				strncpy(assemblyInst->address, readToken, isspace(readToken[strlen(readToken)-1]) ? strlen(readToken)-1 : strlen(readToken) );
				isEmpty = 0;
			} else if(strstr(".skip", readToken) != NULL) { 
				assemblyInst->r1 = atoi(strtok(NULL, instSplit));				
				currentLine += assemblyInst->r1-1;
				isEmpty = 0;
			} else if(strstr(".fill", readToken) != NULL) {
				assemblyInst->r1 = atoi(strtok(NULL, instSplit));				
				isEmpty = 0;
			} else if(strstr(readToken, "halt") != NULL) {
				isEmpty=0;
			}
			if(!isEmpty) {			
				++currentLine;
				pList =	lInsertTail(assemblyInst, pList);
			} else {
				free(assemblyInst);
			}
			if(label == 1) {
				rbInsert(labelTree, (void *)assemblyInst->label, (void *)assemblyInst, compareStr);
			}
		}
	}
	fclose(sourceFile);
	return pList;
}

int signReduction( int num ) {
	return num & 0x0000FFFF;
}

int resolveImmediate(char * address) {
	int result;
	if(strstr(address, "x") != NULL) {
		result = (int)strtol(address, NULL, 16);	
	} else {
		result = atoi(address);
	} 
	result = signReduction(result);
	return result;
}

void assemble( char * outFile, listItem pList, rbTree labelTree ) {
	FILE *fp = fopen(outFile,"ab");
	while(pList != NULL) {
		aInstruction aInst = pList->assemblyInstruction;
		int inst = 0;
		if(strstr(aInst->opcode, ".fill") != NULL) {	
			inst = aInst->r1;
			fwrite(&inst, sizeof(int), 1, fp);
		} else if(strstr(aInst->opcode, ".skip") != NULL) {
			int i;
			for(i = aInst->r1; i>0; i--) {
				fwrite(&inst, sizeof(int), 1, fp);
			}	
		} else if(strstr(Rinst, aInst->opcode) != NULL) {
			int opNum = findOpcode(aInst->opcode);
			inst = ( opNum << 26 ) | (aInst->r1 << 21 ) | (aInst->r2 << 16) | (aInst->r3 << 11);
			fwrite(&inst, sizeof(int), 1, fp);
		} else if(strstr(Jinst, aInst->opcode) != NULL) {
			int opNum = findOpcode(aInst->opcode);
			inst = opNum << 26;
			if(strstr(aInst->address, "x") != NULL) {
				inst |= 4*resolveImmediate(aInst->address);	
			} else { 
				aInstruction labelRef = rbLookup(labelTree, aInst->address, compareStr);
				printInstruction(labelRef);
				inst |= 4*labelRef->line;
			}
			fwrite(&inst, sizeof(int), 1, fp);
		} else if(strstr(Binst, aInst->opcode) != NULL) {
			int opNum = findOpcode(aInst->opcode);
			int relAddress;
			inst = opNum << 26;
			aInstruction labelRef = rbLookup(labelTree, aInst->address, compareStr);
			if(labelRef == NULL) {
				relAddress = resolveImmediate(aInst->address);		
			} else {
				printInstruction(labelRef);
				relAddress = labelRef->line - aInst->line;	
			}
			inst |= (aInst->r1 << 21) | (aInst->r2 << 16) | relAddress;	
			fwrite(&inst, sizeof(int), 1, fp);
		} else if (strstr(Iinst, aInst->opcode) != NULL && strstr(Binst, aInst->opcode) == NULL) {
			int opNum = findOpcode(aInst->opcode);
			int relAddress;
			inst = opNum << 26;
			aInstruction labelRef = rbLookup(labelTree, aInst->address, compareStr);
			if(labelRef == NULL) {
				relAddress = resolveImmediate(aInst->address);
			} else {
				printInstruction(labelRef);
				relAddress = 4*labelRef->line;	
			}		
			inst |= (aInst->r1 << 21) | (aInst->r2 << 16) | relAddress;	
			fwrite(&inst, sizeof(int), 1, fp);
		} else if (strstr(aInst->opcode, "halt") != NULL){
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
	freeTree(labelTree);
	return 0;
}
