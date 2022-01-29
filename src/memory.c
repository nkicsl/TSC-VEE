#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

void Memory_NewMemory(struct Memory* m, uint64_t maxsize) {
	m->store = (byte*)malloc(sizeof(byte) * maxsize);
	for (int i = 0; i < maxsize; i++) {
		m->store[i] = (byte)0;
	}
}
void Memory_Set(struct Memory* m, uint64_t offset, uint64_t size, byte* value) {
	if (size > 0) {
		if ((offset + size) > 1024) {
			printf("Invalid memory: store empty");
		}
		memcpy(m->store + offset, value, size);
	}
}
void Memory_Set32(struct Memory* m, uint64_t offset, struct uint256_t* val) {
	if ((offset + 32) > 1024) {
		printf("invalid memory: store empty");
	}
	for (int i = 0; i < 32; i++) {
		m->store[offset + i] = 0;
	}
	memcpy(m->store + offset, val, sizeof(val));

}
void Memory_Resize(struct Memory* m, uint64_t size) {
	if ((uint64_t)Memory_Len(m) < size) {

		byte* buffer = (byte*)malloc(sizeof(byte) * size);
		memcpy(buffer, m->store, sizeof(m->store));
		free(m->store);
		m->store = buffer;
	}
}
byte* Memory_GetCopy(struct Memory* m, uint64_t offset, uint64_t size) {
	if (size == 0) {
		return NULL;
	}
	if (sizeof(m->store) > sizeof(int)* offset) {
		byte* cpy = (byte*)malloc(sizeof(byte) * size);
		memcpy(cpy, m->store + offset, size);
		return cpy;
	}
	return NULL;
}
byte* Memory_GetPtr(struct Memory* m, uint64_t offset, uint64_t size) {
	if (size == 0) {
		return NULL;
	}
	if (sizeof(m->store) > sizeof(int) * offset) {
		return m->store + offset;
	}
	return NULL;
}
int Memory_Len(struct Memory* m) {
	return sizeof(m->store) / sizeof(byte*);
}
byte* Memory_Data(struct Memory* m) {
	return m->store;
}
void Memory_Print(struct Memory* m) {
	printf("### mem %d bytes ###\n", (int)(sizeof(m->store)/sizeof(byte*)));
	if ((int)sizeof(m->store) / sizeof(byte*) > 0) {
		int addr = 0;
		for (int i = 0; i + 32 <= (int)(sizeof(m->store) / sizeof(byte*)); i += 32) {
			byte* tmp = (byte*)malloc(sizeof(byte) * 32);
			memcpy(tmp, m->store + i, 32);
			printf("%03d: %s\n", addr, tmp);
			addr++;
		}
	}
	else {
		printf("-- empty --\n");
	}
	printf("####################\n");
}