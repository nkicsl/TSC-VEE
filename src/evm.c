#include <stdlib.h>
#include "evm.h"

void EVM_NewEVM(struct EVM* evm, struct BlockContext blockCtx, struct TxContext txCtx, struct Config vmConfig) {
	evm->context = blockCtx;
	evm->tx = txCtx;
	evm->vmConfig = vmConfig;
	Interpreter_NewInterpreter(&evm->interpreter, evm, vmConfig);
}

void EVM_Reset(struct EVM* evm, struct TxContext txCtx) {
	evm->tx = txCtx;
}

void EVM_Cancel(struct EVM* evm) {
	evm->abort = 1;
}

bool EVM_Cancelled(struct EVM* evm) {
	return evm->abort == 1;
}

struct Interpreter EVM_Interpreter(struct EVM* evm) {
	return evm->interpreter;
}

struct Result EVM_Run(struct EVM* evm, struct Contract* contract, byte* input, bool readOnly) {
	return Interpreter_Run(&evm->interpreter, contract, input, readOnly);
}

struct Result EVM_Call(struct EVM* evm, struct ContractRef caller, struct Address addr, byte* code, struct Hash codehash, byte* input, struct uint256_t value) {
	struct Result res;
	if (evm->depth > 1024) {
		res.ret = NULL;
		res.err = "ErrDepth";
		return res;
	}
	if (code == NULL) {
		res.ret = NULL;
		res.err = NULL;
		return res;
	}
	else {
		struct Address addrCopy = addr;
		struct ContractRef cr;
		cr.addr = addrCopy;
		struct Contract* contract = (struct Contract*)malloc(sizeof(struct Contract));
		Contract_NewContract(contract, caller, cr, value);
		Contract_SetCallCode(contract, &addrCopy, codehash, code);
		res = EVM_Run(evm, contract, input, false);
		return res;
	}
}

struct Result EVM_CallCode(struct EVM* evm, struct ContractRef caller, struct Address addr, byte* code, struct Hash codehash, byte* input, struct uint256_t value) {
	struct Result res;
	if (evm->depth > 1024) {
		res.ret = NULL;
		res.err = "ErrDepth";
		return res;
	}
	struct Address addrCopy = addr;
	struct ContractRef cr;
	cr.addr = caller.addr;
	struct Contract* contract = (struct Contract*)malloc(sizeof(struct Contract));
	Contract_NewContract(contract, caller, cr, value);
	Contract_SetCallCode(contract, &addrCopy, codehash, code);
	res = EVM_Run(evm, contract, input, false);
	return res;
}

//Result EVM_DelegateCall(EVM* evm, ContractRef caller, Address addr, byte* code, Hash codehash, byte* input, uint64_t gas);

//Result EVM_StaticCall(EVM* evm, ContractRef caller, Address addr, byte* code, Hash codehash, byte* input, uint64_t gas);
