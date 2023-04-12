#pragma once

#include "evmc.h"
#include "opcode.h"

/// The special gas cost value marking an EVM instruction as "undefined".
int16_t undefined = -1;

/// EIP-2929 constants (https://eips.ethereum.org/EIPS/eip-2929).
/// @{
const int16_t cold_sload_cost = 2100;
const int16_t cold_account_access_cost = 2600;
const int16_t warm_storage_read_cost = 100;

/// Additional cold account access cost.
///
/// The warm access cost is unconditionally applied for every account access instruction.
/// If the access turns out to be cold, this cost must be applied additionally.
const int16_t additional_cold_account_access_cost =
    cold_account_access_cost - warm_storage_read_cost;
/// @}

/// The table of instruction gas costs per EVM revision.
/// The EVM revision specific table of EVM instructions gas costs. For instructions undefined
/// in given EVM revision, the value is instr::undefined.
int16_t gas_costs[12][256];

void GasCostTable_init(int16_t gc[12][256]){
    for(int i = 0; i < 256; i++)
        gc[EVMC_FRONTIER][i] = undefined;
    gc[EVMC_FRONTIER][OP_STOP] = 0;
    gc[EVMC_FRONTIER][OP_ADD] = 3;
    gc[EVMC_FRONTIER][OP_MUL] = 5;
    gc[EVMC_FRONTIER][OP_SUB] = 3;
    gc[EVMC_FRONTIER][OP_DIV] = 5;
    gc[EVMC_FRONTIER][OP_SDIV] = 5;
    gc[EVMC_FRONTIER][OP_MOD] = 5;
    gc[EVMC_FRONTIER][OP_SMOD] = 5;
    gc[EVMC_FRONTIER][OP_ADDMOD] = 8;
    gc[EVMC_FRONTIER][OP_MULMOD] = 8;
    gc[EVMC_FRONTIER][OP_EXP] = 10;
    gc[EVMC_FRONTIER][OP_SIGNEXTEND] = 5;
    gc[EVMC_FRONTIER][OP_LT] = 3;
    gc[EVMC_FRONTIER][OP_GT] = 3;
    gc[EVMC_FRONTIER][OP_SLT] = 3;
    gc[EVMC_FRONTIER][OP_SGT] = 3;
    gc[EVMC_FRONTIER][OP_EQ] = 3;
    gc[EVMC_FRONTIER][OP_ISZERO] = 3;
    gc[EVMC_FRONTIER][OP_AND] = 3;
    gc[EVMC_FRONTIER][OP_OR] = 3;
    gc[EVMC_FRONTIER][OP_XOR] = 3;
    gc[EVMC_FRONTIER][OP_NOT] = 3;
    gc[EVMC_FRONTIER][OP_BYTE] = 3;
    gc[EVMC_FRONTIER][OP_KECCAK256] = 30;
    gc[EVMC_FRONTIER][OP_ADDRESS] = 2;
    gc[EVMC_FRONTIER][OP_BALANCE] = 20;
    gc[EVMC_FRONTIER][OP_ORIGIN] = 2;
    gc[EVMC_FRONTIER][OP_CALLER] = 2;
    gc[EVMC_FRONTIER][OP_CALLVALUE] = 2;
    gc[EVMC_FRONTIER][OP_CALLDATALOAD] = 3;
    gc[EVMC_FRONTIER][OP_CALLDATASIZE] = 2;
    gc[EVMC_FRONTIER][OP_CALLDATACOPY] = 3;
    gc[EVMC_FRONTIER][OP_CODESIZE] = 2;
    gc[EVMC_FRONTIER][OP_CODECOPY] = 3;
    gc[EVMC_FRONTIER][OP_GASPRICE] = 2;
    gc[EVMC_FRONTIER][OP_EXTCODESIZE] = 20;
    gc[EVMC_FRONTIER][OP_EXTCODECOPY] = 20;
    gc[EVMC_FRONTIER][OP_BLOCKHASH] = 20;
    gc[EVMC_FRONTIER][OP_COINBASE] = 2;
    gc[EVMC_FRONTIER][OP_TIMESTAMP] = 2;
    gc[EVMC_FRONTIER][OP_NUMBER] = 2;
    gc[EVMC_FRONTIER][OP_DIFFICULTY] = 2;
    gc[EVMC_FRONTIER][OP_GASLIMIT] = 2;
    gc[EVMC_FRONTIER][OP_POP] = 2;
    gc[EVMC_FRONTIER][OP_MLOAD] = 3;
    gc[EVMC_FRONTIER][OP_MSTORE] = 3;
    gc[EVMC_FRONTIER][OP_SLOAD] = 50;
    gc[EVMC_FRONTIER][OP_SSTORE] = 0;
    gc[EVMC_FRONTIER][OP_JUMP] = 8;
    gc[EVMC_FRONTIER][OP_JUMPI] = 10;
    gc[EVMC_FRONTIER][OP_PC] = 2;
    gc[EVMC_FRONTIER][OP_MSIZE] = 2;
    gc[EVMC_FRONTIER][OP_GAS] = 2;
    gc[EVMC_FRONTIER][OP_JUMPDEST] = 1;
    for (size_t op = (size_t)OP_PUSH1; op <= OP_PUSH32; ++op)
        gc[EVMC_FRONTIER][op] = 3;
    for (size_t op = (size_t)OP_DUP1; op <= OP_DUP16; ++op)
        gc[EVMC_FRONTIER][op] = 3;
    for (size_t op = (size_t)OP_SWAP1; op <= OP_SWAP16; ++op)
        gc[EVMC_FRONTIER][op] = 3;
    for (size_t op = (size_t)OP_LOG0; op <= OP_LOG4; ++op)
        gc[EVMC_FRONTIER][op] = (int16_t)((op - OP_LOG0 + 1) * 375);
    gc[EVMC_FRONTIER][OP_CREATE] = 32000;
    gc[EVMC_FRONTIER][OP_CALL] = 40;
    gc[EVMC_FRONTIER][OP_CALLCODE] = 40;
    gc[EVMC_FRONTIER][OP_RETURN] = 0;
    gc[EVMC_FRONTIER][OP_INVALID] = 0;
    gc[EVMC_FRONTIER][OP_SELFDESTRUCT] = 0;

    memcpy(gc[EVMC_HOMESTEAD], gc[EVMC_FRONTIER], sizeof(int16_t)*256);
    gc[EVMC_HOMESTEAD][OP_DELEGATECALL] = 40;

    memcpy(gc[EVMC_TANGERINE_WHISTLE], gc[EVMC_HOMESTEAD], sizeof(int16_t)*256);
    gc[EVMC_TANGERINE_WHISTLE][OP_BALANCE] = 400;
    gc[EVMC_TANGERINE_WHISTLE][OP_EXTCODESIZE] = 700;
    gc[EVMC_TANGERINE_WHISTLE][OP_EXTCODECOPY] = 700;
    gc[EVMC_TANGERINE_WHISTLE][OP_SLOAD] = 200;
    gc[EVMC_TANGERINE_WHISTLE][OP_CALL] = 700;
    gc[EVMC_TANGERINE_WHISTLE][OP_CALLCODE] = 700;
    gc[EVMC_TANGERINE_WHISTLE][OP_DELEGATECALL] = 700;
    gc[EVMC_TANGERINE_WHISTLE][OP_SELFDESTRUCT] = 5000;

    memcpy(gc[EVMC_SPURIOUS_DRAGON], gc[EVMC_TANGERINE_WHISTLE], sizeof(int16_t)*256);

    memcpy(gc[EVMC_BYZANTIUM], gc[EVMC_SPURIOUS_DRAGON], sizeof(int16_t)*256);
    gc[EVMC_BYZANTIUM][OP_RETURNDATASIZE] = 2;
    gc[EVMC_BYZANTIUM][OP_RETURNDATACOPY] = 3;
    gc[EVMC_BYZANTIUM][OP_STATICCALL] = 700;
    gc[EVMC_BYZANTIUM][OP_REVERT] = 0;

    memcpy(gc[EVMC_CONSTANTINOPLE], gc[EVMC_BYZANTIUM], sizeof(int16_t)*256);
    gc[EVMC_CONSTANTINOPLE][OP_SHL] = 3;
    gc[EVMC_CONSTANTINOPLE][OP_SHR] = 3;
    gc[EVMC_CONSTANTINOPLE][OP_SAR] = 3;
    gc[EVMC_CONSTANTINOPLE][OP_EXTCODEHASH] = 400;
    gc[EVMC_CONSTANTINOPLE][OP_CREATE2] = 32000;

    memcpy(gc[EVMC_PETERSBURG], gc[EVMC_CONSTANTINOPLE], sizeof(int16_t)*256);

    memcpy(gc[EVMC_ISTANBUL], gc[EVMC_PETERSBURG], sizeof(int16_t)*256);
    gc[EVMC_ISTANBUL][OP_BALANCE] = 700;
    gc[EVMC_ISTANBUL][OP_CHAINID] = 2;
    gc[EVMC_ISTANBUL][OP_EXTCODEHASH] = 700;
    gc[EVMC_ISTANBUL][OP_SELFBALANCE] = 5;
    gc[EVMC_ISTANBUL][OP_SLOAD] = 800;

    memcpy(gc[EVMC_BERLIN], gc[EVMC_ISTANBUL], sizeof(int16_t)*256);
    gc[EVMC_BERLIN][OP_EXTCODESIZE] = warm_storage_read_cost;
    gc[EVMC_BERLIN][OP_EXTCODECOPY] = warm_storage_read_cost;
    gc[EVMC_BERLIN][OP_EXTCODEHASH] = warm_storage_read_cost;
    gc[EVMC_BERLIN][OP_BALANCE] = warm_storage_read_cost;
    gc[EVMC_BERLIN][OP_CALL] = warm_storage_read_cost;
    gc[EVMC_BERLIN][OP_CALLCODE] = warm_storage_read_cost;
    gc[EVMC_BERLIN][OP_DELEGATECALL] = warm_storage_read_cost;
    gc[EVMC_BERLIN][OP_STATICCALL] = warm_storage_read_cost;
    gc[EVMC_BERLIN][OP_SLOAD] = warm_storage_read_cost;

    memcpy(gc[EVMC_LONDON], gc[EVMC_BERLIN], sizeof(int16_t)*256);
    gc[EVMC_LONDON][OP_BASEFEE] = 2;

    memcpy(gc[EVMC_PARIS], gc[EVMC_LONDON], sizeof(int16_t)*256);

    memcpy(gc[EVMC_SHANGHAI], gc[EVMC_PARIS], sizeof(int16_t)*256);
    gc[EVMC_SHANGHAI][OP_PUSH0] = 2;

    memcpy(gc[EVMC_CANCUN], gc[EVMC_SHANGHAI], sizeof(int16_t)*256);
} 

