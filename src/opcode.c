#include "opcode.h"

// 0x0 range - arithmetic ops.
const OpCode STOP = 0x00;
const OpCode ADD = 0x01;
const OpCode MUL = 0x02;
const OpCode SUB = 0x03;
const OpCode DIV = 0x04;
const OpCode SDIV = 0x05;
const OpCode MOD = 0x06;
const OpCode SMOD = 0x07;
const OpCode ADDMOD = 0x08;
const OpCode MULMOD = 0x09;
const OpCode EXP = 0x0a;
const OpCode SIGNEXTEND = 0x0b;

// 0x10 range - comparison ops.
const OpCode LT = 0x10;
const OpCode GT = 0x11;
const OpCode SLT = 0x12;
const OpCode SGT = 0x13;
const OpCode EQ = 0x14;
const OpCode ISZERO = 0x15;
const OpCode AND = 0x16;
const OpCode OR = 0x17;
const OpCode XOR = 0x18;
const OpCode NOT = 0x19;
const OpCode BYTE = 0x1a;
const OpCode SHL = 0x1b;
const OpCode SHR = 0x1c;
const OpCode SAR = 0x1d;

const OpCode SHA3 = 0x20;

// 0x30 range - closure state.
const OpCode ADDRESS = 0x30;
const OpCode BALANCE = 0x31;
const OpCode ORIGIN = 0x32;
const OpCode CALLER = 0x33;
const OpCode CALLVALUE = 0x34;
const OpCode CALLDATALOAD = 0x35;
const OpCode CALLDATASIZE = 0x36;
const OpCode CALLDATACOPY = 0x37;
const OpCode CODESIZE = 0x38;
const OpCode CODECOPY = 0x39;
const OpCode GASPRICE = 0x3a;
const OpCode EXTCODESIZE = 0x3b;
const OpCode EXTCODECOPY = 0x3c;
const OpCode RETURNDATASIZE = 0x3d;
const OpCode RETURNDATACOPY = 0x3e;
const OpCode EXTCODEHASH = 0x3f;

// 0x40 range - block operations.
const OpCode BLOCKHASH = 0x40;
const OpCode COINBASE = 0x41;
const OpCode TIMESTAMP = 0x42;
const OpCode NUMBER = 0x43;
const OpCode DIFFICULTY = 0x44;
const OpCode GASLIMIT = 0x45;
const OpCode CHAINID = 0x46;
const OpCode SELFBALANCE = 0x47;

// 0x50 range - 'storage' and execution.
const OpCode POP = 0x50;
const OpCode MLOAD = 0x51;
const OpCode MSTORE = 0x52;
const OpCode MSTORE8 = 0x53;
const OpCode SLOAD = 0x54;
const OpCode SSTORE = 0x55;
const OpCode JUMP = 0x56;
const OpCode JUMPI = 0x57;
const OpCode PC = 0x58;
const OpCode MSIZE = 0x59;
const OpCode GAS = 0x5a;
const OpCode JUMPDEST = 0x5b;

// 0x60 range.
const OpCode PUSH1 = 0x60;
const OpCode PUSH2 = 0x61;
const OpCode PUSH3 = 0x62;
const OpCode PUSH4 = 0x63;
const OpCode PUSH5 = 0x64;
const OpCode PUSH6 = 0x65;
const OpCode PUSH7 = 0x66;
const OpCode PUSH8 = 0x67;
const OpCode PUSH9 = 0x68;
const OpCode PUSH10 = 0x69;
const OpCode PUSH11 = 0x6a;
const OpCode PUSH12 = 0x6b;
const OpCode PUSH13 = 0x6c;
const OpCode PUSH14 = 0x6d;
const OpCode PUSH15 = 0x6e;
const OpCode PUSH16 = 0x6f;
const OpCode PUSH17 = 0x70;
const OpCode PUSH18 = 0x71;
const OpCode PUSH19 = 0x72;
const OpCode PUSH20 = 0x73;
const OpCode PUSH21 = 0x74;
const OpCode PUSH22 = 0x75;
const OpCode PUSH23 = 0x76;
const OpCode PUSH24 = 0x77;
const OpCode PUSH25 = 0x78;
const OpCode PUSH26 = 0x79;
const OpCode PUSH27 = 0x7a;
const OpCode PUSH28 = 0x7b;
const OpCode PUSH29 = 0x7c;
const OpCode PUSH30 = 0x7d;
const OpCode PUSH31 = 0x7e;
const OpCode PUSH32 = 0x7f;
const OpCode DUP1 = 0x80;
const OpCode DUP2 = 0x81;
const OpCode DUP3 = 0x82;
const OpCode DUP4 = 0x83;
const OpCode DUP5 = 0x84;
const OpCode DUP6 = 0x85;
const OpCode DUP7 = 0x86;
const OpCode DUP8 = 0x87;
const OpCode DUP9 = 0x88;
const OpCode DUP10 = 0x89;
const OpCode DUP11 = 0x8a;
const OpCode DUP12 = 0x8b;
const OpCode DUP13 = 0x8c;
const OpCode DUP14 = 0x8d;
const OpCode DUP15 = 0x8e;
const OpCode DUP16 = 0x8f;
const OpCode SWAP1 = 0x90;
const OpCode SWAP2 = 0x91;
const OpCode SWAP3 = 0x92;
const OpCode SWAP4 = 0x93;
const OpCode SWAP5 = 0x94;
const OpCode SWAP6 = 0x95;
const OpCode SWAP7 = 0x96;
const OpCode SWAP8 = 0x97;
const OpCode SWAP9 = 0x98;
const OpCode SWAP10 = 0x99;
const OpCode SWAP11 = 0x9a;
const OpCode SWAP12 = 0x9b;
const OpCode SWAP13 = 0x9c;
const OpCode SWAP14 = 0x9d;
const OpCode SWAP15 = 0x9e;
const OpCode SWAP16 = 0x9f;

