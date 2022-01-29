#include <stdlib.h>
#include "operation.h"
#include "instructions.h"

int maxStack(int pop, int push) {
	return 1024 + pop - push;
}

int minStack(int pops, int push) {
	return pops;
}

int minDupStack(int n) {
	return minStack(n, n + 1);
}

int maxDupStack(int n) {
	return maxStack(n, n + 1);
}

struct MemRes calcMemSize64(struct uint256_t* off, struct uint256_t* l) {
	struct MemRes mr;
	if (UPPER(UPPER_P(l)) != 0 || UPPER(LOWER_P(l)) != 0 || LOWER(UPPER_P(l)) != 0) {
		mr.val = 0;
		mr.err = true;
		return mr;
	}
	return calcMemSize64WithUint(off, LOWER(LOWER_P(l)));
}

struct MemRes calcMemSize64WithUint(struct uint256_t* off, uint64_t length64) {
	struct MemRes mr;
	if (length64 == 0) {
		mr.val = 0;
		mr.err = false;
		return mr;
	}
	uint64_t offset64 = LOWER(LOWER_P(off));
	bool overflow = false;
	if (UPPER(UPPER_P(off)) != 0 || UPPER(LOWER_P(off)) != 0 || LOWER(UPPER_P(off)) != 0) {
		overflow = true;
	}
	if (overflow) {
		mr.val = 0;
		mr.err = true;
		return mr;
	}
	mr.val = offset64 + length64;
	mr.err = mr.val < offset64;
	return mr;
}

struct MemRes memoryMLoad(struct Stack* s) {
	struct uint256_t l = { 0,0,0,32 };
	return calcMemSize64(Stack_Back(s, 0), &l);
}

struct MemRes memoryMStore(struct Stack* s) {
	return calcMemSize64WithUint(Stack_Back(s, 0), 32);
}

struct MemRes memoryMStore8(struct Stack* s) {
	return calcMemSize64WithUint(Stack_Back(s, 0), 1);
}

struct MemRes memoryReturn(struct Stack* s) {
	return calcMemSize64(Stack_Back(s, 0), Stack_Back(s, 1));
}

struct MemRes memoryRevert(struct Stack* s) {
	return calcMemSize64(Stack_Back(s, 0), Stack_Back(s, 1));
}

struct Operation* Operation_JumpTable() {
	struct Operation* JumpTable = (struct Operation*)malloc(sizeof(struct Operation) * 256);

	JumpTable[0].execute = opStop;
	JumpTable[0].minS = minStack(0, 0);
	JumpTable[0].maxS = maxStack(0, 0);
	JumpTable[0].halts = true;
	JumpTable[0].jumps = false;
	JumpTable[0].writes = false;
	JumpTable[0].reverts = false;
	JumpTable[0].returns = false;

	JumpTable[1].execute = opAdd;
	JumpTable[1].minS = minStack(2, 1);
	JumpTable[1].maxS = maxStack(2, 1);
	JumpTable[1].halts = false;
	JumpTable[1].jumps = false;
	JumpTable[1].writes = false;
	JumpTable[1].reverts = false;
	JumpTable[1].returns = false;

	JumpTable[2].execute = opMul;
	JumpTable[2].minS = minStack(2, 1);
	JumpTable[2].maxS = maxStack(2, 1);
	JumpTable[2].halts = false;
	JumpTable[2].jumps = false;
	JumpTable[2].writes = false;
	JumpTable[2].reverts = false;
	JumpTable[2].returns = false;

	JumpTable[3].execute = opSub;
	JumpTable[3].minS = minStack(2, 1);
	JumpTable[3].maxS = maxStack(2, 1);
	JumpTable[3].halts = false;
	JumpTable[3].jumps = false;
	JumpTable[3].writes = false;
	JumpTable[3].reverts = false;
	JumpTable[3].returns = false;

	JumpTable[4].execute = opDiv;
	JumpTable[4].minS = minStack(2, 1);
	JumpTable[4].maxS = maxStack(2, 1);
	JumpTable[4].halts = false;
	JumpTable[4].jumps = false;
	JumpTable[4].writes = false;
	JumpTable[4].reverts = false;
	JumpTable[4].returns = false;

	JumpTable[5].execute = opSdiv;
	JumpTable[5].minS = minStack(2, 1);
	JumpTable[5].maxS = maxStack(2, 1);
	JumpTable[5].halts = false;
	JumpTable[5].jumps = false;
	JumpTable[5].writes = false;
	JumpTable[5].reverts = false;
	JumpTable[5].returns = false;

