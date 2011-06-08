#ifndef ASSEMBLE_H
#define ASSEMBLE_H

typedef struct assemblyInstruction {
	char opcode[5];
	char * label;
	int r1;
	int r2;
	int r3;
	char * address;
	int line;
} * aInstruction;

typedef void (*assembleFunction)(aInstruction instruction);

#endif
