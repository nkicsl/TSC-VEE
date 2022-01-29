#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "evm.h"

void HexToByte(byte* tmp, byte* target) {
	int length = strlen(tmp) / 2;
	uint8_t res = 0;
	for (int i = 0; i < length; i++) {
		res = 0;
		if (tmp[2 * i] >= '0' && tmp[2 * i] <= '9') {
			res += (uint8_t)((tmp[2 * i] - '0') * 16);
		}
		else {
			res += (uint8_t)((tmp[2 * i] - 'a' + 10) * 16);
		}
		if (tmp[2 * i + 1] >= '0' && tmp[2 * i + 1] <= '9') {
			res += (uint8_t)(tmp[2 * i + 1] - '0');
		}
		else {
			res += (uint8_t)(tmp[2 * i + 1] - 'a' + 10);
		}
		target[i] = (byte)res;
	}
}

int main()
{
	/*struct uint256_t one = { 0,0,0,1 };
	struct uint256_t two = { 0,0,0,20 };
	struct uint256_t rd;
	struct uint256_t rm;
	divmod256(&two, &one, &rd, &rm);
	uint32_t outlen = 32;
	char* out = (char*)malloc(sizeof(char) * outlen);
	uint32_t base = 10;
	tostring256(&rd,base,out,outlen);
	printf("It is %s", out);*/
	
	struct Address cb;
	cb.addr = "0x64902116fc387b1f9885a535f28a235367dd8926";
	struct BlockContext blockCtx = { cb, 0, 0, 0, 0 };
	struct TxContext txCtx = { cb, 0 };
	struct Operation* JumpTable = Operation_JumpTable();
	struct Config vmConfig = { Operation_JumpTable(), NULL, NULL, NULL };
	struct EVM* evm = (struct EVM*)malloc(sizeof(struct EVM));
	EVM_NewEVM(evm, blockCtx, txCtx, vmConfig);

	struct ContractRef caller;
	caller.addr.addr = "0x53F73d5519C002eF422B7eFC41c023AaF4889159";

	struct Address addr;
	addr.addr = "0x64902116fc387b1f9885a535f28a235367dd8926";

	byte* codetmp = "608060405260043610603f576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff1680631003e2d2146044575b600080fd5b348015604f57600080fd5b50606c600480360381019080803590602001909291905050506082565b6040518082815260200191505060405180910390f35b60009190505600a165627a7a72305820b2d984d9228e0cfe2950fb67580e382f88e808e7bf9be677c79da28426a4de940029";
	byte* code = (struct byte*)malloc(sizeof(byte) * strlen(codetmp) / 2);
	HexToByte(codetmp, code);

	struct Hash codehash;
	codehash.hash = "0xce9b0e08f1a1877e27c8276fce184a576874e59dcf6390c9fdeaa2f90a04fa24";

	byte* inputtmp = "1003e2d20000000000000000000000000000000000000000000000000000000000000001";
	byte* input = (struct byte*)malloc(sizeof(byte) * strlen(inputtmp) / 2);
	HexToByte(inputtmp, input);
	/*for (int i = 63; i < 100; i++) {
		printf("%d: %u\n", i, code[i]);
	}*/
	struct uint256_t value = { 0 };

	EVM_Call(evm, caller, addr, codetmp, codehash, inputtmp, value);
	return 0;
	
}