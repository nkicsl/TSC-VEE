#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "evm.h"
#include "interpreter.h"

void Interpreter_NewInterpreter(struct Interpreter* i, struct EVM* evm, struct Config cfg) {
	i->evm = evm;
	i->cfg = cfg;
}
struct Result Interpreter_Run(struct Interpreter* i, struct Contract* contract, byte* input, bool readOnly) {
	struct Result res;
	res.ret = NULL;
	res.err = NULL;
	i->evm->depth++;
	i->returnData = NULL;
	if (contract->Code == NULL) {
		return res;
	}
	OpCode op;
	struct Memory* m = (struct Memory*)malloc(sizeof(struct Memory) * 1024);
	Memory_NewMemory(m,1024);
	struct Stack* s = (struct Stack*)malloc(sizeof(struct Stack) * 1024);
	Stack_newStack(s, 1024);
	struct ScopeContext callContext = { m, s, contract };
	uint64_t pc = 0;
	//uint64_t pcCopy;

	contract->Input = input;

	for (int steps = 0;;) {
		steps++;
		if (steps % 1000 == 0 && i->evm->abort != 0) {
			break;
		}
		//pcCopy = pc;
		op = Contract_GetOp(contract, pc);
		//i->cfg.JumpTable = Operation_JumpTable();
		struct Operation operation = i->cfg.JumpTable[op];
		if (operation.execute == NULL) {
			res.err = "ErrInvalidOpCode";
			//res.err = strcat("ErrInvalidOpCode{ Opcode: " + op, "}\n");
			return res;
		}
		int sLen = Stack_len(s);
		if (sLen < operation.minS) {
			/*char* str1 = "ErrStackUnderflow{ stackLen: " + sLen;
			char* str2 = ", required: " + operation.minS;
			res.err = strcat(strcat(str1, str2), "}\n");*/
			res.err = "ErrStackUnderflow";
			return res;
		}
		else if (sLen > operation.maxS) {
			/*char* str1 = "ErrStackOverflow{ stackLen: " + sLen;
			char* str2 = ", limit: " + operation.maxS;
			res.err = strcat(strcat(str1, str2), "}\n");*/
			res.err = "ErrStackOverflow";
			return res;
		}

		
		if (operation.memorySize == memoryMLoad || operation.memorySize == memoryMStore || operation.memorySize == memoryMStore8 || operation.memorySize == memoryReturn) {
			struct MemRes mr;
			mr = operation.memorySize(s);
			if (mr.err) {
				res.err = "ErrGasUintOverflow";
				return res;
			}
		}
		/*
		uint64_t memorySize;
		if (memorySize > 0) {
			Memory_Resize(m, memorySize);
		}*/

		res = operation.execute(&pc, i, &callContext);
		if (operation.returns) {
			i->returnData = res.ret;
		}

		if (res.err != NULL) {
			return res;
		}
		if (operation.reverts) {
			res.err = "ErrExecutionReverted";
			return res;
		}
		if (operation.halts) {
			res.err = NULL;
			return res;
		}
		if (!operation.jumps) {
			pc++;
		}
	}
	res.ret = NULL;
	res.err = NULL;
	return res;
}