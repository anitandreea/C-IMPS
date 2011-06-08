#include "assemble.h"

typedef struct _listItem * listItem;

struct _listItem {
	aInstruction assemblyInstruction;
	listItem next;
};

listItem lCreateItem(aInstruction assemblyInstruction, listItem next);

listItem lInsertTail(aInstruction assemblyInstruction, listItem item);

void freeList(listItem item);

void printInstruction(aInstruction inst);

void printList(listItem item);
