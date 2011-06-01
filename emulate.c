#include <stdlib.h>
#include <stdio.h>

#define OPCODEMASK = 0
#define r1Mask = 1
#define r2Mask = 2
#define r3Mask = 3
#define immediateMask = 4
#define addressMask = 5

int masks[] = {-1 << 26, (-1 << 26) ^ (-1) ^ (1<<31>>10 ^ -1), (-1 << 21) ^ (-1) ^ (1<<31>>15 ^ -1), (-1 << 16) ^ (-1) ^ (1<<31>>20 ^ -1), (1 << 31 >> 15) ^ (-1), (1 << 31 >> 5) ^ (-1)};
int masksShift[] = {26,21,16,11,0,0};

typedef struct {
	char *memory[65536];
	int *registers[32];
	int PC;
} IMPSState;

int extractFromInstruction(int mask, int instruction) {
	return (instruction & masks[mask]) >> masksShift[mask];	
}

int * allocateArray(int size) {
	int *instructions;
	instructions = (int *) malloc( size * sizeof( int ) ); 
	int i;
	for(i=0; i<size; i++) {
		instructions[i] = 0;
	}
	return instructions;
}

int getFileSize(char *filename) {
	FILE *fp = fopen(filename,"rb");
    	fseek(fp, 0, SEEK_END);
    	int instructionSize=ftell(fp)>>2;
	printf("%d\n", instructionSize);
	fclose(fp);
	return instructionSize;
}

int * readFile( char *filename, int fileSize ) {
	FILE *fp = fopen(filename,"rb");
	int *instructions = allocateArray(fileSize);
	fread(instructions,sizeof(int),fileSize,fp);
	fclose(fp);
	return instructions;
}

void printInstructions(int * instructions, int instructionSize) {
	int i;
	for(i=0; i<instructionSize; i++ ) {
		printBinary(instructions[i]);
	}	
}

void printBinary( int number ) {
	int mask = 1 << 31;
	int i;
	for(i=0;i<32;i++) {
		if((number & mask) == 0) printf("0"); else printf("1");
		number = number << 1;
	}
	printf("\n");
}

void writeFile( char *filename, int *instructions, int fileSize ) {
	FILE *fp = fopen(filename,"wb");
	fwrite(instructions, sizeof(int), fileSize, fp);
	fclose(fp);
}

void dumpState( IMPSState * state ) {
	printf("Registers:\n");
	printf("PC :	%d (%x)", state->PC, state->PC);
	int i;
	for(i =0; i<32; i++) {
		printf("$%d:	%d (%x)", i, *state->registers[i], *state->registers[i]);
	}
}

int main( int argc, char **argv) {
	int instructionSize = getFileSize(argv[1]);
	int * instructions = readFile(argv[1], instructionSize);
	printInstructions(instructions, instructionSize);
	//writeFile("abc",instructions,instructionSize);
	return 0;
}
