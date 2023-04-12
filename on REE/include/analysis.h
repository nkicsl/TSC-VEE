#pragma once

#include <inttypes.h>

#include "mocked_host.h"
#include "execution_state.h"
#include "opcode.h"
#include "instructions.h"

struct Instruction;

/// Compressed information about instruction basic block.
struct BlockInfo
{
    /// The total base gas cost of all instructions in the block.
    uint32_t gas_cost;

    /// The stack height required to execute the block.
    int16_t stack_req;

    /// The maximum stack height growth relative to the stack height at block start.
    int16_t stack_max_growth;
};

// static_assert(sizeof(BlockInfo) == 8);

void BlockInfo_init(struct BlockInfo *bi)
{
    bi->gas_cost = 0;
    bi->stack_req = 0;
    bi->stack_max_growth = 0;
}

void BlockInfo_init_v(struct BlockInfo *bi, uint32_t gc, int16_t sr, int16_t smg)
{
    bi->gas_cost = gc;
    bi->stack_req = sr;
    bi->stack_max_growth = smg;
}


/// The view/controller for EVM stack.
struct Stack
{
    /// The pointer to the top item.
    /// This is never null.
    struct uint256_t *top_item;
    /// The pointer to the stack space "bottom".
    struct uint256_t *m_bottom;
};

void Stack_init(struct Stack *stack)
{
    stack->top_item = (struct uint256_t *)malloc(sizeof(struct uint256_t));
    stack->m_bottom = (struct uint256_t *)malloc(sizeof(struct uint256_t));
}

/// Empties the stack by resetting the top item pointer to the new provided stack space.
void Stack_reset(struct Stack *stack, struct uint256_t *stack_space_bottom)
{
    stack->m_bottom = stack_space_bottom;
    stack->top_item = stack->m_bottom;
}

/// Init with the provided stack space.
void Stack_init_v(struct Stack *stack, struct uint256_t *stack_space_bottom)
{
    Stack_reset(stack, stack_space_bottom);
}

/// The current number of items on the stack.
int Stack_size(struct Stack *stack)
{
    return (int)(stack->top_item - stack->m_bottom);
}

/// Returns the reference to the top item.
// NOLINTNEXTLINE(readability-make-member-function-const)
struct uint256_t *Stack_top(struct Stack *stack)
{
    return stack->top_item;
}

/// Returns the reference to the stack item on given position from the stack top.
// NOLINTNEXTLINE(readability-make-member-function-const)
struct uint256_t Stack_get(struct Stack *stack, int index)
{
    return *(stack->top_item - index);
}

/// Pushes an item on the stack. The stack limit is not checked.
void Stack_push(struct Stack *stack, struct uint256_t *item)
{
    *(++(stack->top_item)) = *item;
}

/// Returns an item popped from the top of the stack.
struct uint256_t Stack_pop(struct Stack *stack)
{
    return *stack->top_item--;
}

/// Generic execution state for generic instructions implementations.
struct AdvancedExecutionState
{
    struct ExecutionState estate;

    /// The execution state specialized for the Advanced interpreter.
    struct Stack stack;
    uint32_t current_block_cost;
};

void AdvancedExecutionState_init(struct AdvancedExecutionState* state)
{
    struct ExecutionState execution_state = state->estate;
    ExecutionState_init(&execution_state);

    /// The execution state specialized for the Advanced interpreter.
    Stack_init_v(&state->stack, StackSpace_bottom(&execution_state.stack_space));
    state->current_block_cost = 0;
}

void AdvancedExecutionState_init_v(struct AdvancedExecutionState* state, struct evmc_message* message, enum evmc_revision revision, struct evmc_host_context* host_ctx, uint8_t* code, size_t code_size)
{
    
    struct ExecutionState* execution_state = &(state->estate);
    ExecutionState_init_v(execution_state, message, revision, host_ctx, code, code_size);

    /// The execution state specialized for the Advanced interpreter.
    Stack_init_v(&state->stack, StackSpace_bottom(&execution_state->stack_space));
    state->current_block_cost = 0;
}

void AdvancedExecutionState_reset(struct AdvancedExecutionState* state, struct evmc_message* message, enum evmc_revision revision, struct evmc_host_context* host_ctx, uint8_t* code, size_t code_size)
{
    struct ExecutionState execution_state = state->estate;
    execution_state.gas_left = message->gas;
    Memory_clear(&execution_state.memory);
    execution_state.msg = message;
    
    MockedHost_init(host_ctx);
    execution_state.rev = revision;
    execution_state.return_data = NULL;
    execution_state.code.data = code;
    execution_state.code.size = code_size;

    execution_state.status = EVMC_SUCCESS;
    execution_state.output_offset = 0;
    execution_state.output_size = 0;
    execution_state.analysis.advanced = NULL;
    evmcContext_init(execution_state.m_tx);
}

