#ifndef _EVM_H
#define _EVM_H

#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"
#include "contract.h"
#include "uint256.h"

struct BlockContext {
	struct Address Coinbase;
	uint64_t GasLimit;
	struct uint256_t BlockNumber;
	struct uint256_t Time;
	struct uint256_t Difficulty;
};

struct TxContext {
	struct Address Origin;
	struct uint256_t GasPrice;
};

struct EVM {
	struct BlockContext context;
	struct TxContext tx;
	int depth;
	struct Config vmConfig;
	struct Interpreter* interpreters;
	struct Interpreter interpreter;
	int32_t abort;
	uint64_t callGasTemp;
};

void EVM_NewEVM(struct EVM* evm, struct BlockContext blockCtx, struct TxContext txCtx, struct Config vmConfig);

void EVM_Reset(struct EVM* evm, struct TxContext txCtx);

void EVM_Cancel(struct EVM* evm);

bool EVM_Cancelled(struct EVM* evm);

struct Interpreter EVM_Interpreter(struct EVM* evm);

struct Result EVM_Run(struct EVM* evm, struct Contract* contract, byte* input, bool readonly);

struct Result EVM_Call(struct EVM* evm, struct ContractRef caller, struct Address addr, byte* code, struct Hash codehash, byte* input, struct uint256_t value);

struct Result EVM_CallCode(struct EVM* evm, struct ContractRef caller, struct Address addr, byte* code, struct Hash codehash, byte* input, struct uint256_t value);

//struct Result EVM_DelegateCall(struct EVM* evm, struct ContractRef caller, struct Address addr, byte* code, struct Hash codehash, byte* input);

//struct Result EVM_StaticCall(struct EVM* evm, struct ContractRef caller, struct Address addr, byte* code, struct Hash codehash, byte* input);

#endif