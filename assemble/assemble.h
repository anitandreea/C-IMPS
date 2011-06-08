typedef struct assemblyInstruction {
	char * opcode;
	char * label;
	int r1;
	int r2;
	int r3;
	int immediate;
	int address;
} * aInstruction;

typedef void (*assembleFunction)(aInstruction instruction);

union labelReference {
	int line;
	assembleFunction assemble;
} labelRef;	