bool AdvancedExecutionState_in_static_mode(struct AdvancedExecutionState* state){
    struct ExecutionState execution_state = state->estate;
    return (execution_state.msg->flags & EVMC_STATIC != 0); 
}

struct evmc_tx_context AdvancedExecutionState_get_tx_context(struct AdvancedExecutionState* state)
{
    struct ExecutionState execution_state = state->estate;
    if(execution_state.m_tx.block_timestamp == 0)
        execution_state.m_tx = get_tx_context(&execution_state.host_ctx);
    return execution_state.m_tx;
}

/// The execution state specialized for the Advanced interpreter.
/// Terminates the execution with the given status code.
struct Instruction *AdvancedExecutionState_exit(struct AdvancedExecutionState* state, enum evmc_status_code status_code)
{
    struct ExecutionState execution_state = state->estate;
    execution_state.status = status_code;
    return NULL;
}

union InstructionArgument
{
    int64_t number;
    struct uint256_t *push_value;
    uint64_t small_push_value;
    struct BlockInfo block;
};

// static_assert(sizeof(union InstructionArgument) == sizeof(uint64_t), "Incorrect size of instruction_argument");

/// The pointer to function implementing an instruction execution.
typedef struct Instruction* (*instruction_exec_fn)(struct Instruction* instr, struct AdvancedExecutionState* state);

/// The evmone intrinsic opcodes.
///
/// These intrinsic instructions may be injected to the code in the analysis phase.
/// They contain additional and required logic to be executed by the interpreter.
enum intrinsic_opcodes
{
    /// The BEGINBLOCK instruction.
    ///
    /// This instruction is defined as alias for JUMPDEST and replaces all JUMPDEST instructions.
    /// It is also injected at beginning of basic blocks not being the valid jump destination.
    /// It checks basic block execution requirements and terminates execution if they are not met.
    OPX_BEGINBLOCK = OP_JUMPDEST
};

struct OpTableEntry
{
    instruction_exec_fn fn;
    int16_t gas_cost;
    int8_t stack_req;
    int8_t stack_change;
};

typedef struct OpTable{
    struct OpTableEntry table[256];
}OpTable;

struct Instruction
{
    instruction_exec_fn fn;
    union InstructionArgument arg;
};

void Instrcution_init(struct Instruction *instr)
{
    instr->fn = NULL;
}

void Instrcution_init_v(struct Instruction *instr, instruction_exec_fn f)
{
    instr->fn = f;
}


int instr_cmp(const struct Instruction* a, const struct Instruction* b){
    return -1;
}

#define i_val struct Instruction
#define i_cmp instr_cmp
#define i_tag instr
#include "../stc/cvec.h"

int u256_cmp(const struct uint256_t* a, const struct uint256_t* b){
    struct uint256_t tmpa = *a;
    struct uint256_t tmpb = *b;
    if(equal256(&tmpa, &tmpb))
        return 0;
    else{
        if(gt256(&tmpa, &tmpb))
            return 1;
        else
            return -1;
    }
}

#define i_val struct uint256_t
#define i_cmp u256_cmp
#define i_tag u256
#include "../stc/cvec.h"

#define i_val int32_t
#define i_tag int32
#include "../stc/cvec.h"

struct CodeAnalysis
{
    cvec_instr instrs;

    cvec_u256 push_values;

    /// The offsets of JUMPDESTs in the original code.
    /// These are values that JUMP/JUMPI receives as an argument.
    /// The elements are sorted.
    cvec_int32 jumpdest_offsets;

    /// The indexes of the instructions in the generated instruction table
    /// matching the elements from jumdest_offsets.
    /// This is value to which the next instruction pointer must be set in JUMP/JUMPI.
    cvec_int32 jumpdest_targets;
};

void CodeAnalysis_init(struct CodeAnalysis *ca)
{
    ca->instrs = cvec_instr_init();
    ca->push_values = cvec_u256_init();
    ca->jumpdest_offsets = cvec_int32_init();
    ca->jumpdest_targets = cvec_int32_init();
}