// 0xa0 range - logging ops.
const OpCode LOG0 = 0xa0;
const OpCode LOG1 = 0xa1;
const OpCode LOG2 = 0xa2;
const OpCode LOG3 = 0xa3;
const OpCode LOG4 = 0xa4;

// unofficial opcodes used for parsing.
const OpCode PUSH = 0xb0;
const OpCode DUP = 0xb1;
const OpCode SWAP = 0xb2;

// 0xf0 range - closures.
const OpCode CREATE = 0xf0;
const OpCode CALL = 0xf1;
const OpCode CALLCODE = 0xf2;
const OpCode RETURN = 0xf3;
const OpCode DELEGATECALL = 0xf4;
const OpCode CREATE2 = 0xf5;
const OpCode STATICCALL = 0xfa;
const OpCode REVERT = 0xfd;
const OpCode SELFDESTRUCT = 0xff;

bool OpCode_IsPush(OpCode op) {
	int iop = op - 0x00;
	if (iop >= 96 && iop <= 127) {
		return true;
	}
	else {
		return false;
	}
}

bool OpCode_IsStaticJump(OpCode op) {
	return op == JUMP;
}

char* OpCode_String(OpCode op) {
	switch (op - 0x00) {
	case 0:
		return "STOP";
	case 1:
		return "ADD";
	case 2:
		return "MUL";
	case 3:
		return "SUB";
	case 4:
		return "DIV";
	case 5:
		return "SDIV";
	case 6:
		return "MOD";
	case 7:
		return "SMOD";
	case 8:
		return "ADDMOD";
	case 9:
		return "MULMOD";
	case 10:
		return "EXP";
	case 11:
		return "SIGNEXTEND";
	case 16:
		return "LT";
	case 17:
		return "GT";
	case 18:
		return "SLT";
	case 19:
		return "SGT";
	case 20:
		return "EQ";
	case 21:
		return "ISZERO";
	case 22:
		return "AND";
	case 23:
		return "OR";
	case 24:
		return "XOR";
	case 25:
		return "NOT";
	case 26:
		return "BYTE";
	case 27:
		return "SHL";
	case 28:
		return "SHR";
	case 29:
		return "SAR";
	case 32:
		return "SHA3";
	case 48:
		return "ADDRESS";
	case 49:
		return "BALANCE";
	case 50:
		return "ORIGIN";
	case 51:
		return "CALLER";
	case 52:
		return "CALLVALUE";
	case 53:
		return "CALLDATALOAD";
	case 54:
		return "CALLDATASIZE";
	case 55:
		return "CALLDATACOPY";
	case 56:
		return "CODESIZE";
	case 57:
		return "CODECOPY";
	case 58:
		return "GASPRICE";
	case 59:
		return "EXTCODESIZE";
	case 60:
		return "EXTCODECOPY";
	case 61:
		return "RETURNDATASIZE";
	case 62:
		return "RETURNDATACOPY";
	case 63:
		return "EXTCODEHASH";
	case 64:
		return "BLOCKHASH";
	case 65:
		return "COINBASE";
	case 66:
		return "TIMESTAMP";
	case 67:
		return "NUMBER";
	case 68:
		return "DIFFICULTY";
	case 69:
		return "GASLIMIT";
	case 70:
		return "CHAINID";
	case 71:
		return "SELFBALANCE";
	case 80:
		return "POP";
	case 81:
		return "MLOAD";
	case 82:
		return "MSTORE";
	case 83:
		return "MSTORE8";
	case 84:
		return "SLOAD";
	case 85:
		return "SSTORE";
	case 86:
		return "JUMP";
	case 87:
		return "JUMPI";
	case 88:
		return "PC";
	case 89:
		return "MSIZE";
	case 90:
		return "GAS";
	case 91:
		return "JUMPDEST";
	case 96:
		return "PUSH1";
	case 97:
		return "PUSH2";
	case 98:
		return "PUSH3";
	case 99:
		return "PUSH4";
	case 100:
		return "PUSH5";
	case 101:
		return "PUSH6";
	case 102:
		return "PUSH7";
	case 103:
		return "PUSH8";
	case 104:
		return "PUSH9";
	case 105:
		return "PUSH10";
	case 106:
		return "PUSH11";
	case 107:
		return "PUSH12";
	case 108:
		return "PUSH13";
	case 109:
		return "PUSH14";
	case 110:
		return "PUSH15";
	case 111:
		return "PUSH16";
	case 112:
		return "PUSH17";
	case 113:
		return "PUSH18";
	case 114:
		return "PUSH19";
	case 115:
		return "PUSH20";
	case 116:
		return "PUSH21";
	case 117:
		return "PUSH22";
	case 118:
		return "PUSH23";
	case 119:
		return "PUSH24";
	case 120:
		return "PUSH25";
	case 121:
		return "PUSH26";
	case 122:
		return "PUSH27";
	case 123:
		return "PUSH28";
	case 124:
		return "PUSH29";
	case 125:
		return "PUSH30";
	case 126:
		return "PUSH31";
	case 127:
		return "PUSH32";
	case 128:
		return "DUP1";
	case 129:
		return "DUP2";
	case 130:
		return "DUP3";
	case 131:
		return "DUP4";
	case 132:
		return "DUP5";
	case 133:
		return "DUP6";
	case 134:
		return "DUP7";
	case 135:
		return "DUP8";
	case 136:
		return "DUP9";
	case 137:
		return "DUP10";
	case 138:
		return "DUP11";
	case 139:
		return "DUP12";
	case 140:
		return "DUP13";
	case 141:
		return "DUP14";
	case 142:
		return "DUP15";
	case 143:
		return "DUP16";
	case 144:
		return "SWAP1";
	case 145:
		return "SWAP2";
	case 146:
		return "SWAP3";
	case 147:
		return "SWAP4";
	case 148:
		return "SWAP5";
	case 149:
		return "SWAP6";
	case 150:
		return "SWAP7";
	case 151:
		return "SWAP8";
	case 152:
		return "SWAP9";
	case 153:
		return "SWAP10";
	case 154:
		return "SWAP11";
	case 155:
		return "SWAP12";
	case 156:
		return "SWAP13";
	case 157:
		return "SWAP14";
	case 158:
		return "SWAP15";
	case 159:
		return "SWAP16";
	case 160:
		return "LOG0";
	case 161:
		return "LOG1";
	case 162:
		return "LOG2";
	case 163:
		return "LOG3";
	case 164:
		return "LOG4";
	case 176:
		return "PUSH";
	case 177:
		return "DUP";
	case 178:
		return "SWAP";
	case 240:
		return "CREATE";
	case 241:
		return "CALL";
	case 242:
		return "CALLCODE";
	case 243:
		return "RETURN";
	case 244:
		return "DELEGATECALL";
	case 245:
		return "CREATE2";
	case 250:
		return "STATICCALL";
	case 253:
		return "REVERT";
	case 255:
		return "SELFDESTRUCT";
	default:
		return "Opcode not defined";
	}
}

