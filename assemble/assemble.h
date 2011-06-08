typedef struct assemblyInstruction {
	char opcode[5];
	char * label;
	int r1;
	int r2;
	int r3;
	char * immediate;
	char * address;
} * aInstruction;

typedef void (*assembleFunction)(aInstruction instruction);

typedef union labelReference {
	int line;
	assembleFunction assemble;
} *labelRef;	