int find_jumpdest(struct CodeAnalysis *analysis, int offset)
{
    cvec_int32_iter begin = cvec_int32_begin(&analysis->jumpdest_offsets);
    cvec_int32_iter end = cvec_int32_end(&analysis->jumpdest_offsets);
    cvec_int32_iter it = cvec_int32_lower_bound(&analysis->jumpdest_offsets, (int32_t)offset);
    return (it.ref != end.ref && *it.ref == offset) ? *cvec_int32_at(&analysis->jumpdest_targets, (size_t)(it.ref - begin.ref)) : -1;
}

struct CodeAnalysis analyze(enum evmc_revision rev, uint8_t* code, size_t code_size);

// OpTable get_op_table(enum evmc_revision rev); 

/// Fake wrap for generic instruction implementations accessing current code location.
/// This is to make any op<...> compile, but pointers must be replaced with Advanced-specific
/// implementation. Definition not provided.
struct Instruction* op_stop(struct Instruction* instr, struct AdvancedExecutionState* astate){
    return AdvancedExecutionState_exit(astate, stop().status);
}

struct Instruction* op_add(struct Instruction* instr, struct AdvancedExecutionState* astate){
    add(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_ADD)).stack_height_change;
    return ++instr;
}

struct Instruction* op_mul(struct Instruction* instr, struct AdvancedExecutionState* astate){
    mul(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_MUL)).stack_height_change;
    return ++instr;
}

struct Instruction* op_sub(struct Instruction* instr, struct AdvancedExecutionState* astate){
    sub(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_SUB)).stack_height_change;
    return ++instr;
}

struct Instruction* op_div(struct Instruction* instr, struct AdvancedExecutionState* astate){
    div_(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_DIV)).stack_height_change;
    return ++instr;
}

struct Instruction* op_sdiv(struct Instruction* instr, struct AdvancedExecutionState* astate){
    sdiv(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_SDIV)).stack_height_change;
    return ++instr;
}

struct Instruction* op_mod(struct Instruction* instr, struct AdvancedExecutionState* astate){
    mod(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_MOD)).stack_height_change;
    return ++instr;
}

struct Instruction* op_smod(struct Instruction* instr, struct AdvancedExecutionState* astate){
    smod(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_SMOD)).stack_height_change;
    return ++instr;
}

struct Instruction* op_addmod(struct Instruction* instr, struct AdvancedExecutionState* astate){
    addmod(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_ADDMOD)).stack_height_change;
    return ++instr;
}

struct Instruction* op_mulmod(struct Instruction* instr, struct AdvancedExecutionState* astate){
    mulmod(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_MULMOD)).stack_height_change;
    return ++instr;
}

struct Instruction* op_exp(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = exp_(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_EXP)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_signextend(struct Instruction* instr, struct AdvancedExecutionState* astate){
    signextend(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_SIGNEXTEND)).stack_height_change;
    return ++instr;
}

struct Instruction* op_lt(struct Instruction* instr, struct AdvancedExecutionState* astate){
    lt(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_LT)).stack_height_change;
    return ++instr;
}

struct Instruction* op_gt(struct Instruction* instr, struct AdvancedExecutionState* astate){
    gt(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_GT)).stack_height_change;
    return ++instr;
}

struct Instruction* op_slt(struct Instruction* instr, struct AdvancedExecutionState* astate){
    slt(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_SLT)).stack_height_change;
    return ++instr;
}

struct Instruction* op_sgt(struct Instruction* instr, struct AdvancedExecutionState* astate){
    sgt(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_SGT)).stack_height_change;
    return ++instr;
}

struct Instruction* op_eq(struct Instruction* instr, struct AdvancedExecutionState* astate){
    eq(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_EQ)).stack_height_change;
    return ++instr;
}

struct Instruction* op_iszero(struct Instruction* instr, struct AdvancedExecutionState* astate){
    iszero(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_ISZERO)).stack_height_change;
    return ++instr;
}

struct Instruction* op_and(struct Instruction* instr, struct AdvancedExecutionState* astate){
    and_(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_AND)).stack_height_change;
    return ++instr;
}

struct Instruction* op_or(struct Instruction* instr, struct AdvancedExecutionState* astate){
    or_(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_OR)).stack_height_change;
    return ++instr;
}

struct Instruction* op_xor(struct Instruction* instr, struct AdvancedExecutionState* astate){
    xor_(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_XOR)).stack_height_change;
    return ++instr;
}

struct Instruction* op_not(struct Instruction* instr, struct AdvancedExecutionState* astate){
    not_(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_NOT)).stack_height_change;
    return ++instr;
}

struct Instruction* op_byte(struct Instruction* instr, struct AdvancedExecutionState* astate){
    byte_(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_BYTE)).stack_height_change;
    return ++instr;
}

