#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define DEBUG 1
#define OPCODEMASK 0
#define r1Mask 1
#define r2Mask 2
#define r3Mask 3
#define immediateMask 4
#define addressMask 5

#define HALT 99
#define SUCCESS 0

#define ADDRESS_OUT_OF_BOUNDS 10
#define ILLEGAL_OPCODE 11

int masks[] = {-1 << 26, (-1 << 26) ^ (-1) ^ (1<<31>>10 ^ -1), (-1 << 21) ^ (-1) ^ (1<<31>>15 ^ -1), (-1 << 16) ^ (-1) ^ (1<<31>>20 ^ -1), (1 << 31 >> 15) ^ (-1), (1 << 31 >> 5) ^ (-1)};
int masksShift[] = {26,21,16,11,0,0};



struct IMPSState {
	char memory[65536] ;
	int registers[32] ;
	int PC;
	int IR;
};

void LOG_DEBUG( char *format, ... ) {
	if(DEBUG == 1) {
		va_list arguments;
		va_start( arguments, format);
		vprintf(format, arguments);
		va_end(arguments);
	}
}

void raiseError( int errorType ) {
	switch(errorType) {
		case ADDRESS_OUT_OF_BOUNDS:
			printf("Requested address is out of bounds");
			break;
		case ILLEGAL_OPCODE:
			printf("Operation not permited");
			break;
	}
}

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
	LOG_DEBUG("%d\n", instructionSize);
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

void printBits( int number ) {
	int mask = 1 << 31;
	int i;
	int s = 1;
	for(i=0;i<32;i++) {
		if((number & mask) == 0) LOG_DEBUG("0"); else LOG_DEBUG("1");
		if(s % 8 == 0) {
			LOG_DEBUG(" ");
		}
		++s;
		number = number << 1;
	}
	LOG_DEBUG("\n");
}

void printInstructions(int * instructions, int instructionSize) {
	int i;
	for(i=0; i<instructionSize; i++ ) {
		printBits(instructions[i]);
	}	
}

void writeFile( char *filename, int *instructions, int fileSize ) {
	FILE *fp = fopen(filename,"wb");
	fwrite(instructions, sizeof(int), fileSize, fp);
	fclose(fp);
}

void dumpState( struct IMPSState * state ) {
	printf("Registers:\n");
	printf("PC :	%d (%x)\n", state->PC, state->PC);
	int i;
	for(i =0; i<32; i++) {
		printf("$%d:	%d (%x)\n", i, state->registers[i], state->registers[i]);
	}
}

struct IMPSState * initState() {
	struct IMPSState *state;
	state = (struct IMPSState *)malloc(sizeof(struct IMPSState));
	int i;
	for(i=0; i<65535; i++) {
		state->memory[i] = 0;
	}
	for(i=0; i<32; i++) {
		state->registers[i] = 0;
	}
	state->PC = 0;
	state->IR = 0;
	return state;
}

int writeToMemory(struct IMPSState * state, int address, int data) { 
	if( address > 65535 || address < 0 ) {
		raiseError(ADDRESS_OUT_OF_BOUNDS);
	}
}

int halt(struct IMPSState * state) {
	return HALT;
}

int main( int argc, char **argv) {

	struct IMPSState *state = initState();

	int *OpCodeFunction[18];
	OpCodeFunction[0] = (int *)&halt;
	OpCodeFunction[1] = (int *)&halt;
	OpCodeFunction[2] = (int *)&halt;
	OpCodeFunction[3] = (int *)&halt;
	OpCodeFunction[4] = (int *)&halt;
	OpCodeFunction[5] = (int *)&halt;
	OpCodeFunction[6] = (int *)&halt;
	OpCodeFunction[7] = (int *)&halt;
	OpCodeFunction[8] = (int *)&halt;
	OpCodeFunction[9] = (int *)&halt;
	OpCodeFunction[10] = (int *)&halt;
	OpCodeFunction[11] = (int *)&halt;
	OpCodeFunction[12] = (int *)&halt;
	OpCodeFunction[13] = (int *)&halt;
	OpCodeFunction[14] = (int *)&halt;
	OpCodeFunction[15] = (int *)&halt;
	OpCodeFunction[16] = (int *)&halt;
	OpCodeFunction[17] = (int *)&halt;


	int instructionSize = getFileSize(argv[1]);
	int * instructions = readFile(argv[1], instructionSize);
	printInstructions(instructions, instructionSize);
/*int result = SUCCESS;
while(result != HALT) {
}*/
	//writeFile("abc",instructions,instructionSize);

	dumpState(state);
	free(state);
	return 0;
}
