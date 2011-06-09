#include "list.h"
#include <stdlib.h>
#include <stdio.h>

listItem lCreateItem(aInstruction assemblyInstruction, listItem next) {
	listItem newItem = (listItem)malloc(sizeof(struct _listItem));
	if(newItem == NULL) {
		perror("malloc"); exit(1);
	}
	newItem->assemblyInstruction = assemblyInstruction;
	newItem->next = next;
	return newItem;
}

listItem lInsertTail(aInstruction assemblyInstruction, listItem item) {
	if( item == NULL ) {
		listItem newItem = lCreateItem(assemblyInstruction, NULL);
		return newItem;
	}
	item->next = lInsertTail(assemblyInstruction, item->next);
	return item;
}

void freeList(listItem item) {
	if(item == NULL) return;
	freeList(item->next);
	free(item);
}

void printInstruction(aInstruction inst) {
	if( inst != NULL ) {
		if( inst->label != NULL ) {
			printf("Label: %s\n", inst->label);
		}
		printf("Line: %d; Opcode: %s\n", inst->line, inst->opcode);
		printf("R1: %d\n", inst->r1);
		printf("R2: %d\n", inst->r2);
		printf("R3: %d\n", inst->r3);
		if( inst->address != NULL ) {
			printf("Address: %s\n", inst->address);
		}
		printf("\n");
	}
}

void printList(listItem item) {
	if(item==NULL) return;
	printInstruction(item->assemblyInstruction);
	printList(item->next);
}