struct Instruction* op_shl(struct Instruction* instr, struct AdvancedExecutionState* astate){
    shl(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_SHL)).stack_height_change;
    return ++instr;
}

struct Instruction* op_shr(struct Instruction* instr, struct AdvancedExecutionState* astate){
    shr(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_SHR)).stack_height_change;
    return ++instr;
}

struct Instruction* op_sar(struct Instruction* instr, struct AdvancedExecutionState* astate){
    sar(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_SAR)).stack_height_change;
    return ++instr;
}

struct Instruction* op_keccak256(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = keccak256(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_KECCAK256)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_address(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    address(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_ADDRESS)).stack_height_change;
    return ++instr;
}

struct Instruction* op_balance(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = balance(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_BALANCE)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_origin(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    origin(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_ORIGIN)).stack_height_change;
    return ++instr;
}

struct Instruction* op_caller(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    caller(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_CALLER)).stack_height_change;
    return ++instr;
}

struct Instruction* op_callvalue(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    callvalue(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_CALLVALUE)).stack_height_change;
    return ++instr;
}

struct Instruction* op_calldataload(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    calldataload(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_CALLDATALOAD)).stack_height_change;
    return ++instr;
}

struct Instruction* op_calldatasize(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    calldatasize(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_CALLDATASIZE)).stack_height_change;
    return ++instr;
}

struct Instruction* op_calldatacopy(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = calldatacopy(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_CALLDATACOPY)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_codesize(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    codesize(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_CODESIZE)).stack_height_change;
    return ++instr;
}

struct Instruction* op_codecopy(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = codecopy(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_CODECOPY)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_gasprice(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    gasprice(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_GASPRICE)).stack_height_change;
    return ++instr;
}

struct Instruction* op_extcodesize(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = extcodesize(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_EXTCODESIZE)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_extcodecopy(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = extcodecopy(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_EXTCODECOPY)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_returndatasize(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    returndatasize(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_RETURNDATASIZE)).stack_height_change;
    return ++instr;
}

struct Instruction* op_returndatacopy(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = returndatacopy(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_RETURNDATACOPY)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_extcodehash(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = extcodehash(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_EXTCODEHASH)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_blockhash(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    blockhash(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_BLOCKHASH)).stack_height_change;
    return ++instr;
}

struct Instruction* op_coinbase(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    coinbase(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_COINBASE)).stack_height_change;
    return ++instr;
}

struct Instruction* op_timestamp(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    timestamp(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_TIMESTAMP)).stack_height_change;
    return ++instr;
}

struct Instruction* op_number(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    number(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_NUMBER)).stack_height_change;
    return ++instr;
}

struct Instruction* op_difficulty(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    difficulty(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DIFFICULTY)).stack_height_change;
    return ++instr;
}

struct Instruction* op_gaslimit(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    gaslimit(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_GASLIMIT)).stack_height_change;
    return ++instr;
}

struct Instruction* op_chainid(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    chainid(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_CHAINID)).stack_height_change;
    return ++instr;
}

struct Instruction* op_selfbalance(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    selfbalance(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SELFBALANCE)).stack_height_change;
    return ++instr;
}

struct Instruction* op_basefee(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    basefee(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_BASEFEE)).stack_height_change;
    return ++instr;
}

struct Instruction* op_pop(struct Instruction* instr, struct AdvancedExecutionState* astate){
    pop(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_POP)).stack_height_change;
    return ++instr;
}

