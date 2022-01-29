#ifndef _OPERATION_H
#define _OPERATION_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef unsigned char byte;

struct Result {
	byte* ret;
	byte* err;
};

struct MemRes {
	uint64_t val;
	bool err;
};

struct ScopeContext {
	struct Memory* m;
	struct Stack* s;
	struct Contract* c;
};

struct Operation {
	struct Result(*execute) (uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
	uint64_t constantGas;
	struct MemRes(*memorySize) (struct Stack* s);
	int minS;
	int maxS;
	bool halts;
	bool jumps;
	bool writes;
	bool reverts;
	bool returns;
};

struct Operation* Operation_JumpTable();

int maxStack(int pop, int push);

int minStack(int pops, int push);

int minDupStack(int n);

int maxDupStack(int n);

struct MemRes memoryMLoad(struct Stack* s);

struct MemRes memoryMStore(struct Stack* s);

struct MemRes memoryMStore8(struct Stack* s);

struct MemRes memoryReturn(struct Stack* s);

struct MemRes calcMemSize64(struct uint256_t* off, struct uint256_t* l);

struct MemRes calcMemSize64WithUint(struct uint256_t* off, uint64_t length64);

#endif

