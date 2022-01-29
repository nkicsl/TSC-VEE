#ifndef  _INSTRUCTIONS_H
#define  _INSTRUCTIONS_H

#include <stdint.h>
#include "interpreter.h"

typedef unsigned char byte;

struct Result opAdd(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opSub(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opMul(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opDiv(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opSdiv(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opMod(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opSmod(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opExp(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opSignExtend(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opLt(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opGt(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opSlt(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opSgt(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opEq(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opIszero(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opAnd(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opOr(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opXor(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opNot(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opByte(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opAddmod(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opMulmod(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opSHL(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opSHR(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opSAR(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opSha3(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opAddress(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
/*
struct Result opBalance(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opOrigin(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opCaller(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opCallValue(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opCallDataCopy(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opReturnDataSize(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opReturnDataCopy(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opExtCodeSize(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opCodeSize(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opCodeCopy(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opExtCodeCopy(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opExtCodeHash(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opGasprice(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opBlockhash(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opCoinbase(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opTimestamp(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opNumber(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opDifficulty(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opGasLimit(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
*/
struct Result opCallDataSize(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opCallDataLoad(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opPop(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opMload(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opMstore(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opMstore8(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opSload(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opSstore(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opJump(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opJumpi(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opJumpdest(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opPc(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opMsize(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opGas(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opPush1(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opCreate(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opCreate2(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opCall(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opCallCode(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opDelegateCall(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
//struct Result opStaticCall(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opReturn(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opRevert(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opStop(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opSuicide(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result makeLog(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opPush2(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opPush3(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opPush4(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opPush29(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opDup1(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);
struct Result opSwap1(uint64_t* pc, struct Interpreter* interpreter, struct ScopeContext* scopecontext);

#endif