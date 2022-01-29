#include <string.h>
#include "instructions.h"
#include "uint256.h"

struct Result opAdd(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t t;
	add256(y, &x, &t);
	copy256(y, &t);
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opSub(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t t;
	minus256(y, &x, &t);
	copy256(y, &t);
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opMul(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t t;
	mul256(y, &x, &t);
	copy256(y, &t);
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opDiv(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t t;
	struct uint256_t m;
	uint32_t outlen = 32;
	char out[32] = "";
	uint32_t base = 2;
	/*
	tostring256(&x, base, out, outlen);
	printf("x is: %s", out);
	tostring256(y, base, out, outlen);
	printf("y is: %s", out);
	*/
	divmod256(y, &x, &t, &m);
	copy256(y, &t);
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opSdiv(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t t;
	struct uint256_t m;
	divmod256(y, &x, &t, &m);
	copy256(y, &t);
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opMod(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t t;
	struct uint256_t m;
	divmod256(y, &x, &t, &m);
	copy256(y, &m);
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opSmod(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t t;
	struct uint256_t m;
	divmod256(y, &x, &t, &m);
	copy256(y, &m);
	r.err = NULL;
	r.ret = NULL;
	return r;
}

/*
struct Result opExp(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s) {

}
struct Result opSignExtend(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s) {

}
*/

struct Result opNot(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t* x = Stack_peek(scopecontext->s);
	struct uint256_t t;
	not256(x, &t);
	copy256(x, &t);
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opLt(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t tmp = { 0, 0, 0, 1 };
	if (gt256(y, &x)) {
		copy256(y, &tmp);
	}
	else {
		clear256(y);
	}
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opGt(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t tmp = { 0, 0, 0, 1 };
	if (gt256(&x, y)) {
		copy256(y, &tmp);
	}
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opSlt(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t tmp = { 0, 0, 0, 1 };
	if (gt256(y, &x)) {
		copy256(y, &tmp);
	}
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opSgt(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t tmp = { 0, 0, 0, 1 };
	if (gt256(&x, y)) {
		copy256(y, &tmp);
	}
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opEq(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t tmp;
	struct uint256_t t;
	clear256(&tmp);
	clear256(&t);
	if (equal256(y, &x)) {
		not256(&tmp, &t);
	}
	copy256(y, &t);
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opIszero(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t* x = Stack_peek(scopecontext->s);
	struct uint256_t tmp;
	struct uint256_t t;
	clear256(&tmp);
	clear256(&t);
	if (zero256(x)) {
		not256(&tmp, &t);
	}
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opAnd(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t t;
	and256(y, &x, &t);
	copy256(y, &t);
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opOr(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t t;
	or256(y, &x, &t);
	copy256(y, &t);
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opXor(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t* y = Stack_peek(scopecontext->s);
	struct uint256_t t;
	xor256(y, &x, &t);
	copy256(y, &t);
	r.err = NULL;
	r.ret = NULL;
	return r;
}

/*
struct Result opByte(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t th = Stack_pop(scopecontext->s);
	struct uint256_t* val = Stack_peek(scopecontext->s);
	struct uint256_t t;
	xor256(y, &x, &t);
	copy256(y, &t);
	r.err = NULL;
	r.ret = NULL;
	return r;
}*/

struct Result opAddmod(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t y = Stack_pop(scopecontext->s);
	struct uint256_t* z = Stack_peek(scopecontext->s);
	if (zero256(z)) {
		clear256(z);
	}
	else {
		struct uint256_t tmp;
		struct uint256_t t;
		struct uint256_t m;
		add256(&y, &x, &tmp);
		divmod256(&tmp, z, &t, &m);
		copy256(z, &m);
	}
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opMulmod(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	struct uint256_t x = Stack_pop(scopecontext->s);
	struct uint256_t y = Stack_pop(scopecontext->s);
	struct uint256_t* z = Stack_peek(scopecontext->s);
	struct uint256_t tmp;
	mul256(&y, &x, &tmp);
	struct uint256_t t;
	struct uint256_t m;
	divmod256(&tmp, z, &t, &m);
	copy256(z, &m);
	r.err = NULL;
	r.ret = NULL;
	return r;
}
/*
struct Result opSHL(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opSHR(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opSAR(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opSha3(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
*/

struct Result opAddress(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Address a = scopecontext->c->CallerAddress;
	uint64_t tmp1 = ((uint64_t)(uint8_t)a.addr[19] << 24) | ((uint64_t)(uint8_t)a.addr[18] << 16) | ((uint64_t)(uint8_t)a.addr[17] << 8) | ((uint64_t)(uint8_t)a.addr[16]);
	uint64_t tmp2 = ((uint64_t)(uint8_t)a.addr[15] << 56) | ((uint64_t)(uint8_t)a.addr[14] << 48) | ((uint64_t)(uint8_t)a.addr[13] << 40) | ((uint64_t)(uint8_t)a.addr[12] << 32) | ((uint64_t)(uint8_t)a.addr[11] << 24) | ((uint64_t)(uint8_t)a.addr[10] << 16) | ((uint64_t)(uint8_t)a.addr[9] << 8) | ((uint64_t)(uint8_t)a.addr[8]);
	uint64_t tmp3 = ((uint64_t)(uint8_t)a.addr[7] << 56) | ((uint64_t)(uint8_t)a.addr[6] << 48) | ((uint64_t)(uint8_t)a.addr[5] << 40) | ((uint64_t)(uint8_t)a.addr[4] << 32) | ((uint64_t)(uint8_t)a.addr[3] << 24) | ((uint64_t)(uint8_t)a.addr[2] << 16) | ((uint64_t)(uint8_t)a.addr[1] << 8) | ((uint64_t)(uint8_t)a.addr[0]);
	struct uint256_t address = { 0, tmp1, tmp2, tmp3 };
	Stack_push(scopecontext->s, address);
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}
/*
struct Result opBalance(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opOrigin(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opCaller(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opCallValue(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opCallDataCopy(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opReturnDataSize(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opReturnDataCopy(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opExtCodeSize(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opCodeSize(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opCodeCopy(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opExtCodeCopy(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opExtCodeHash(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opGasprice(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opBlockhash(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opCoinbase(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opTimestamp(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opNumber(uint64_t* pc, struct Interpreter* i, struct ScopeContext* s);
struct Result opDifficulty(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opGasLimit(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
*/
struct Result opCallDataLoad(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct uint256_t* x = Stack_peek(scopecontext->s);
	bool overflow = true;
	if (UPPER(LOWER_P(x)) == 0 && LOWER(UPPER_P(x)) == 0 && UPPER(UPPER_P(x)) == 0) {
		overflow = false;
	}
	uint64_t offset = LOWER(LOWER_P(x));
	if (!overflow) {
		uint8_t buff1[8];
		uint8_t buff2[8];
		uint8_t buff3[8];
		uint8_t buff4[8];
		for (int i = 0; i < 8; i++) {
			buff1[i] = (uint8_t)Contract_GetInput(scopecontext->c, offset + i);
		}
		for (int i = 8; i < 16; i++) {
			buff2[i - 8] = (uint8_t)Contract_GetInput(scopecontext->c, offset + i);
		}
		for (int i = 16; i < 24; i++) {
			buff3[i - 16] = (uint8_t)Contract_GetInput(scopecontext->c, offset + i);
		}
		for (int i = 24; i < 32; i++) {
			buff4[i - 24] = (uint8_t)Contract_GetInput(scopecontext->c, offset + i);
		}
		struct uint256_t tmp = { readUint64BE(buff1), readUint64BE(buff2) , readUint64BE(buff3) , readUint64BE(buff4) };
		copy256(x, &tmp);
	}
	else {
		clear256(x);
	}
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}
struct Result opCallDataSize(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	uint64_t length = (uint64_t)(strlen(scopecontext->c->Input) / 2);
	struct uint256_t tmp = { 0,0,0, length };
	Stack_push(scopecontext->s, tmp);
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}
struct Result opPop(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	Stack_pop(scopecontext->s);
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opMload(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct uint256_t* v = Stack_peek(scopecontext->s);
	int64_t offset = LOWER(LOWER_P(v));
	byte* mptr = Memory_GetPtr(scopecontext->m, offset, 32);
	int l = sizeof(mptr) / sizeof(byte*);
	if (l > 0 && l <= 8) {
		uint64_t tmp = 0;
		for (int i = l - 1; i >= 0; i--) {
			tmp |= (uint64_t)mptr[i] << (8 * i);
		}
		struct uint256_t t = { 0, 0, 0, tmp };
		copy256(v, &t);
	}
	else if (l > 8 && l <= 16) {
		uint64_t tmp1 = 0;
		uint64_t tmp2 = 0;
		for (int i = l - 1; i >= 8; i--) {
			tmp1 |= (uint64_t)mptr[i] << (8 * (i - 8));
		}
		for (int i = 7; i >= 0; i--) {
			tmp2 |= (uint64_t)mptr[i] << (8 * i);
		}
		struct uint256_t t = { 0, 0, tmp1, tmp2 };
		copy256(v, &t);
	}
	else if (l > 16 && l <= 24) {
		uint64_t tmp1 = 0;
		uint64_t tmp2 = 0;
		uint64_t tmp3 = 0;
		for (int i = l - 1; i >= 16; i--) {
			tmp1 |= (uint64_t)mptr[i] << (8 * (i - 16));
		}
		for (int i = 15; i >= 8; i--) {
			tmp2 |= (uint64_t)mptr[i] << (8 * (i - 8));
		}
		for (int i = 7; i >= 0; i--) {
			tmp3 |= (uint64_t)mptr[i] << (8 * i);
		}
		struct uint256_t t = { 0, tmp1, tmp2, tmp3 };
		copy256(v, &t);
	}
	else if (l > 24 && l <= 32) {
		uint64_t tmp1 = 0;
		uint64_t tmp2 = 0;
		uint64_t tmp3 = 0;
		uint64_t tmp4 = 0;
		for (int i = l - 1; i >= 24; i--) {
			tmp1 |= (uint64_t)mptr[i] << (8 * (i - 24));
		}
		for (int i = 23; i >= 16; i--) {
			tmp1 |= (uint64_t)mptr[i] << (8 * (i - 16));
		}
		for (int i = 15; i >= 8; i--) {
			tmp2 |= (uint64_t)mptr[i] << (8 * (i - 8));
		}
		for (int i = 7; i >= 0; i--) {
			tmp3 |= (uint64_t)mptr[i] << (8 * i);
		}
		struct uint256_t t = { tmp1, tmp2, tmp3, tmp4 };
		copy256(v, &t);
	}
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opMstore(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	struct uint256_t mStart = Stack_pop(scopecontext->s);
	struct uint256_t val = Stack_pop(scopecontext->s);
	Memory_Set32(scopecontext->m, (uint64_t)LOWER(LOWER(mStart)), &val);
	return r;
}
struct Result opMstore8(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opSload(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}
struct Result opSstore(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}
//struct Result opSstore(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
//	struct uint256_t loc = Stack_pop(scopecontext->s);
//	struct uint256_t val = Stack_pop(scopecontext->s);
//	//wait writing...
//	struct Result r;
//	r.err = NULL;
//	r.ret = NULL;
//	return r;
//}

struct Result opJump(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}
struct Result opJumpi(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct uint256_t pos = Stack_pop(scopecontext->s);
	struct uint256_t cond = Stack_pop(scopecontext->s);
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	if (!(LOWER(LOWER(cond)) == 0 && UPPER(LOWER(cond)) == 0 && LOWER(UPPER(cond)) == 0 && UPPER(UPPER(cond)) == 0)) {
		if (!Contract_validJumpdest(scopecontext->c, pos)) {
			r.ret = NULL;
			r.err = "ErrInvalidJump";
		}
		(*pc) = LOWER(LOWER(pos));
	}
	else {
		(*pc)++;
	}
	return r;
}

struct Result opJumpdest(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opPc(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct uint256_t pcvalue = { 0, 0, 0, *pc };
	Stack_push(scopecontext->s, pcvalue);
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opMsize(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct uint256_t msize = { 0, 0, 0, (uint64_t)Memory_Len(scopecontext->m) };
	Stack_push(scopecontext->s, msize);
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opGas(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}

struct Result opPush1(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	(*pc) += 1;
	struct uint256_t code = { 0,0,0,(uint64_t)(uint8_t)Contract_GetOp(scopecontext->c, *pc) };
	Stack_push(scopecontext->s, code);
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}

//struct Result opCreate(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opCreate2(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opCall(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opCallCode(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opDelegateCall(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opStaticCall(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);

struct Result opReturn(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct uint256_t offset = Stack_pop(scopecontext->s);
	struct uint256_t size = Stack_pop(scopecontext->s);
	byte* ret = Memory_GetPtr(scopecontext->m, LOWER(LOWER(offset)), LOWER(LOWER(size)));
	struct Result r;
	r.err = NULL;
	r.ret = ret;
	return r;
}

struct Result opRevert(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct uint256_t offset = Stack_pop(scopecontext->s);
	struct uint256_t size = Stack_pop(scopecontext->s);
	byte* ret = Memory_GetPtr(scopecontext->m, LOWER(LOWER(offset)), LOWER(LOWER(size)));
	struct Result r;
	r.err = NULL;
	r.ret = ret;
	return r;
}

struct Result opStop(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	return r;
}

//struct Result opSuicide(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//
//struct Result makeLog(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opPush2(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	int codelen = strlen(scopecontext->c->Code) / 2;
	int startMin = codelen;
	if ((int)((*pc) + 1) < startMin) {
		startMin = (int)((*pc) + 1);
	}
	int endMin = codelen;
	if (startMin + 2 < endMin) {
		endMin = startMin + 2;
	}
	for (int i = startMin; i < endMin; i++) {
		struct uint256_t tmp = { 0,0,0,(uint64_t)(uint8_t)Contract_GetOp(scopecontext->c, i) };
		Stack_push(scopecontext->s, tmp);
	}
	(*pc) += 2;
	return r;
}
struct Result opPush3(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	int codelen = strlen(scopecontext->c->Code) / 2;
	int startMin = codelen;
	if ((int)((*pc) + 1) < startMin) {
		startMin = (int)((*pc) + 1);
	}
	int endMin = codelen;
	if (startMin + 3 < endMin) {
		endMin = startMin + 3;
	}
	for (int i = startMin; i < endMin; i++) {
		struct uint256_t tmp = { 0,0,0,(uint64_t)(uint8_t)Contract_GetOp(scopecontext->c, i) };
		Stack_push(scopecontext->s, tmp);
	}
	(*pc) += 3;
	return r;
}
struct Result opPush4(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	int codelen = strlen(scopecontext->c->Code) / 2;
	int startMin = codelen;
	if ((int)((*pc) + 1) < startMin) {
		startMin = (int)((*pc) + 1);
	}
	int endMin = codelen;
	if (startMin + 4 < endMin) {
		endMin = startMin + 4;
	}
	for (int i = startMin; i < endMin; i++) {
		struct uint256_t tmp = { 0,0,0,(uint64_t)(uint8_t)Contract_GetOp(scopecontext->c, i) };
		Stack_push(scopecontext->s, tmp);
	}
	(*pc) += 4;
	return r;
}
struct Result opPush29(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	struct Result r;
	r.err = NULL;
	r.ret = NULL;
	int codelen = strlen(scopecontext->c->Code) / 2;
	int startMin = codelen;
	if ((int)((*pc) + 1) < startMin) {
		startMin = (int)((*pc) + 1);
	}
	int endMin = codelen;
	if (startMin + 29 < endMin) {
		endMin = startMin + 29;
	}
	for (int i = startMin; i < endMin; i++) {
		struct uint256_t tmp = { 0,0,0,(uint64_t)(uint8_t)Contract_GetOp(scopecontext->c, i) };
		Stack_push(scopecontext->s, tmp);
	}
	(*pc) += 29;
	return r;
}
struct Result opDup1(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	Stack_dup(scopecontext->s, 1);
	struct Result r;
	r.ret = NULL;
	r.err = NULL;
	return r;
}
struct Result opSwap1(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext) {
	Stack_swap(scopecontext->s, 2);
	struct Result r;
	r.ret = NULL;
	r.err = NULL;
	return r;
}