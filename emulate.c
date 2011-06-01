#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

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
#define REG_ADDRESS_OUT_OF_BOUNDS 12

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
		case REG_ADDRESS_OUT_OF_BOUNDS:
			printf("Requested register doesn't exist");
			break;
	}
	exit(2);
}

int extractFromInstruction(int mask, int instruction) {
	return (instruction & masks[mask]) >> masksShift[mask];	
}

int getFileSize(char *filename) {
	FILE *fp = fopen(filename,"rb");
    	fseek(fp, 0, SEEK_END);
    	int instructionSize=ftell(fp)>>2;
	fclose(fp);
	return instructionSize;
}

int readInstructions( struct IMPSState * state, char *filename, int fileSize ) {
	FILE *fp = fopen(filename,"rb");
	fread(state->memory,sizeof(unsigned int),fileSize,fp);
	fclose(fp);
	return SUCCESS;
}

char *printBits( int number ) {
	char *binary = '\0';
	int mask = 1 << 31;
	int i;
	int s = 1;
	for(i=0;i<32;i++) {
		if((number & mask) == 0) {
			strcat(binary,"0");
		} else { 
			strcat(binary, "1");
		}
		if(s % 8 == 0) {
			strcat(binary, " ");
		}
		++s;
		number = number << 1;
	}
	strcat(binary,"\n");
	return binary;
}



void dumpState( struct IMPSState * state ) {
	printf("\nRegisters:\n");
	printf("PC :	%d (%x)\n", state->PC, state->PC);
	int i;
	for(i =0; i<32; i++) {
		printf("$%d:	%d (%x)\n", i, state->registers[i], state->registers[i]);
	}
}

struct IMPSState * initState() {
	struct IMPSState *state;
	state = (struct IMPSState *)malloc(sizeof(struct IMPSState));
	if(state == NULL) {
		perror("malloc");
		exit(2);
	}
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

void writeMemory(struct IMPSState * state, int address, int data) { 
	if( address > 65535 || address < 0 ) {
		raiseError(ADDRESS_OUT_OF_BOUNDS);
	}
	*((unsigned int *)&state->memory[address]) = data;
}

int readMemory(struct IMPSState * state, int address) {
	if( address > 65535 || address < 0 ) {
		raiseError(ADDRESS_OUT_OF_BOUNDS);
	}
	unsigned int data = *((unsigned int *)&state->memory[address]);
	return data;
}

void writeRegister(struct IMPSState * state, int r, int data) {
	if(r < 0 || r > 31) {
		raiseError(REG_ADDRESS_OUT_OF_BOUNDS);
	}
	state->registers[r] = data;	
}

int readRegister(struct IMPSState * state, int r) {
	if(r < 0 || r > 31) {
		raiseError(REG_ADDRESS_OUT_OF_BOUNDS);
	}
	return state->registers[r];
}

int checkOpCode(int opCode) {
	if(opCode < 0 || opCode > 17) {
		raiseError(ILLEGAL_OPCODE);
	}
	return opCode;
}

int halt(struct IMPSState * state) {
	return HALT;
}

int add(struct IMPSState * state) {
	int r1 = extractFromInstruction(r1Mask, state->IR);
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int r3 = readRegister(state, extractFromInstruction(r3Mask, state->IR));
      	LOG_DEBUG("Instruction: %s; (R%d = %d + %d)",printBits(state->IR),r1,r2,r3);
	writeRegister(state, r1, r2+r3);	
	state->PC = state->PC + 4;
	return SUCCESS;
}

int main( int argc, char **argv) {

	struct IMPSState *state = initState();

	int (*OpCodeFunction[18]) (struct IMPSState * state);
	OpCodeFunction[0] = &halt;
	OpCodeFunction[1] = &add;
	OpCodeFunction[2] = &halt;
	OpCodeFunction[3] = &halt;
	OpCodeFunction[4] = &halt;
	OpCodeFunction[5] = &halt;
	OpCodeFunction[6] = &halt;
	OpCodeFunction[7] = &halt;
	OpCodeFunction[8] = &halt;
	OpCodeFunction[9] = &halt;
	OpCodeFunction[10] = &halt;
	OpCodeFunction[11] = &halt;
	OpCodeFunction[12] = &halt;
	OpCodeFunction[13] = &halt;
	OpCodeFunction[14] = &halt;
	OpCodeFunction[15] = &halt;
	OpCodeFunction[16] = &halt;
	OpCodeFunction[17] = &halt;

	readInstructions(state, argv[1], getFileSize(argv[1]));
	
	int result = SUCCESS;
	
	//while(result != HALT) {
		state->IR = readMemory(state, state->PC);
		int OpCode = extractFromInstruction(OPCODEMASK, state->IR);
		LOG_DEBUG("%d\n",OpCode);
		result = (*OpCodeFunction[checkOpCode(OpCode)])(state);
	//}

	dumpState(state);
	free(state);
	return 0;
}
