#ifndef _CONTRACT_H
#define _CONTRACT_H

#include <stdint.h>
#include "hash.h"
#include "uint256.h"
#include "opcode.h"

struct Address {
	byte* addr;
};

struct ContractRef {
	struct Address addr;
};

struct Contract {
	struct Address CallerAddress;
	struct ContractRef caller;
	struct ContractRef self;
	//byte* jumpdests;
	//byte* analysis;
	byte* Code;
	struct Hash CodeHash;
	struct Address* CodeAddr;
	byte* Input;
	struct uint256_t value;
};

void Contract_NewContract(struct Contract* c, struct ContractRef caller, struct ContractRef object, struct uint256_t value);
bool Contract_validJumpdest(struct Contract* c, struct uint256_t dest);
//bool Contract_isCode(Contract *c, uint64_t udest);
//Contract* Contract_AsDelegate(Contract *c);
OpCode Contract_GetOp(struct Contract* c, uint64_t n);
OpCode Contract_GetInput(struct Contract* c, uint64_t n);
byte Contract_GetByte(struct Contract* c, uint64_t n);
byte Contract_GetInputByte(struct Contract* c, uint64_t n);
struct Address Contract_Caller(struct Contract* c);
struct Address Contract_Address(struct Contract* c);
struct uint256_t Contract_Value(struct Contract* c);
void Contract_SetCallCode(struct Contract* c, struct Address* addr, struct Hash hash, byte* code);
//void Contract_SetCodeOptionalHash(Contract *c, Address *addr, CodeAndHash);

#endif