	JumpTable[6].execute = opMod;
	JumpTable[6].minS = minStack(2, 1);
	JumpTable[6].maxS = maxStack(2, 1);
	JumpTable[6].halts = false;
	JumpTable[6].jumps = false;
	JumpTable[6].writes = false;
	JumpTable[6].reverts = false;
	JumpTable[6].returns = false;

	JumpTable[7].execute = opSmod;
	JumpTable[7].minS = minStack(2, 1);
	JumpTable[7].maxS = maxStack(2, 1);
	JumpTable[7].halts = false;
	JumpTable[7].jumps = false;
	JumpTable[7].writes = false;
	JumpTable[7].reverts = false;
	JumpTable[7].returns = false;

	JumpTable[8].execute = opAddmod;
	JumpTable[8].minS = minStack(3, 1);
	JumpTable[8].maxS = maxStack(3, 1);
	JumpTable[8].halts = false;
	JumpTable[8].jumps = false;
	JumpTable[8].writes = false;
	JumpTable[8].reverts = false;
	JumpTable[8].returns = false;

	JumpTable[9].execute = opMulmod;
	JumpTable[9].minS = minStack(3, 1);
	JumpTable[9].maxS = maxStack(3, 1);
	JumpTable[9].halts = false;
	JumpTable[9].jumps = false;
	JumpTable[9].writes = false;
	JumpTable[9].reverts = false;
	JumpTable[9].returns = false;

	JumpTable[16].execute = opLt;
	JumpTable[16].minS = minStack(2, 1);
	JumpTable[16].maxS = maxStack(2, 1);
	JumpTable[16].halts = false;
	JumpTable[16].jumps = false;
	JumpTable[16].writes = false;
	JumpTable[16].reverts = false;
	JumpTable[16].returns = false;

	JumpTable[17].execute = opGt;
	JumpTable[17].minS = minStack(2, 1);
	JumpTable[17].maxS = maxStack(2, 1);
	JumpTable[17].halts = false;
	JumpTable[17].jumps = false;
	JumpTable[17].writes = false;
	JumpTable[17].reverts = false;
	JumpTable[17].returns = false;

	JumpTable[18].execute = opSlt;
	JumpTable[18].minS = minStack(2, 1);
	JumpTable[18].maxS = maxStack(2, 1);
	JumpTable[18].halts = false;
	JumpTable[18].jumps = false;
	JumpTable[18].writes = false;
	JumpTable[18].reverts = false;
	JumpTable[18].returns = false;

	JumpTable[19].execute = opSgt;
	JumpTable[19].minS = minStack(2, 1);
	JumpTable[19].maxS = maxStack(2, 1);
	JumpTable[19].halts = false;
	JumpTable[19].jumps = false;
	JumpTable[19].writes = false;
	JumpTable[19].reverts = false;
	JumpTable[19].returns = false;

	JumpTable[20].execute = opEq;
	JumpTable[20].minS = minStack(2, 1);
	JumpTable[20].maxS = maxStack(2, 1);
	JumpTable[20].halts = false;
	JumpTable[20].jumps = false;
	JumpTable[20].writes = false;
	JumpTable[20].reverts = false;
	JumpTable[20].returns = false;

	JumpTable[21].execute = opIszero;
	JumpTable[21].minS = minStack(1, 1);
	JumpTable[21].maxS = maxStack(1, 1);
	JumpTable[21].halts = false;
	JumpTable[21].jumps = false;
	JumpTable[21].writes = false;
	JumpTable[21].reverts = false;
	JumpTable[21].returns = false;

	JumpTable[22].execute = opAnd;
	JumpTable[22].minS = minStack(2, 1);
	JumpTable[22].maxS = maxStack(2, 1);
	JumpTable[22].halts = false;
	JumpTable[22].jumps = false;
	JumpTable[22].writes = false;
	JumpTable[22].reverts = false;
	JumpTable[22].returns = false;

	JumpTable[23].execute = opXor;
	JumpTable[23].minS = minStack(2, 1);
	JumpTable[23].maxS = maxStack(2, 1);
	JumpTable[23].halts = false;
	JumpTable[23].jumps = false;
	JumpTable[23].writes = false;
	JumpTable[23].reverts = false;
	JumpTable[23].returns = false;

	JumpTable[24].execute = opOr;
	JumpTable[24].minS = minStack(2, 1);
	JumpTable[24].maxS = maxStack(2, 1);
	JumpTable[24].halts = false;
	JumpTable[24].jumps = false;
	JumpTable[24].writes = false;
	JumpTable[24].reverts = false;
	JumpTable[24].returns = false;