/// The EVM instruction traits.
struct Traits
{
    /// The instruction name;
    char *name;

    /// The number of stack items the instruction accesses during execution.
    int8_t stack_height_required;

    /// The stack height change caused by the instruction execution. Can be negative.
    int8_t stack_height_change;

    /// The EVM revision in which the instruction has been defined. For instructions available in
    /// every EVM revision the value is ::EVMC_FRONTIER. For undefined instructions the value is not
    /// available.
    enum evmc_revision since;
};

void Traits_init(struct Traits *t)
{
    t->name = NULL;
    t->stack_height_required = 0;
    t->stack_height_change = 0;
    t->since = EVMC_LONDON;
}

struct Traits makeTrait(char* name, int8_t sr, int8_t sc, enum evmc_revision rev)
{
    struct Traits t;
    t.name = name;
    t.stack_height_required = sr;
    t.stack_height_change = sc;
    t.since = rev;
    return t;
}

/// The global, EVM revision independent, table of traits of all known EVM instructions.
struct Traits traits[256];

void TraitsTable_init(struct Traits t[256]){
    t[OP_STOP] = makeTrait("STOP", 0, 0, EVMC_FRONTIER);
    t[OP_ADD] = makeTrait("ADD", 2, -1, EVMC_FRONTIER);
    t[OP_MUL] = makeTrait("MUL", 2, -1, EVMC_FRONTIER);
    t[OP_SUB] = makeTrait("SUB", 2, -1, EVMC_FRONTIER);
    t[OP_DIV] = makeTrait("DIV", 2, -1, EVMC_FRONTIER);
    t[OP_SDIV] = makeTrait("SDIV", 2, -1, EVMC_FRONTIER);
    t[OP_MOD] = makeTrait("MOD", 2, -1, EVMC_FRONTIER);
    t[OP_SMOD] = makeTrait("SMOD", 2, -1, EVMC_FRONTIER);
    t[OP_ADDMOD] = makeTrait("ADDMOD", 3, -2, EVMC_FRONTIER);
    t[OP_MULMOD] = makeTrait("MULMOD", 3, -2, EVMC_FRONTIER);
    t[OP_EXP] = makeTrait("EXP", 2, -1, EVMC_FRONTIER);
    t[OP_SIGNEXTEND] = makeTrait("SIGNEXTEND", 2, -1, EVMC_FRONTIER);

    t[OP_LT] = makeTrait("LT", 2, -1, EVMC_FRONTIER);
    t[OP_GT] = makeTrait("GT", 2, -1, EVMC_FRONTIER);
    t[OP_SLT] = makeTrait("SLT", 2, -1, EVMC_FRONTIER);
    t[OP_SGT] = makeTrait("SGT", 2, -1, EVMC_FRONTIER);
    t[OP_EQ] = makeTrait("EQ", 2, -1, EVMC_FRONTIER);
    t[OP_ISZERO] = makeTrait("ISZERO", 1, 0, EVMC_FRONTIER);
    t[OP_AND] = makeTrait("AND", 2, -1, EVMC_FRONTIER);
    t[OP_OR] = makeTrait("OR", 2, -1, EVMC_FRONTIER);
    t[OP_XOR] = makeTrait("XOR", 2, -1, EVMC_FRONTIER);
    t[OP_NOT] = makeTrait("NOT", 1, 0, EVMC_FRONTIER);
    t[OP_BYTE] = makeTrait("BYTE", 2, -1, EVMC_FRONTIER);
    t[OP_SHL] = makeTrait("SHL", 2, -1, EVMC_CONSTANTINOPLE);
    t[OP_SHR] = makeTrait("SHR", 2, -1, EVMC_CONSTANTINOPLE);
    t[OP_SAR] = makeTrait("SAR", 2, -1, EVMC_CONSTANTINOPLE);

    t[OP_KECCAK256] = makeTrait("KECCAK256", 2, -1, EVMC_FRONTIER);

    t[OP_ADDRESS] = makeTrait("ADDRESS", 0, 1, EVMC_FRONTIER);
    t[OP_BALANCE] = makeTrait("BALANCE", 1, 0, EVMC_FRONTIER);
    t[OP_ORIGIN] = makeTrait("ORIGIN", 0, 1, EVMC_FRONTIER);
    t[OP_CALLER] = makeTrait("CALLER", 0, 1, EVMC_FRONTIER);
    t[OP_CALLVALUE] = makeTrait("CALLVALUE", 0, 1, EVMC_FRONTIER);
    t[OP_CALLDATALOAD] = makeTrait("CALLDATALOAD", 1, 0, EVMC_FRONTIER);
    t[OP_CALLDATASIZE] = makeTrait("CALLDATASIZE", 0, 1, EVMC_FRONTIER);
    t[OP_CALLDATACOPY] = makeTrait("CALLDATACOPY", 3, -3, EVMC_FRONTIER);
    t[OP_CODESIZE] = makeTrait("CODESIZE", 0, 1, EVMC_FRONTIER);
    t[OP_CODECOPY] = makeTrait("CODECOPY", 3, -3, EVMC_FRONTIER);
    t[OP_GASPRICE] = makeTrait("GASPRICE", 0, 1, EVMC_FRONTIER);
    t[OP_EXTCODESIZE] = makeTrait("EXTCODESIZE", 1, 0, EVMC_FRONTIER);
    t[OP_EXTCODECOPY] = makeTrait("EXTCODECOPY", 4, -4, EVMC_FRONTIER);
    t[OP_RETURNDATASIZE] = makeTrait("RETURNDATASIZE", 0, 1, EVMC_BYZANTIUM);
    t[OP_RETURNDATACOPY] = makeTrait("RETURNDATACOPY", 3, -3, EVMC_BYZANTIUM);
    t[OP_EXTCODEHASH] = makeTrait("EXTCODEHASH", 1, 0, EVMC_CONSTANTINOPLE);

    t[OP_BLOCKHASH] = makeTrait("BLOCKHASH", 1, 0, EVMC_FRONTIER);
    t[OP_COINBASE] = makeTrait("COINBASE", 0, 1, EVMC_FRONTIER);
    t[OP_TIMESTAMP] = makeTrait("TIMESTAMP", 0, 1, EVMC_FRONTIER);
    t[OP_NUMBER] = makeTrait("NUMBER", 0, 1, EVMC_FRONTIER);
    t[OP_DIFFICULTY] = makeTrait("DIFFICULTY", 0, 1, EVMC_FRONTIER);
    t[OP_GASLIMIT] = makeTrait("GASLIMIT", 0, 1, EVMC_FRONTIER);
    t[OP_CHAINID] = makeTrait("CHAINID", 0, 1, EVMC_ISTANBUL);
    t[OP_SELFBALANCE] = makeTrait("SELFBALANCE", 0, 1, EVMC_ISTANBUL);
    t[OP_BASEFEE] = makeTrait("BASEFEE", 0, 1, EVMC_LONDON);

    t[OP_POP] = makeTrait("POP", 1, -1, EVMC_FRONTIER);
    t[OP_MLOAD] = makeTrait("MLOAD", 1, 0, EVMC_FRONTIER);
    t[OP_MSTORE] = makeTrait("MSTORE", 2, -2, EVMC_FRONTIER);
    t[OP_MSTORE8] = makeTrait("MSTORE8", 2, -2, EVMC_FRONTIER);
    t[OP_SLOAD] = makeTrait("SLOAD", 1, 0, EVMC_FRONTIER);
    t[OP_SSTORE] = makeTrait("SSTORE", 2, -2, EVMC_FRONTIER);
    t[OP_JUMP] = makeTrait("JUMP", 1, -1, EVMC_FRONTIER);
    t[OP_JUMPI] = makeTrait("JUMPI", 2, -2, EVMC_FRONTIER);
    t[OP_PC] = makeTrait("PC", 0, 1, EVMC_FRONTIER);
    t[OP_MSIZE] = makeTrait("MSIZE", 0, 1, EVMC_FRONTIER);
    t[OP_GAS] = makeTrait("GAS", 0, 1, EVMC_FRONTIER);
    t[OP_JUMPDEST] = makeTrait("JUMPDEST", 0, 0, EVMC_FRONTIER);

    t[OP_PUSH0] = makeTrait("PUSH0", 0, 1, EVMC_SHANGHAI);

    t[OP_PUSH1] = makeTrait("PUSH1", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH2] = makeTrait("PUSH2", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH3] = makeTrait("PUSH3", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH4] = makeTrait("PUSH4", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH5] = makeTrait("PUSH5", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH6] = makeTrait("PUSH6", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH7] = makeTrait("PUSH7", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH8] = makeTrait("PUSH8", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH9] = makeTrait("PUSH9", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH10] = makeTrait("PUSH10", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH11] = makeTrait("PUSH11", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH12] = makeTrait("PUSH12", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH13] = makeTrait("PUSH13", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH14] = makeTrait("PUSH14", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH15] = makeTrait("PUSH15", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH16] = makeTrait("PUSH16", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH17] = makeTrait("PUSH17", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH18] = makeTrait("PUSH18", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH19] = makeTrait("PUSH19", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH20] = makeTrait("PUSH20", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH21] = makeTrait("PUSH21", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH22] = makeTrait("PUSH22", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH23] = makeTrait("PUSH23", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH24] = makeTrait("PUSH24", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH25] = makeTrait("PUSH25", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH26] = makeTrait("PUSH26", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH27] = makeTrait("PUSH27", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH28] = makeTrait("PUSH28", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH29] = makeTrait("PUSH29", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH30] = makeTrait("PUSH30", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH31] = makeTrait("PUSH31", 0, 1, EVMC_FRONTIER);
    t[OP_PUSH32] = makeTrait("PUSH32", 0, 1, EVMC_FRONTIER);

    t[OP_DUP1] = makeTrait("DUP1", 1, 1, EVMC_FRONTIER);
    t[OP_DUP2] = makeTrait("DUP2", 2, 1, EVMC_FRONTIER);
    t[OP_DUP3] = makeTrait("DUP3", 3, 1, EVMC_FRONTIER);
    t[OP_DUP4] = makeTrait("DUP4", 4, 1, EVMC_FRONTIER);
    t[OP_DUP5] = makeTrait("DUP5", 5, 1, EVMC_FRONTIER);
    t[OP_DUP6] = makeTrait("DUP6", 6, 1, EVMC_FRONTIER);
    t[OP_DUP7] = makeTrait("DUP7", 7, 1, EVMC_FRONTIER);
    t[OP_DUP8] = makeTrait("DUP8", 8, 1, EVMC_FRONTIER);
    t[OP_DUP9] = makeTrait("DUP9", 9, 1, EVMC_FRONTIER);
    t[OP_DUP10] = makeTrait("DUP10", 10, 1, EVMC_FRONTIER);
    t[OP_DUP11] = makeTrait("DUP11", 11, 1, EVMC_FRONTIER);
    t[OP_DUP12] = makeTrait("DUP12", 12, 1, EVMC_FRONTIER);
    t[OP_DUP13] = makeTrait("DUP13", 13, 1, EVMC_FRONTIER);
    t[OP_DUP14] = makeTrait("DUP14", 14, 1, EVMC_FRONTIER);
    t[OP_DUP15] = makeTrait("DUP15", 15, 1, EVMC_FRONTIER);
    t[OP_DUP16] = makeTrait("DUP16", 16, 1, EVMC_FRONTIER);

    t[OP_SWAP1] = makeTrait("SWAP1", 2, 0, EVMC_FRONTIER);
    t[OP_SWAP2] = makeTrait("SWAP2", 3, 0, EVMC_FRONTIER);
    t[OP_SWAP3] = makeTrait("SWAP3", 4, 0, EVMC_FRONTIER);
    t[OP_SWAP4] = makeTrait("SWAP4", 5, 0, EVMC_FRONTIER);
    t[OP_SWAP5] = makeTrait("SWAP5", 6, 0, EVMC_FRONTIER);
    t[OP_SWAP6] = makeTrait("SWAP6", 7, 0, EVMC_FRONTIER);
    t[OP_SWAP7] = makeTrait("SWAP7", 8, 0, EVMC_FRONTIER);
    t[OP_SWAP8] = makeTrait("SWAP8", 9, 0, EVMC_FRONTIER);
    t[OP_SWAP9] = makeTrait("SWAP9", 10, 0, EVMC_FRONTIER);
    t[OP_SWAP10] = makeTrait("SWAP10", 11, 0, EVMC_FRONTIER);
    t[OP_SWAP11] = makeTrait("SWAP11", 12, 0, EVMC_FRONTIER);
    t[OP_SWAP12] = makeTrait("SWAP12", 13, 0, EVMC_FRONTIER);
    t[OP_SWAP13] = makeTrait("SWAP13", 14, 0, EVMC_FRONTIER);
    t[OP_SWAP14] = makeTrait("SWAP14", 15, 0, EVMC_FRONTIER);
    t[OP_SWAP15] = makeTrait("SWAP15", 16, 0, EVMC_FRONTIER);
    t[OP_SWAP16] = makeTrait("SWAP16", 17, 0, EVMC_FRONTIER);

    t[OP_LOG0] = makeTrait("LOG0", 2, -2, EVMC_FRONTIER);
    t[OP_LOG1] = makeTrait("LOG1", 3, -3, EVMC_FRONTIER);
    t[OP_LOG2] = makeTrait("LOG2", 4, -4, EVMC_FRONTIER);
    t[OP_LOG3] = makeTrait("LOG3", 5, -5, EVMC_FRONTIER);
    t[OP_LOG4] = makeTrait("LOG4", 6, -6, EVMC_FRONTIER);

    t[OP_CREATE] = makeTrait("CREATE", 3, -2, EVMC_FRONTIER);
    t[OP_CALL] = makeTrait("CALL", 7, -6, EVMC_FRONTIER);
    t[OP_CALLCODE] = makeTrait("CALLCODE", 7, -6, EVMC_FRONTIER);
    t[OP_RETURN] = makeTrait("RETURN", 2, -2, EVMC_FRONTIER);
    t[OP_DELEGATECALL] = makeTrait("DELEGATECALL", 6, -5, EVMC_HOMESTEAD);
    t[OP_CREATE2] = makeTrait("CREATE2", 4, -3, EVMC_CONSTANTINOPLE);
    t[OP_STATICCALL] = makeTrait("STATICCALL", 6, -5, EVMC_BYZANTIUM);
    t[OP_REVERT] = makeTrait("REVERT", 2, -2, EVMC_BYZANTIUM);
    t[OP_INVALID] = makeTrait("INVALID", 0, 0, EVMC_FRONTIER);
    t[OP_SELFDESTRUCT] = makeTrait("SELFDESTRUCT", 1, -1, EVMC_FRONTIER);
}