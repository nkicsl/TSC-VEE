#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "contract.h"

void Contract_NewContract(struct Contract* c, struct ContractRef caller, struct ContractRef object, struct uint256_t value) {
	c->CallerAddress = caller.addr;
	c->caller = caller;
	c->self = object;
	c->value = value;
}
bool Contract_validJumpdest(struct Contract* c, struct uint256_t dest) {
	bool overflow = true;
	if (UPPER(LOWER(dest)) == 0 && LOWER(UPPER(dest)) == 0 && UPPER(UPPER(dest)) == 0) {
		overflow = false;
	}
	uint64_t udest = LOWER(LOWER(dest));
	if (overflow || udest >= (uint64_t)(strlen(c->Code) / 2)) {
		return false;
	}
	if (!(c->Code[2 * udest] == '5' && c->Code[2 * udest + 1] == 'b')) {
		return false;
	}
	return true;
}
//bool Contract_isCode(Contract* c, uint64_t udest);
//Contract* Contract_AsDelegate(Contract* c);

OpCode Contract_GetOp(struct Contract* c, uint64_t n) {
	return (OpCode)Contract_GetByte(c, n);
}
OpCode Contract_GetInput(struct Contract* c, uint64_t n) {
	return (OpCode)Contract_GetInputByte(c, n);
}
byte Contract_GetByte(struct Contract* c, uint64_t n) {
	if (c->Code[2 * n] != NULL && c->Code[2 * n + 1] != NULL) {
		uint8_t res = 0;
		if (c->Code[2 * n] >= '0' && c->Code[2 * n] <= '9') {
			res += (c->Code[2 * n] - '0') * 16;
		}
		else {
			res += (c->Code[2 * n] - 'a' + 10) * 16;
		}
		if (c->Code[2 * n +1 ] >= '0' && c->Code[2 * n + 1] <= '9') {
			res += c->Code[2 * n + 1] - '0';
		}
		else {
			res += c->Code[2 * n + 1] - 'a' + 10;
		}
		return (byte)res;
	}
	return 0;
}
byte Contract_GetInputByte(struct Contract* c, uint64_t n) {
	if (c->Input[2 * n] != NULL && c->Input[2 * n + 1] != NULL) {
		uint8_t res = 0;
		if (c->Input[2 * n] >= '0' && c->Input[2 * n] <= '9') {
			res += (c->Input[2 * n] - '0') * 16;
		}
		else {
			res += (c->Input[2 * n] - 'a' + 10) * 16;
		}
		if (c->Input[2 * n + 1] >= '0' && c->Input[2 * n + 1] <= '9') {
			res += c->Input[2 * n + 1] - '0';
		}
		else {
			res += c->Input[2 * n + 1] - 'a' + 10;
		}
		return (byte)res;
	}
	return 0;
}
struct Address Contract_Caller(struct Contract* c) {
	return c->CallerAddress;
}
struct Address Contract_Address(struct Contract* c) {
	return c->self.addr;
}
struct uint256_t Contract_Value(struct Contract* c) {
	return c->value;
}
void Contract_SetCallCode(struct Contract* c, struct Address* addr, struct Hash hash, byte* code) {
	c->Code = code;
	c->CodeHash = hash;
	c->CodeAddr = addr;
}
//void Contract_SetCodeOptionalHash(Contract* c, Address* addr, CodeAndHash);