	JumpTable[25].execute = opNot;
	JumpTable[25].minS = minStack(1, 1);
	JumpTable[25].maxS = maxStack(1, 1);
	JumpTable[25].halts = false;
	JumpTable[25].jumps = false;
	JumpTable[25].writes = false;
	JumpTable[25].reverts = false;
	JumpTable[25].returns = false;

	JumpTable[48].execute = opAddress;
	JumpTable[48].minS = minStack(0, 1);
	JumpTable[48].maxS = maxStack(0, 1);
	JumpTable[48].halts = false;
	JumpTable[48].jumps = false;
	JumpTable[48].writes = false;
	JumpTable[48].reverts = false;
	JumpTable[48].returns = false;

	JumpTable[53].execute = opCallDataLoad;
	JumpTable[53].minS = minStack(1, 1);
	JumpTable[53].maxS = maxStack(1, 1);
	JumpTable[53].halts = false;
	JumpTable[53].jumps = false;
	JumpTable[53].writes = false;
	JumpTable[53].reverts = false;
	JumpTable[53].returns = false;
	
	JumpTable[54].execute = opCallDataSize;
	JumpTable[54].minS = minStack(0, 1);
	JumpTable[54].maxS = maxStack(0, 1);
	JumpTable[54].halts = false;
	JumpTable[54].jumps = false;
	JumpTable[54].writes = false;
	JumpTable[54].reverts = false;
	JumpTable[54].returns = false;

	JumpTable[80].execute = opPop;
	JumpTable[80].minS = minStack(1, 0);
	JumpTable[80].maxS = maxStack(1, 0);
	JumpTable[80].halts = false;
	JumpTable[80].jumps = false;
	JumpTable[80].writes = false;
	JumpTable[80].reverts = false;
	JumpTable[80].returns = false;

	JumpTable[81].execute = opMload;
	JumpTable[81].minS = minStack(1, 1);
	JumpTable[81].maxS = maxStack(1, 1);
	JumpTable[81].memorySize = memoryMLoad;
	JumpTable[81].halts = false;
	JumpTable[81].jumps = false;
	JumpTable[81].writes = false;
	JumpTable[81].reverts = false;
	JumpTable[81].returns = false;

	JumpTable[82].execute = opMstore;
	JumpTable[82].minS = minStack(2, 0);
	JumpTable[82].maxS = maxStack(2, 0);
	JumpTable[82].memorySize = memoryMStore;
	JumpTable[82].halts = false;
	JumpTable[82].jumps = false;
	JumpTable[82].writes = false;
	JumpTable[82].reverts = false;
	JumpTable[82].returns = false;

	JumpTable[83].execute = opMstore8;
	JumpTable[83].minS = minStack(2, 0);
	JumpTable[83].maxS = maxStack(2, 0);
	JumpTable[83].memorySize = memoryMStore8;
	JumpTable[83].halts = false;
	JumpTable[83].jumps = false;
	JumpTable[83].writes = false;
	JumpTable[83].reverts = false;
	JumpTable[83].returns = false;

	JumpTable[84].execute = opSload;
	JumpTable[84].minS = minStack(1, 1);
	JumpTable[84].maxS = maxStack(1, 1);
	JumpTable[84].halts = false;
	JumpTable[84].jumps = false;
	JumpTable[84].writes = false;
	JumpTable[84].reverts = false;
	JumpTable[84].returns = false;

	JumpTable[85].execute = opSstore;
	JumpTable[85].minS = minStack(2, 0);
	JumpTable[85].maxS = maxStack(2, 0);
	JumpTable[85].writes = true;
	JumpTable[85].halts = false;
	JumpTable[85].jumps = false;
	JumpTable[85].reverts = false;
	JumpTable[85].returns = false;

	JumpTable[86].execute = opJump;
	JumpTable[86].minS = minStack(1, 0);
	JumpTable[86].maxS = maxStack(1, 0);
	JumpTable[86].jumps = true;
	JumpTable[86].halts = false;
	JumpTable[86].writes = false;
	JumpTable[86].reverts = false;
	JumpTable[86].returns = false;

	JumpTable[87].execute = opJumpi;
	JumpTable[87].minS = minStack(2, 0);
	JumpTable[87].maxS = maxStack(2, 0);
	JumpTable[87].jumps = true;
	JumpTable[87].halts = false;
	JumpTable[87].writes = false;
	JumpTable[87].reverts = false;
	JumpTable[87].returns = false;

