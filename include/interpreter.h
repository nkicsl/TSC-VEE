#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include <stdbool.h>

#include "operation.h"
#include "memory.h"
#include "stack.h"
#include "contract.h"

struct Config {
	//bool Debug;
	//tracer;
	//bool NoRecursion;
	//bool EnablePreimageRecording;
	struct Operation* JumpTable;
	char* EWASMInterpreter;
	char* EVMInterpreter;
	int* ExtraEips;
};

struct Interpreter {
	struct EVM* evm;
	struct Config cfg;
	//hasher;
	//hasherBuf;
	bool readOnly;
	byte* returnData;
};

void Interpreter_NewInterpreter(struct Interpreter* i, struct EVM* evm, struct Config cfg);
struct Result Interpreter_Run(struct Interpreter* i, struct Contract* contract, byte* input, bool readOnly);

#endif