struct Instruction* op_mload(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = mload(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_MLOAD)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_mstore(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = mstore(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_MSTORE)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_mstore8(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = mstore8(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_MSTORE8)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_sload(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = sload(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SLOAD)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_sstore(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    int64_t gas_left_correction = (int64_t)astate->current_block_cost - (int64_t)instr->arg.number;
    state->gas_left += gas_left_correction;

    enum evmc_status_code status = sstore(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SSTORE)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);

    if((state->gas_left -= gas_left_correction) < 0)
        return AdvancedExecutionState_exit(astate, EVMC_OUT_OF_GAS);

    return ++instr;
}

struct Instruction* op_jump(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    struct uint256_t dst = Stack_pop(&astate->stack);
    int pc = -1;
    int dst_ = (int)LOWER(LOWER(dst));
    if( dst_ > INT_MAX || (pc = find_jumpdest(state->analysis.advanced, dst_)) < 0)
        return AdvancedExecutionState_exit(astate, EVMC_BAD_JUMP_DESTINATION);
    return cvec_instr_at_mut(&state->analysis.advanced->instrs, pc);
}

struct Instruction* op_jumpi(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    struct uint256_t tmp = Stack_get(&astate->stack, 1);
    if(!zero256(&tmp))
        instr = op_jump(instr, astate);
    else{
        Stack_pop(&astate->stack);
        ++instr;
    }

    // OPT: The pc must be the BEGINBLOCK (even in fallback case),
    //      so we can execute it straight away.
    Stack_pop(&astate->stack);
    return instr;
}

struct Instruction* op_pc(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    struct uint256_t tmp = (struct uint256_t){0, 0, 0, instr->arg.number};
    Stack_push(&astate->stack, &tmp);
    return ++instr;
}

struct Instruction* op_msize(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    msize(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_MSIZE)).stack_height_change;
    return ++instr;
}

struct Instruction* op_gas(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    uint64_t correction = (uint64_t) astate->current_block_cost - (uint64_t)instr->arg.number;
    uint64_t gas = (uint64_t)state->gas_left + correction;
    struct uint256_t tmp = (struct uint256_t){0, 0, 0, gas};
    Stack_push(&astate->stack, &tmp);
    return ++instr;
}

struct Instruction* op_jumpdest(struct Instruction* instr, struct AdvancedExecutionState* astate){
    jumpdest(makeStackTop(astate->stack.top_item));
    astate->stack.top_item += (*(traits + OP_JUMPDEST)).stack_height_change;
    return ++instr;
}

struct Instruction* op_push0(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    push0(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_PUSH0)).stack_height_change;
    return ++instr;
}

struct Instruction* op_push_small(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct uint256_t tmp = (struct uint256_t){0, 0, 0, instr->arg.small_push_value};
    Stack_push(&astate->stack, &tmp);
    return ++instr;
}

struct Instruction* op_push_full(struct Instruction* instr, struct AdvancedExecutionState* astate){
    Stack_push(&astate->stack, instr->arg.push_value);
    return ++instr;
}

struct Instruction* op_dup1(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup1(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP1)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup2(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup2(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP2)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup3(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup3(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP3)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup4(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup4(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP4)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup5(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup5(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP5)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup6(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup6(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP6)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup7(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup7(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP7)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup8(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup8(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP8)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup9(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup9(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP9)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup10(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup10(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP10)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup11(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup11(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP11)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup12(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup12(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP12)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup13(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup13(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP13)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup14(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup14(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP14)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup15(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup15(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP15)).stack_height_change;
    return ++instr;
}

struct Instruction* op_dup16(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    dup16(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DUP16)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap1(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap1(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP1)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap2(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap2(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP2)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap3(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap3(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP3)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap4(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap4(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP4)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap5(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap5(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP5)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap6(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap6(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP6)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap7(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap7(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP7)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap8(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap8(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP8)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap9(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap9(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP9)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap10(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap10(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP10)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap11(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap11(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP11)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap12(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap12(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP12)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap13(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap13(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP13)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap14(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap14(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP14)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap15(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap15(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP15)).stack_height_change;
    return ++instr;
}

struct Instruction* op_swap16(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    swap16(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_SWAP16)).stack_height_change;
    return ++instr;
}

struct Instruction* op_log0(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = log0(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_LOG0)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_log1(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = log1(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_LOG1)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_log2(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = log2_(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_LOG2)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_log3(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = log3(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_LOG3)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_log4(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    enum evmc_status_code status = log4(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_LOG4)).stack_height_change;
    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    return ++instr;
}

struct Instruction* op_create(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    int64_t gas_left_correction = (int64_t)astate->current_block_cost - (int64_t)instr->arg.number;
    state->gas_left += gas_left_correction;

    enum evmc_status_code status = create(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_CREATE)).stack_height_change;

    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    
    if((state->gas_left -= gas_left_correction) < 0)
        return AdvancedExecutionState_exit(astate, EVMC_OUT_OF_GAS);

    return ++instr;
}

struct Instruction* op_call(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    int64_t gas_left_correction = (int64_t)astate->current_block_cost - (int64_t)instr->arg.number;
    state->gas_left += gas_left_correction;

    enum evmc_status_code status = call_(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_CALL)).stack_height_change;

    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    
    if((state->gas_left -= gas_left_correction) < 0)
        return AdvancedExecutionState_exit(astate, EVMC_OUT_OF_GAS);

    return ++instr;
}

struct Instruction* op_callcode(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    int64_t gas_left_correction = (int64_t)astate->current_block_cost - (int64_t)instr->arg.number;
    state->gas_left += gas_left_correction;

    enum evmc_status_code status = callcode(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_CALLCODE)).stack_height_change;

    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    
    if((state->gas_left -= gas_left_correction) < 0)
        return AdvancedExecutionState_exit(astate, EVMC_OUT_OF_GAS);

    return ++instr;
}

struct Instruction* op_return(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    return AdvancedExecutionState_exit(astate, return_(makeStackTop(astate->stack.top_item), state).status);
}

struct Instruction* op_delegatecall(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    int64_t gas_left_correction = (int64_t)astate->current_block_cost - (int64_t)instr->arg.number;
    state->gas_left += gas_left_correction;

    enum evmc_status_code status = delegatecall(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_DELEGATECALL)).stack_height_change;

    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    
    if((state->gas_left -= gas_left_correction) < 0)
        return AdvancedExecutionState_exit(astate, EVMC_OUT_OF_GAS);

    return ++instr;
}

struct Instruction* op_create2(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    int64_t gas_left_correction = (int64_t)astate->current_block_cost - (int64_t)instr->arg.number;
    state->gas_left += gas_left_correction;

    enum evmc_status_code status = create2(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_CREATE2)).stack_height_change;

    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    
    if((state->gas_left -= gas_left_correction) < 0)
        return AdvancedExecutionState_exit(astate, EVMC_OUT_OF_GAS);

    return ++instr;
}

struct Instruction* op_staticcall(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    int64_t gas_left_correction = (int64_t)astate->current_block_cost - (int64_t)instr->arg.number;
    state->gas_left += gas_left_correction;

    enum evmc_status_code status = staticcall(makeStackTop(astate->stack.top_item), state);
    astate->stack.top_item += (*(traits + OP_STATICCALL)).stack_height_change;

    if(status != EVMC_SUCCESS)
        return AdvancedExecutionState_exit(astate, status);
    
    if((state->gas_left -= gas_left_correction) < 0)
        return AdvancedExecutionState_exit(astate, EVMC_OUT_OF_GAS);

    return ++instr;
}

struct Instruction* op_revert(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    return AdvancedExecutionState_exit(astate, revert(makeStackTop(astate->stack.top_item), state).status);
}

struct Instruction* op_invalid(struct Instruction* instr, struct AdvancedExecutionState* astate){
    return AdvancedExecutionState_exit(astate, invalid().status);
}

struct Instruction* op_selfdestruct(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    return AdvancedExecutionState_exit(astate, selfdestruct_(makeStackTop(astate->stack.top_item), state).status);
}

struct Instruction* op_undefined(struct Instruction* instr, struct AdvancedExecutionState* astate){
    return AdvancedExecutionState_exit(astate, EVMC_UNDEFINED_INSTRUCTION);
}

struct Instruction* opx_beginblock(struct Instruction* instr, struct AdvancedExecutionState* astate){
    struct ExecutionState* state = &astate->estate;
    struct BlockInfo block = instr->arg.block;

    if((state->gas_left -= (int64_t)block.gas_cost) < 0)
        return AdvancedExecutionState_exit(astate, EVMC_OUT_OF_GAS);
    
    if(Stack_size(&astate->stack) < block.stack_req)
        return AdvancedExecutionState_exit(astate, EVMC_STACK_UNDERFLOW);
    
    if(Stack_size(&astate->stack) + block.stack_max_growth > 1024)
        return AdvancedExecutionState_exit(astate, EVMC_STACK_OVERFLOW);
    
    astate->current_block_cost = (uint32_t)block.gas_cost;
    return ++instr;
}

struct InstructionFnTable{
    instruction_exec_fn table[256];
};

void InstructionFnTable_init(struct InstructionFnTable* instrfns){
    instrfns->table[OP_STOP] = op_stop;
    instrfns->table[OP_ADD] = op_add;
    instrfns->table[OP_MUL] = op_mul;
    instrfns->table[OP_SUB] = op_sub;
    instrfns->table[OP_DIV] = op_div;
    instrfns->table[OP_SDIV] = op_sdiv;
    instrfns->table[OP_MOD] = op_mod;
    instrfns->table[OP_SMOD] = op_smod;
    instrfns->table[OP_ADDMOD] = op_addmod;
    instrfns->table[OP_MULMOD] = op_mulmod;
    instrfns->table[OP_EXP] = op_exp;
    instrfns->table[OP_SIGNEXTEND] = op_signextend;

    instrfns->table[OP_LT] = op_lt;
    instrfns->table[OP_GT] = op_gt;
    instrfns->table[OP_SLT] = op_slt;
    instrfns->table[OP_SGT] = op_sgt;
    instrfns->table[OP_EQ] = op_eq;
    instrfns->table[OP_ISZERO] = op_iszero;
    instrfns->table[OP_AND] = op_and;
    instrfns->table[OP_OR] = op_or;
    instrfns->table[OP_XOR] = op_xor;
    instrfns->table[OP_NOT] = op_not;
    instrfns->table[OP_BYTE] = op_byte;
    instrfns->table[OP_SHL] = op_shl;
    instrfns->table[OP_SHR] = op_shr;
    instrfns->table[OP_SAR] = op_sar;

    instrfns->table[OP_KECCAK256] = op_keccak256;

    instrfns->table[OP_ADDRESS] = op_address;
    instrfns->table[OP_BALANCE] = op_balance;
    instrfns->table[OP_ORIGIN] = op_origin;
    instrfns->table[OP_CALLER] = op_caller;
    instrfns->table[OP_CALLVALUE] = op_callvalue;
    instrfns->table[OP_CALLDATALOAD] = op_calldataload;
    instrfns->table[OP_CALLDATASIZE] = op_calldatasize;
    instrfns->table[OP_CALLDATACOPY] = op_calldatacopy;
    instrfns->table[OP_CODESIZE] = op_codesize;
    instrfns->table[OP_CODECOPY] = op_codecopy;
    instrfns->table[OP_GASPRICE] = op_gasprice;
    instrfns->table[OP_EXTCODESIZE] = op_extcodesize;
    instrfns->table[OP_EXTCODECOPY] = op_extcodecopy;
    instrfns->table[OP_RETURNDATASIZE] = op_returndatasize;
    instrfns->table[OP_RETURNDATACOPY] = op_returndatacopy;
    instrfns->table[OP_EXTCODEHASH] = op_extcodehash;

    instrfns->table[OP_BLOCKHASH] = op_blockhash;
    instrfns->table[OP_COINBASE] = op_coinbase;
    instrfns->table[OP_TIMESTAMP] = op_timestamp;
    instrfns->table[OP_NUMBER] = op_number;
    instrfns->table[OP_DIFFICULTY] = op_difficulty;
    instrfns->table[OP_GASLIMIT] = op_gaslimit;
    instrfns->table[OP_CHAINID] = op_chainid;
    instrfns->table[OP_SELFBALANCE] = op_selfbalance;
    instrfns->table[OP_BASEFEE] = op_basefee;

    instrfns->table[OP_POP] = op_pop;
    instrfns->table[OP_MLOAD] = op_mload;
    instrfns->table[OP_MSTORE] = op_mstore;
    instrfns->table[OP_MSTORE8] = op_mstore8;
    instrfns->table[OP_SLOAD] = op_sload;
    instrfns->table[OP_SSTORE] = op_sstore;
    instrfns->table[OP_JUMP] = op_jump;
    instrfns->table[OP_JUMPI] = op_jumpi;
    instrfns->table[OP_PC] = op_pc;
    instrfns->table[OP_MSIZE] = op_msize;
    instrfns->table[OP_GAS] = op_gas;
    instrfns->table[OP_JUMPDEST] = op_jumpdest;

    instrfns->table[OP_PUSH0] = op_push0;

    instrfns->table[OP_PUSH1] = op_push_small;
    instrfns->table[OP_PUSH2] = op_push_small;
    instrfns->table[OP_PUSH3] = op_push_small;
    instrfns->table[OP_PUSH4] = op_push_small;
    instrfns->table[OP_PUSH5] = op_push_small;
    instrfns->table[OP_PUSH6] = op_push_small;
    instrfns->table[OP_PUSH7] = op_push_small;
    instrfns->table[OP_PUSH8] = op_push_small;
    instrfns->table[OP_PUSH9] = op_push_full;
    instrfns->table[OP_PUSH10] = op_push_full;
    instrfns->table[OP_PUSH11] = op_push_full;
    instrfns->table[OP_PUSH12] = op_push_full;
    instrfns->table[OP_PUSH13] = op_push_full;
    instrfns->table[OP_PUSH14] = op_push_full;
    instrfns->table[OP_PUSH15] = op_push_full;
    instrfns->table[OP_PUSH16] = op_push_full;
    instrfns->table[OP_PUSH17] = op_push_full;
    instrfns->table[OP_PUSH18] = op_push_full;
    instrfns->table[OP_PUSH19] = op_push_full;
    instrfns->table[OP_PUSH20] = op_push_full;
    instrfns->table[OP_PUSH21] = op_push_full;
    instrfns->table[OP_PUSH22] = op_push_full;
    instrfns->table[OP_PUSH23] = op_push_full;
    instrfns->table[OP_PUSH24] = op_push_full;
    instrfns->table[OP_PUSH25] = op_push_full;
    instrfns->table[OP_PUSH26] = op_push_full;
    instrfns->table[OP_PUSH27] = op_push_full;
    instrfns->table[OP_PUSH28] = op_push_full;
    instrfns->table[OP_PUSH29] = op_push_full;
    instrfns->table[OP_PUSH30] = op_push_full;
    instrfns->table[OP_PUSH31] = op_push_full;
    instrfns->table[OP_PUSH32] = op_push_full;

    instrfns->table[OP_DUP1] = op_dup1;
    instrfns->table[OP_DUP2] = op_dup2;
    instrfns->table[OP_DUP3] = op_dup3;
    instrfns->table[OP_DUP4] = op_dup4;
    instrfns->table[OP_DUP5] = op_dup5;
    instrfns->table[OP_DUP6] = op_dup6;
    instrfns->table[OP_DUP7] = op_dup7;
    instrfns->table[OP_DUP8] = op_dup8;
    instrfns->table[OP_DUP9] = op_dup9;
    instrfns->table[OP_DUP10] = op_dup10;
    instrfns->table[OP_DUP11] = op_dup11;
    instrfns->table[OP_DUP12] = op_dup12;
    instrfns->table[OP_DUP13] = op_dup13;
    instrfns->table[OP_DUP14] = op_dup14;
    instrfns->table[OP_DUP15] = op_dup15;
    instrfns->table[OP_DUP16] = op_dup16;

    instrfns->table[OP_SWAP1] = op_swap1;
    instrfns->table[OP_SWAP2] = op_swap2;
    instrfns->table[OP_SWAP3] = op_swap3;
    instrfns->table[OP_SWAP4] = op_swap4;
    instrfns->table[OP_SWAP5] = op_swap5;
    instrfns->table[OP_SWAP6] = op_swap6;
    instrfns->table[OP_SWAP7] = op_swap7;
    instrfns->table[OP_SWAP8] = op_swap8;
    instrfns->table[OP_SWAP9] = op_swap9;
    instrfns->table[OP_SWAP10] = op_swap10;
    instrfns->table[OP_SWAP11] = op_swap11;
    instrfns->table[OP_SWAP12] = op_swap12;
    instrfns->table[OP_SWAP13] = op_swap13;
    instrfns->table[OP_SWAP14] = op_swap14;
    instrfns->table[OP_SWAP15] = op_swap15;
    instrfns->table[OP_SWAP16] = op_swap16;

    instrfns->table[OP_LOG0] = op_log0;
    instrfns->table[OP_LOG1] = op_log1;
    instrfns->table[OP_LOG2] = op_log2;
    instrfns->table[OP_LOG3] = op_log3;
    instrfns->table[OP_LOG4] = op_log4;

    instrfns->table[OP_CREATE] = op_create;
    instrfns->table[OP_CALL] = op_call;
    instrfns->table[OP_CALLCODE] = op_callcode;
    instrfns->table[OP_RETURN] = op_return;
    instrfns->table[OP_DELEGATECALL] = op_delegatecall;
    instrfns->table[OP_CREATE2] = op_create2;
    instrfns->table[OP_STATICCALL] = op_staticcall;
    instrfns->table[OP_REVERT] = op_revert;
    instrfns->table[OP_INVALID] = op_invalid;
    instrfns->table[OP_SELFDESTRUCT] = op_selfdestruct;

    instrfns->table[OPX_BEGINBLOCK] = opx_beginblock;
}

OpTable get_op_table(enum evmc_revision rev){
    struct InstructionFnTable instr_fns;
    InstructionFnTable_init(&instr_fns);

    OpTable opt;
    for(size_t i = 0; i < 256; ++i){
        int16_t gas_cost = *(*(gas_costs + rev) + i);
        if(gas_cost == -1){
            opt.table[i].fn = op_undefined;
            opt.table[i].gas_cost = 0;
        }else{
            opt.table[i].fn = instr_fns.table[i];
            opt.table[i].gas_cost = gas_cost;
            opt.table[i].stack_req = (*(traits + i)).stack_height_required;
            opt.table[i].stack_change = (*(traits + i)).stack_height_change;
        }
    }
    return opt;
} 