	JumpTable[88].execute = opPc;
	JumpTable[88].minS = minStack(0, 1);
	JumpTable[88].maxS = maxStack(0, 1);
	JumpTable[88].halts = false;
	JumpTable[88].jumps = false;
	JumpTable[88].writes = false;
	JumpTable[88].reverts = false;
	JumpTable[88].returns = false;

	JumpTable[89].execute = opMsize;
	JumpTable[89].minS = minStack(0, 1);
	JumpTable[89].maxS = maxStack(0, 1);
	JumpTable[89].halts = false;
	JumpTable[89].jumps = false;
	JumpTable[89].writes = false;
	JumpTable[89].reverts = false;
	JumpTable[89].returns = false;

	JumpTable[90].execute = opGas;
	JumpTable[90].minS = minStack(0, 1);
	JumpTable[90].maxS = maxStack(0, 1);
	JumpTable[90].halts = false;
	JumpTable[90].jumps = false;
	JumpTable[90].writes = false;
	JumpTable[90].reverts = false;
	JumpTable[90].returns = false;

	JumpTable[91].execute = opJumpdest;
	JumpTable[91].minS = minStack(0, 0);
	JumpTable[91].maxS = maxStack(0, 0);
	JumpTable[91].halts = false;
	JumpTable[91].jumps = false;
	JumpTable[91].writes = false;
	JumpTable[91].reverts = false;
	JumpTable[91].returns = false;

	JumpTable[96].execute = opPush1;
	JumpTable[96].minS = minStack(0, 1);
	JumpTable[96].maxS = maxStack(0, 1);
	JumpTable[96].halts = false;
	JumpTable[96].jumps = false;
	JumpTable[96].writes = false;
	JumpTable[96].reverts = false;
	JumpTable[96].returns = false;

	JumpTable[97].execute = opPush2;
	JumpTable[97].minS = minStack(0, 1);
	JumpTable[97].maxS = maxStack(0, 1);
	JumpTable[97].halts = false;
	JumpTable[97].jumps = false;
	JumpTable[97].writes = false;
	JumpTable[97].reverts = false;
	JumpTable[97].returns = false;

	JumpTable[98].execute = opPush3;
	JumpTable[98].minS = minStack(0, 1);
	JumpTable[98].maxS = maxStack(0, 1);
	JumpTable[98].halts = false;
	JumpTable[98].jumps = false;
	JumpTable[98].writes = false;
	JumpTable[98].reverts = false;
	JumpTable[98].returns = false;

	JumpTable[99].execute = opPush4;
	JumpTable[99].minS = minStack(0, 1);
	JumpTable[99].maxS = maxStack(0, 1);
	JumpTable[99].halts = false;
	JumpTable[99].jumps = false;
	JumpTable[99].writes = false;
	JumpTable[99].reverts = false;
	JumpTable[99].returns = false;

	JumpTable[124].execute = opPush29;
	JumpTable[124].minS = minStack(0, 1);
	JumpTable[124].maxS = maxStack(0, 1);
	JumpTable[124].halts = false;
	JumpTable[124].jumps = false;
	JumpTable[124].writes = false;
	JumpTable[124].reverts = false;
	JumpTable[124].returns = false;

	JumpTable[128].execute = opDup1;
	JumpTable[128].minS = minDupStack(1);
	JumpTable[128].maxS = maxDupStack(1);
	JumpTable[128].halts = false;
	JumpTable[128].jumps = false;
	JumpTable[128].writes = false;
	JumpTable[128].reverts = false;
	JumpTable[128].returns = false;

	JumpTable[144].execute = opSwap1;
	JumpTable[144].minS = minDupStack(2);
	JumpTable[144].maxS = maxDupStack(2);
	JumpTable[144].halts = false;
	JumpTable[144].jumps = false;
	JumpTable[144].writes = false;
	JumpTable[144].reverts = false;
	JumpTable[144].returns = false;

	JumpTable[243].execute = opReturn;
	JumpTable[243].minS = minStack(2, 0);
	JumpTable[243].maxS = maxStack(2, 0);
	JumpTable[243].memorySize = memoryReturn;
	JumpTable[243].halts = true;
	JumpTable[243].jumps = false;
	JumpTable[243].writes = false;
	JumpTable[243].reverts = false;
	JumpTable[243].returns = false;

	JumpTable[253].execute = opRevert;
	JumpTable[243].minS = minStack(2, 0);
	JumpTable[243].maxS = maxStack(2, 0);
	JumpTable[243].memorySize = memoryRevert;
	JumpTable[243].halts = true;
	JumpTable[243].jumps = false;
	JumpTable[243].writes = false;
	JumpTable[243].reverts = true;
	JumpTable[243].returns = true;

	return JumpTable;
}