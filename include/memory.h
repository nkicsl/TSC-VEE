#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdint.h>
#include "operation.h"

typedef unsigned char byte;

struct Memory {
	byte* store;
	//uint64_t lastGasCost;
};

void Memory_NewMemory(struct Memory* m, uint64_t maxsize);
void Memory_Set(struct Memory* m, uint64_t offset, uint64_t size, byte* value);
void Memory_Set32(struct Memory* m, uint64_t offset, struct uint256_t* val);
void Memory_Resize(struct Memory* m, uint64_t size);
byte* Memory_GetCopy(struct Memory* m, uint64_t offset, uint64_t size);
byte* Memory_GetPtr(struct Memory* m, uint64_t offset, uint64_t size);
int Memory_Len(struct Memory* m);
byte* Memory_Data(struct Memory* m);
void Memory_Print(struct Memory* m);

#endif