OpCode OpCode_StringToOp(char* str) {
	if (str == "STOP") {
		return STOP;
	}
	else if (str == "ADD") {
		return ADD;
	}
	else if (str == "MUL") {
		return MUL;
	}
	else if (str == "SUB") {
		return SUB;
	}
	else if (str == "DIV") {
		return DIV;
	}
	else if (str == "SDIV") {
		return SDIV;
	}
	else if (str == "MOD") {
		return MOD;
	}
	else if (str == "SMOD") {
		return SMOD;
	}
	else if (str == "ADDMOD") {
		return ADDMOD;
	}
	else if (str == "MULMOD") {
		return MULMOD;
	}
	else if (str == "EXP") {
		return EXP;
	}
	else if (str == "SIGNEXTEND") {
		return SIGNEXTEND;
	}
	else if (str == "LT") {
		return LT;
	}
	else if (str == "GT") {
		return GT;
	}
	else if (str == "SLT") {
		return SLT;
	}
	else if (str == "SGT") {
		return SGT;
	}
	else if (str == "EQ") {
		return EQ;
	}
	else if (str == "ISZERO") {
		return ISZERO;
	}
	else if (str == "AND") {
		return AND;
	}
	else if (str == "OR") {
		return OR;
	}
	else if (str == "XOR") {
		return XOR;
	}
	else if (str == "NOT") {
		return NOT;
	}
	else if (str == "BYTE") {
		return BYTE;
	}
	else if (str == "SHL") {
		return SHL;
	}
	else if (str == "SHR") {
		return SHR;
	}
	else if (str == "SAR") {
		return SAR;
	}
	else if (str == "SHA3") {
		return SHA3;
	}
	else if (str == "ADDRESS") {
		return ADDRESS;
	}
	else if (str == "BALANCE") {
		return BALANCE;
	}
	else if (str == "ORIGIN") {
		return ORIGIN;
	}
	else if (str == "CALLER") {
		return CALLER;
	}
	else if (str == "CALLVALUE") {
		return CALLVALUE;
	}
	else if (str == "CALLDATALOAD") {
		return CALLDATALOAD;
	}
	else if (str == "CALLDATASIZE") {
		return CALLDATASIZE;
	}
	else if (str == "CALLDATACOPY") {
		return CALLDATACOPY;
	}
	else if (str == "CODESIZE") {
		return CODESIZE;
	}
	else if (str == "CODECOPY") {
		return CODECOPY;
	}
	else if (str == "GASPRICE") {
		return GASPRICE;
	}
	else if (str == "EXTCODESIZE") {
		return EXTCODESIZE;
	}
	else if (str == "EXTCODECOPY") {
		return EXTCODECOPY;
	}
	else if (str == "RETURNDATASIZE") {
		return RETURNDATASIZE;
	}
	else if (str == "RETURNDATACOPY") {
		return RETURNDATACOPY;
	}
	else if (str == "EXTCODEHASH") {
		return EXTCODEHASH;
	}
	else if (str == "BLOCKHASH") {
		return BLOCKHASH;
	}
	else if (str == "COINBASE") {
		return COINBASE;
	}
	else if (str == "TIMESTAMP") {
		return TIMESTAMP;
	}
	else if (str == "NUMBER") {
		return NUMBER;
	}
	else if (str == "DIFFICULTY") {
		return DIFFICULTY;
	}
	else if (str == "GASLIMIT") {
		return GASLIMIT;
	}
	else if (str == "CHAINID") {
		return CHAINID;
	}
	else if (str == "SELFBALANCE") {
		return SELFBALANCE;
	}
	else if (str == "POP") {
		return POP;
	}
	else if (str == "MLOAD") {
		return MLOAD;
	}
	else if (str == "MSTORE") {
		return MSTORE;
	}
	else if (str == "MSTORE8") {
		return MSTORE8;
	}
	else if (str == "SLOAD") {
		return SLOAD;
	}
	else if (str == "SSTORE") {
		return SSTORE;
	}
	else if (str == "JUMP") {
		return JUMP;
	}
	else if (str == "JUMPI") {
		return JUMPI;
	}
	else if (str == "PC") {
		return PC;
	}
	else if (str == "MSIZE") {
		return MSIZE;
	}
	else if (str == "GAS") {
		return GAS;
	}
	else if (str == "JUMPDEST") {
		return JUMPDEST;
	}
	else if (str == "PUSH1") {
		return PUSH1;
	}
	else if (str == "PUSH2") {
		return PUSH2;
	}
	else if (str == "PUSH3") {
		return PUSH3;
	}
	else if (str == "PUSH4") {
		return PUSH4;
	}
	else if (str == "PUSH5") {
		return PUSH5;
	}
	else if (str == "PUSH6") {
		return PUSH6;
	}
	else if (str == "PUSH7") {
		return PUSH7;
	}
	else if (str == "PUSH8") {
		return PUSH8;
	}
	else if (str == "PUSH9") {
		return PUSH9;
	}
	else if (str == "PUSH10") {
		return PUSH10;
	}
	else if (str == "PUSH11") {
		return PUSH11;
	}
	else if (str == "PUSH12") {
		return PUSH12;
	}
	else if (str == "PUSH13") {
		return PUSH13;
	}
	else if (str == "PUSH14") {
		return PUSH14;
	}
	else if (str == "PUSH15") {
		return PUSH15;
	}
	else if (str == "PUSH16") {
		return PUSH16;
	}
	else if (str == "PUSH17") {
		return PUSH17;
	}
	else if (str == "PUSH18") {
		return PUSH18;
	}
	else if (str == "PUSH19") {
		return PUSH19;
	}
	else if (str == "PUSH20") {
		return PUSH20;
	}
	else if (str == "PUSH21") {
		return PUSH21;
	}
	else if (str == "PUSH22") {
		return PUSH22;
	}
	else if (str == "PUSH23") {
		return PUSH23;
	}
	else if (str == "PUSH24") {
		return PUSH24;
	}
	else if (str == "PUSH25") {
		return PUSH25;
	}
	else if (str == "PUSH26") {
		return PUSH26;
	}
	else if (str == "PUSH27") {
		return PUSH27;
	}
	else if (str == "PUSH28") {
		return PUSH28;
	}
	else if (str == "PUSH29") {
		return PUSH29;
	}
	else if (str == "PUSH30") {
		return PUSH30;
	}
	else if (str == "PUSH31") {
		return PUSH31;
	}
	else if (str == "PUSH32") {
		return PUSH32;
	}
	else if (str == "DUP1") {
		return DUP1;
	}
	else if (str == "DUP2") {
		return DUP2;
	}
	else if (str == "DUP3") {
		return DUP3;
	}
	else if (str == "DUP4") {
		return DUP4;
	}
	else if (str == "DUP5") {
		return DUP5;
	}
	else if (str == "DUP6") {
		return DUP6;
	}
	else if (str == "DUP7") {
		return DUP7;
	}
	else if (str == "DUP8") {
		return DUP8;
	}
	else if (str == "DUP9") {
		return DUP9;
	}
	else if (str == "DUP10") {
		return DUP10;
	}
	else if (str == "DUP11") {
		return DUP11;
	}
	else if (str == "DUP12") {
		return DUP12;
	}
	else if (str == "DUP13") {
		return DUP13;
	}
	else if (str == "DUP14") {
		return DUP14;
	}
	else if (str == "DUP15") {
		return DUP15;
	}
	else if (str == "DUP16") {
		return DUP16;
	}
	else if (str == "SWAP1") {
		return SWAP1;
	}
	else if (str == "SWAP2") {
		return SWAP2;
	}
	else if (str == "SWAP3") {
		return SWAP3;
	}
	else if (str == "SWAP4") {
		return SWAP4;
	}
	else if (str == "SWAP5") {
		return SWAP5;
	}
	else if (str == "SWAP6") {
		return SWAP6;
	}
	else if (str == "SWAP7") {
		return SWAP7;
	}
	else if (str == "SWAP8") {
		return SWAP8;
	}
	else if (str == "SWAP9") {
		return SWAP9;
	}
	else if (str == "SWAP10") {
		return SWAP10;
	}
	else if (str == "SWAP11") {
		return SWAP11;
	}
	else if (str == "SWAP12") {
		return SWAP12;
	}
	else if (str == "SWAP13") {
		return SWAP13;
	}
	else if (str == "SWAP14") {
		return SWAP14;
	}
	else if (str == "SWAP15") {
		return SWAP15;
	}
	else if (str == "SWAP16") {
		return SWAP16;
	}
	else if (str == "LOG0") {
		return LOG0;
	}
	else if (str == "LOG1") {
		return LOG1;
	}
	else if (str == "LOG2") {
		return LOG2;
	}
	else if (str == "LOG3") {
		return LOG3;
	}
	else if (str == "LOG4") {
		return LOG4;
	}
	else if (str == "PUSH") {
		return PUSH;
	}
	else if (str == "DUP") {
		return DUP;
	}
	else if (str == "SWAP") {
		return SWAP;
	}
	else if (str == "CREATE") {
		return CREATE;
	}
	else if (str == "CALL") {
		return CALL;
	}
	else if (str == "CALLCODE") {
		return CALLCODE;
	}
	else if (str == "RETURN") {
		return RETURN;
	}
	else if (str == "DELEGATECALL") {
		return DELEGATECALL;
	}
	else if (str == "CREATE2") {
		return CREATE2;
	}
	else if (str == "STATICCALL") {
		return STATICCALL;
	}
	else if (str == "REVERT") {
		return REVERT;
	}
	else if (str == "SELFDESTRUCT") {
		return SELFDESTRUCT;
	}
	else {
		return "Opcode not defined";
	}
}
