#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define DEBUG 0
#define opCodeMask 0
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

int masks[] = {0xfc000000,0x03e00000,0x001f0000,0x0000f800,0x0000ffff,0x03ffffff};
int masksShift[] = {26,21,16,11,0,0};

struct IMPSState {
	char memory[65536] ;
	int registers[32] ;
	int PC;
	int IR;
};

char *printBits( int number ) {
	char * binary = (char *)malloc(32*sizeof(char));
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
	return binary;
}

void LOG_DEBUG( int * IR, char *format, ... ) {
	if(DEBUG == 1) {
		va_list arguments;
		va_start( arguments, format);
		if( IR != NULL ) {
			char * IRbinary = printBits(*IR);
			printf("Instruction: %s; ", IRbinary);
			free(IRbinary);
		}
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

int signExt( int x ) {
	if( x & (1<<15) ) {
		x = x | ( 1 << 31 >> 15 ) ;
	}
	return x;
}

int extractFromInstruction(int mask, int instruction) {
	int result = (instruction & masks[mask]) >> masksShift[mask];	
	if(mask == immediateMask) {
		result = signExt(result);
	}
	return result;
}

int readInstructions( struct IMPSState * state, char *filename ) {
	FILE *fp = fopen(filename,"rb");
	if(fp == NULL) {
		perror("Can't open specified file");
		exit(3);
	}
	fseek(fp, 0, SEEK_END);
    	int size=ftell(fp)>>2;
	fseek(fp, 0, SEEK_SET);

	fread(state->memory,sizeof(int),size,fp);
	fclose(fp);
	return SUCCESS;
}

void dumpState( struct IMPSState * state ) {
	printf("\nRegisters:\n");
	printf("PC :	%7d (0x%08x)\n", state->PC, state->PC);
	int i;
	for(i =0; i<32; i++) {
		printf("$%-2d:	%7d (0x%08x)\n", i, state->registers[i], state->registers[i]);
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
	*((int *)&state->memory[address]) = data;
}

int readMemory(struct IMPSState * state, int address) {
	if( address > 65535 || address < 0 ) {
		raiseError(ADDRESS_OUT_OF_BOUNDS);
	}
	int data = *((int *)&state->memory[address]);
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
      	LOG_DEBUG(&state->IR,"(HALT)\n");
	return HALT;
}

int add(struct IMPSState * state) {
	int r1 = extractFromInstruction(r1Mask, state->IR);
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int r3 = readRegister(state, extractFromInstruction(r3Mask, state->IR));
      	LOG_DEBUG(&state->IR, "(R%d = %d + %d)\n",r1,r2,r3);
	writeRegister(state, r1, r2+r3);	
	state->PC = state->PC + 4;
	return SUCCESS;
}

int addi(struct IMPSState * state) {
	int r1 = extractFromInstruction(r1Mask, state->IR);
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int c = extractFromInstruction(immediateMask, state->IR);
      	LOG_DEBUG(&state->IR,"(R%d = %d + %d)\n",r1,r2,c);
	writeRegister(state, r1, r2+c);	
	state->PC = state->PC + 4;
	return SUCCESS;
}

int sub(struct IMPSState * state) {
	int r1 = extractFromInstruction(r1Mask, state->IR);
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int r3 = readRegister(state, extractFromInstruction(r3Mask, state->IR));
      	LOG_DEBUG(&state->IR,"(R%d = %d - %d)\n",r1,r2,r3);
	writeRegister(state, r1, r2-r3);	
	state->PC = state->PC + 4;
	return SUCCESS;
}

int subi(struct IMPSState * state) {
	int r1 = extractFromInstruction(r1Mask, state->IR);
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int c = extractFromInstruction(immediateMask, state->IR);
      	LOG_DEBUG(&state->IR,"(R%d = %d - %d)\n",r1,r2,c);
	writeRegister(state, r1, r2-c);	
	state->PC = state->PC + 4;
	return SUCCESS;
}

int mul(struct IMPSState * state) {
	int r1 = extractFromInstruction(r1Mask, state->IR);
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int r3 = readRegister(state, extractFromInstruction(r3Mask, state->IR));
      	LOG_DEBUG(&state->IR,"(R%d = %d * %d)\n",r1,r2,r3);
	writeRegister(state, r1, r2*r3);	
	state->PC = state->PC + 4;
	return SUCCESS;
}

int muli(struct IMPSState * state) {
	int r1 = extractFromInstruction(r1Mask, state->IR);
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int c = extractFromInstruction(immediateMask, state->IR);
      	LOG_DEBUG(&state->IR,"(R%d = %d * %d)\n",r1,r2,c);
	writeRegister(state, r1, r2*c);	
	state->PC = state->PC + 4;
	return SUCCESS;
}

int lw(struct IMPSState * state) {
	int r1 = extractFromInstruction(r1Mask, state->IR);
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int c = extractFromInstruction(immediateMask, state->IR);
      	LOG_DEBUG(&state->IR,"(R%d = MEMORY[%d + %d])\n",r1,r2,c);
	writeRegister(state, r1, readMemory(state, r2+c));	
	state->PC = state->PC + 4;
	return SUCCESS;
}

int sw(struct IMPSState * state) {
	int r1 = extractFromInstruction(r1Mask, state->IR);
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int c = extractFromInstruction(immediateMask, state->IR);
      	LOG_DEBUG(&state->IR,"(MEMORY[%d + %d] = R%d)\n",r2,c,r1);
	writeMemory(state, r2+c, readRegister(state,r1));	
	state->PC = state->PC + 4;
	return SUCCESS;
}

int beq(struct IMPSState * state) {
	int r1 = readRegister(state, extractFromInstruction(r1Mask, state->IR));
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int c = extractFromInstruction(immediateMask, state->IR);
      	LOG_DEBUG(&state->IR,"(If %d == %d then PC = PC + %d * 4)\n",r1,r2,c);
	if(r1 == r2) {
		state->PC = state->PC + c*4;
	} else {
		state->PC = state->PC + 4;
	}
	return SUCCESS;
}

int bne(struct IMPSState * state) {
	int r1 = readRegister(state, extractFromInstruction(r1Mask, state->IR));
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int c = extractFromInstruction(immediateMask, state->IR);
      	LOG_DEBUG(&state->IR,"(If %d != %d then PC = PC + %d * 4)\n",r1,r2,c);
	if(r1 != r2) {
		state->PC = state->PC + c*4;
	} else {
		state->PC = state->PC + 4;
	}
	return SUCCESS;
}

int blt(struct IMPSState * state) {
	int r1 = readRegister(state, extractFromInstruction(r1Mask, state->IR));
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int c = extractFromInstruction(immediateMask, state->IR);
      	LOG_DEBUG(&state->IR,"(If %d < %d then PC = PC + %d * 4)\n",r1,r2,c);
	if(r1 < r2) {
		state->PC = state->PC + c*4;
	} else {
		state->PC = state->PC + 4;
	}
	return SUCCESS;
}

int bgt(struct IMPSState * state) {
	int r1 = readRegister(state, extractFromInstruction(r1Mask, state->IR));
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int c = extractFromInstruction(immediateMask, state->IR);
      	LOG_DEBUG(&state->IR,"(If %d > %d then PC = PC + %d * 4)\n",r1,r2,c);
	if(r1 > r2) {
		state->PC = state->PC + c*4;
	} else {
		state->PC = state->PC + 4;
	}
	return SUCCESS;
}

int ble(struct IMPSState * state) {
	int r1 = readRegister(state, extractFromInstruction(r1Mask, state->IR));
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int c = extractFromInstruction(immediateMask, state->IR);
      	LOG_DEBUG(&state->IR,"(If %d <= %d then PC = PC + %d * 4)\n",r1,r2,c);
	if(r1 <= r2) {
		state->PC = state->PC + c*4;
	} else {
		state->PC = state->PC + 4;
	}
	return SUCCESS;
}

int bge(struct IMPSState * state) {
	int r1 = readRegister(state, extractFromInstruction(r1Mask, state->IR));
	int r2 = readRegister(state, extractFromInstruction(r2Mask, state->IR));
	int c = extractFromInstruction(immediateMask, state->IR);
      	LOG_DEBUG(&state->IR,"(If %d >= %d then PC = PC + %d * 4)\n",r1,r2,c);
	if(r1 >= r2) {
		state->PC = state->PC + c*4;
	} else {
		state->PC = state->PC + 4;
	}
	return SUCCESS;
}

int jmp(struct IMPSState * state) {
	int a = extractFromInstruction(addressMask, state->IR);
      	LOG_DEBUG(&state->IR,"([JMP] PC = %d)\n",a);
	state->PC = a;
	return SUCCESS;
}

int jr(struct IMPSState * state) {
	int r1 = readRegister(state, extractFromInstruction(r1Mask, state->IR));
      	LOG_DEBUG(&state->IR,"([JR] PC = %d)\n",r1);
	state->PC = r1;
	return SUCCESS;
}

int jal(struct IMPSState * state) {
	int a = extractFromInstruction(addressMask, state->IR);
      	LOG_DEBUG(&state->IR,"(R31 = PC + 4; PC = %d)\n",a);
	writeRegister(state, 31, state->PC+4);
	state->PC = a;
	return SUCCESS;
}

int main( int argc, char **argv) {

	struct IMPSState *state = initState();

	int (*OpCodeFunction[18]) (struct IMPSState * state);
	OpCodeFunction[0] = &halt;
	OpCodeFunction[1] = &add;
	OpCodeFunction[2] = &addi;
	OpCodeFunction[3] = &sub;
	OpCodeFunction[4] = &subi;
	OpCodeFunction[5] = &mul;
	OpCodeFunction[6] = &muli;
	OpCodeFunction[7] = &lw;
	OpCodeFunction[8] = &sw;
	OpCodeFunction[9] = &beq;
	OpCodeFunction[10] = &bne;
	OpCodeFunction[11] = &blt;
	OpCodeFunction[12] = &bgt;
	OpCodeFunction[13] = &ble;
	OpCodeFunction[14] = &bge;
	OpCodeFunction[15] = &jmp;
	OpCodeFunction[16] = &jr;
	OpCodeFunction[17] = &jal;

	readInstructions(state, argv[1]);
	
	int result = SUCCESS;
	while(result != HALT) {
		state->IR = readMemory(state, state->PC);
		int OpCode = extractFromInstruction(opCodeMask, state->IR);
		result = (*OpCodeFunction[checkOpCode(OpCode)])(state);
	}
	state->PC = state->PC + 4;
	dumpState(state);
	free(state);
	return 0;
}
