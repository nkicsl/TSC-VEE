#pragma once

#include <limits.h>

#include "instructions_traits.h"
#include "instructions_xmacro.h"
#include "sha3.h"
#include "execution_state.h"

void u256_to_u8_32(struct uint256_t* x, uint8_t* data){
    uint64_t tmp = LOWER(LOWER_P(x));
    for(int i = 31; i > 23; i--){
        data[i] = tmp & 0xFF;
        tmp = tmp >> 8;
    }
    tmp = UPPER(LOWER_P(x));
    for(int i = 23; i > 15; i--){
        data[i] = tmp & 0xFF;
        tmp = tmp >> 8;
    }
    tmp = LOWER(UPPER_P(x));
    for(int i = 15; i > 7; i--){
        data[i] = tmp & 0xFF;
        tmp = tmp >> 8;
    }
    tmp = UPPER(UPPER_P(x));
    for(int i = 7; i >= 0; i--){
        data[i] = tmp & 0xFF;
        tmp = tmp >> 8;
    }
}

void u256_to_u8_20(struct uint256_t* x, uint8_t* data){
    uint64_t tmp = LOWER(LOWER_P(x));
    for(int i = 31; i > 23; i--){
        data[i - 12] = tmp & 0xFF;
        tmp = tmp >> 8;
    }
    tmp = UPPER(LOWER_P(x));
    for(int i = 23; i > 15; i--){
        data[i - 12] = tmp & 0xFF;
        tmp = tmp >> 8;
    }
    tmp = LOWER(UPPER_P(x));
    for(int i = 15; i > 11; i--){
        data[i - 12] = tmp & 0xFF;
        tmp = tmp >> 8;
    }
}

void u256_to_u8_8(struct uint256_t* x, uint8_t* data){
    uint64_t tmp = LOWER(LOWER_P(x));
    for(int i = 31; i > 23; i--){
        data[i - 24] = tmp & 0xFF;
        tmp = tmp >> 8;
    }
}

typedef uint8_t* code_iterator;

/// Represents the pointer to the stack top item
/// and allows retrieving stack items and manipulating the pointer.
struct StackTop
{
    struct uint256_t *m_top;
};

void StackTop_init(struct StackTop* st, struct uint256_t* top)
{
    st->m_top = top;
}

struct StackTop makeStackTop(struct uint256_t* top){
    struct StackTop result;
    result.m_top = top;
    return result;
}

/// Returns the reference to the stack item by index, where 0 means the top item
/// and positive index values the items further down the stack.
/// Using [-1] is also valid, but .push() should be used instead.
struct uint256_t* StackTop_get(struct StackTop* st, int index)
{
    return st->m_top - index;
}

/// Returns the reference to the stack top item.
struct uint256_t* StackTop_top(struct StackTop *st)
{
    return st->m_top;
}

/// Returns the current top item and move the stack top pointer down.
/// The value is returned by reference because the stack slot remains valid.
struct uint256_t* StackTop_pop(struct StackTop *st)
{
    return st->m_top--;
}

// Assigns the value to the stack top and moves the stack top pointer up.
void StackTop_push(struct StackTop *st, struct uint256_t* value)
{
    *(++st->m_top) = *value;
}

/// A wrapper for evmc_status_code to indicate that an instruction
/// unconditionally terminates execution.
struct StopToken
{
    ///< The status code execution terminates with.
    enum evmc_status_code status;
};

const uint32_t max_buffer_size = UINT32_MAX;

/// The size of the EVM 256-bit word.
const int word_size = 32;

/// Returns number of words what would fit to provided number of bytes,
/// i.e. it rounds up the number bytes to number of words.
int64_t num_words(uint64_t size_in_bytes)
{
    return (int64_t)((size_in_bytes + (word_size - 1)) / word_size);
}

// Grows EVM memory and checks its cost.
//
// This function should not be inlined because this may affect other inlining decisions:
// - making check_memory() too costly to inline,
// - making mload()/mstore()/mstore8() too costly to inline.
//
// TODO: This function should be moved to Memory class.
bool grow_memory(struct ExecutionState* state, uint64_t new_size)
{
    int64_t new_words = num_words(new_size);
    int64_t current_words = (int64_t)(Memory_size(&state->memory) / word_size);
    int64_t new_cost = 3 * new_words + new_words * new_words / 512;
    int64_t current_cost = 3 * current_words + current_words * current_words / 512;
    int64_t cost = new_cost - current_cost;

    if ((state->gas_left -= cost) < 0)
        return false;

    Memory_grow(&state->memory, (size_t)(new_words * word_size));
    return true;
}

// Check memory requirements of a reasonable size.
bool check_memory_u64(struct ExecutionState* state, struct uint256_t* offset, uint64_t size)
{
    // TODO: This should be done in intx.
    // There is "branchless" variant of this using | instead of ||, but benchmarks difference
    // is within noise. This should be decided when moving the implementation to intx.
    if (((UPPER(UPPER_P(offset)) | UPPER(LOWER_P(offset)) | LOWER(UPPER_P(offset))) != 0) || (LOWER(LOWER_P(offset)) > max_buffer_size))
        return false;

    uint64_t new_size = LOWER(LOWER_P(offset)) + size;
    if (new_size > Memory_size(&state->memory))
        return grow_memory(state, new_size);

    return true;
}

// Check memory requirements for "copy" instructions.
bool check_memory_u256(struct ExecutionState *state, struct uint256_t* offset, struct uint256_t* size)
{
    // Copy of size 0 is always valid (even if offset is huge).
    if (zero256(size))
        return true;

    // This check has 3 same word checks with the check above.
    // However, compilers do decent although not perfect job unifying common instructions.
    // TODO: This should be done in intx.
    if (((UPPER(UPPER_P(offset)) | UPPER(LOWER_P(offset)) | LOWER(UPPER_P(offset))) != 0) || (LOWER(LOWER_P(offset)) > max_buffer_size))
        return false;

    return check_memory_u64(state, offset, LOWER(LOWER_P(size)));
}

/// The "core" instruction implementations.
///
/// These are minimal EVM instruction implementations which assume:
/// - the stack requirements (overflow, underflow) have already been checked,
/// - the "base" gas const has already been charged,
/// - the `stack` pointer points to the EVM stack top element.
/// Moreover, these implementations _do not_ inform about new stack height
/// after execution. The adjustment must be performed by the caller.
void pop(struct StackTop stack){
    return;
}

void jumpdest(struct StackTop stack){
    return;
}

struct StopToken stop(){
    struct StopToken st;
    st.status = EVMC_SUCCESS;
    return st;
}

struct StopToken invalid(){
    struct StopToken st;
    st.status = EVMC_INVALID_INSTRUCTION;
    return st;
}

void add(struct StackTop stack)
{   
    struct uint256_t *x = StackTop_pop(&stack);
    struct uint256_t *y = StackTop_top(&stack);
    struct uint256_t target = {0, 0, 0, 0};
    add256(x, y, &target);
    copy256(y , &target);
}

void mul(struct StackTop stack)
{
    struct uint256_t *x = StackTop_pop(&stack);
    struct uint256_t *y = StackTop_top(&stack);
    struct uint256_t target = {0, 0, 0, 0};
    mul256(x, y, &target);
    copy256(y , &target);
}

void sub(struct StackTop stack)
{
    struct uint256_t *x = StackTop_get(&stack, 0);
    struct uint256_t *y = StackTop_get(&stack, 1);
    struct uint256_t target = {0, 0, 0, 0};
    minus256(x, y, &target);
    copy256(y , &target);
}

void div_(struct StackTop stack)
{
    struct uint256_t *tmp = StackTop_get(&stack, 1);
    struct uint256_t target = {0, 0, 0, 0};
    struct uint256_t mod = {0, 0, 0, 0};
    if(!zero256(tmp)){
        divmod256(StackTop_get(&stack, 0), tmp, &target, &mod);
        copy256(tmp, &target);
    }
    else{
        copy256(tmp, &mod);
    }
}

void sdiv(struct StackTop stack)
{
    struct uint256_t *tmp = StackTop_get(&stack, 1);
    struct uint256_t target = {0, 0, 0, 0};
    struct uint256_t mod = {0, 0, 0, 0};
    if(!zero256(tmp)){
        divmod256(StackTop_get(&stack, 0), tmp, &target, &mod);
        copy256(tmp, &target);
    }
    else{
        copy256(tmp, &mod);
    }
}

void mod(struct StackTop stack)
{
    struct uint256_t *tmp = StackTop_get(&stack, 1);
    struct uint256_t target = {0, 0, 0, 0};
    struct uint256_t mod = {0, 0, 0, 0};
    if(!zero256(tmp))
        divmod256(StackTop_get(&stack, 0), tmp, &target, &mod);
    copy256(tmp, &mod);
}

void smod(struct StackTop stack)
{
    struct uint256_t *tmp = StackTop_get(&stack, 1);
    struct uint256_t target = {0, 0, 0, 0};
    struct uint256_t mod = {0, 0, 0, 0};
    if(!zero256(tmp))
        divmod256(StackTop_get(&stack, 0), tmp, &target, &mod);
    copy256(tmp, &mod);
}

void addmod(struct StackTop stack)
{
    struct uint256_t *x = StackTop_pop(&stack);
    struct uint256_t *y = StackTop_pop(&stack);
    struct uint256_t *m = StackTop_top(&stack);
    struct uint256_t mod = {0, 0, 0, 0};
    
    if(!zero256(m)){
        struct uint256_t target1 = {0, 0, 0, 0};
        struct uint256_t target2 = {0, 0, 0, 0};
        add256(x, y, &target1);
        divmod256(&target1, m, &target2, &mod);
        copy256(m, &target2);
    }
    else
        copy256(m, &mod);
}

void mulmod(struct StackTop stack)
{
    struct uint256_t *x = StackTop_get(&stack, 0);
    struct uint256_t *y = StackTop_get(&stack, 1);
    struct uint256_t *m = StackTop_get(&stack, 2);
    struct uint256_t mod = {0, 0, 0, 0};

    if(!zero256(m)){
        struct uint256_t target1 = {0, 0, 0, 0};
        struct uint256_t target2 = {0, 0, 0, 0};
        mul256(x, y, &target1);
        divmod256(&target1, m, &target2, &mod);
        copy256(m, &target2);
    }
    else
        copy256(m, &mod);
}

unsigned clz(uint64_t x){
    unsigned n = 64;
    for(int i = 5; i >= 0; --i){
        unsigned s = (unsigned)1 << i;
        uint64_t hi = x >> s;
        if(hi != 0){
            n -= s;
            x = hi;
        }
    }
    return n - (unsigned)x;
}

unsigned count_significant_bytes(uint64_t x){
    return (64 - clz(x) + 7) / 8;
}

unsigned csb256(struct uint256_t x){
    unsigned r64 = count_significant_bytes(64);
    unsigned tmp = UPPER(UPPER(x));
    if(tmp != 0){
        return count_significant_bytes(tmp) + r64 * 3;
    }
    tmp = LOWER(UPPER(x));
    if(tmp != 0){
        return count_significant_bytes(tmp) + r64 * 2;
    }
    tmp = UPPER(LOWER(x));
    if(tmp != 0){
        return count_significant_bytes(tmp) + r64;
    }
    tmp = UPPER(LOWER(x));
        return count_significant_bytes(tmp);
}

enum evmc_status_code exp_(struct StackTop stack, struct ExecutionState* state)
{
    struct uint256_t *base = StackTop_pop(&stack);
    struct uint256_t *exponent = StackTop_top(&stack);

    int exponent_significant_bytes = csb256(*exponent);
    int exponent_cost = state->rev >= EVMC_SPURIOUS_DRAGON ? 50 : 10;
    int additional_cost = exponent_significant_bytes * exponent_cost;
    if ((state->gas_left -= additional_cost) < 0)
        return EVMC_OUT_OF_GAS;
    
    struct uint256_t times = *exponent;
    struct uint256_t step = {0, 0, 0, 1};
    struct uint256_t target = *base;

    if(zero256(exponent)){
        *exponent = step;
    }
    else{
        while (!zero256(exponent))
        {
            struct uint256_t tmp = (struct uint256_t){0, 0, 0, 0};
            mul256(&target, base, &tmp);
            copy256(&target, &tmp);

            tmp = (struct uint256_t){0, 0, 0, 0};
            minus256(exponent, &step, &tmp);
            copy256(exponent, &tmp);
        }
    }
    return EVMC_SUCCESS;
}

uint64_t* u256_index(struct uint256_t* x, size_t index){
    uint64_t* res;
    switch(index){
        case 0:
            res = &LOWER(LOWER_P(x));
            break;
        case 1:
            res = &UPPER(LOWER_P(x));
            break;
        case 2:
            res = &LOWER(UPPER_P(x));
            break;
        case 3:
            res = &UPPER(UPPER_P(x));
            break;
    }
    return res;
}

void signextend(struct StackTop stack)
{
    struct uint256_t* ext = StackTop_pop(&stack);
    struct uint256_t* x = StackTop_top(&stack);

    // For 31 we also don't need to do anything.
    if(UPPER(UPPER_P(ext)) == 0 && LOWER(UPPER_P(ext)) == 0 && UPPER(LOWER_P(ext)) == 0 && LOWER(LOWER_P(ext)) < (uint64_t)31){
        // uint256 -> uint64
        uint64_t e = LOWER(LOWER_P(ext));
        // Index of the word with the sign bit
        size_t sign_word_index = (size_t)(e / sizeof(e));
        // Index of the sign byte in the sign word
        size_t sign_byte_index = (size_t)(e % sizeof(e));
        uint64_t sign_word = *u256_index(x, sign_word_index);

        size_t sign_byte_offset = sign_byte_index * 8;
        // Move sign byte to position 0
        int8_t sign_byte = sign_word >> sign_byte_offset;

        // Sign-extend the "sign" byte and move it to the right position. Value bits are zeros
        uint64_t sext_byte = (uint64_t)(int64_t)sign_byte;
        uint64_t sext = sext_byte << sign_byte_offset;

        uint64_t sign_mask = ~(uint64_t)0 << sign_byte_offset;
        // Reset extended bytes
        uint64_t value = sign_word & ~sign_mask;
        // Combine the result word
        sign_word = sext | value;

        // Produce bits (all zeros or ones) for extended words. This is done by SAR of
        // the sign-extended byte. Shift by any value 7-63 would work.
        uint64_t sign_ex = (uint64_t)(int64_t)(sext_byte >> 8);

        for (size_t i = 3; i > sign_word_index; --i)
            // Clear extended words.
            *u256_index(x, sign_word_index) = sign_ex;
    }
}

void lt(struct StackTop stack)
{
    struct uint256_t *x = StackTop_pop(&stack);
    struct uint256_t *y = StackTop_top(&stack);
    if (gt256(y, x))
        *y = (struct uint256_t){0, 0, 0, 1};
    else
        *y = (struct uint256_t){0, 0, 0, 0};
}

void gt(struct StackTop stack)
{
    struct uint256_t *x = StackTop_pop(&stack);
    struct uint256_t *y = StackTop_top(&stack);
    if (gt256(x, y))
        *y = (struct uint256_t){0, 0, 0, 1};
    else
        *y = (struct uint256_t){0, 0, 0, 0};
}

void slt(struct StackTop stack)
{
    struct uint256_t *x = StackTop_pop(&stack);
    struct uint256_t *y = StackTop_top(&stack);
    if (gt256(y, x))
        *y = (struct uint256_t){0, 0, 0, 1};
    else
        *y = (struct uint256_t){0, 0, 0, 0};
}

void sgt(struct StackTop stack)
{
    struct uint256_t *x = StackTop_pop(&stack);
    struct uint256_t *y = StackTop_top(&stack);
    if (gt256(x, y))
        *y = (struct uint256_t){0, 0, 0, 1};
    else
        *y = (struct uint256_t){0, 0, 0, 0};
}

void eq(struct StackTop stack)
{
    struct uint256_t *x = StackTop_get(&stack, 0);
    struct uint256_t *y = StackTop_get(&stack, 1);
    if (equal256(x, y))
        *y = (struct uint256_t){0, 0, 0, 1};
    else
        *y = (struct uint256_t){0, 0, 0, 0};
}

void iszero(struct StackTop stack)
{
    struct uint256_t *x = StackTop_top(&stack);
    if(zero256(x))
        *x = (struct uint256_t){0, 0, 0, 1};
    else
        *x = (struct uint256_t){0, 0, 0, 0};
}

void and_(struct StackTop stack)
{
    struct uint256_t *tmp = StackTop_pop(&stack);
    struct uint256_t *curr_top = StackTop_top(&stack);
    struct uint256_t target = {0, 0, 0, 0};
    and256(tmp, curr_top, &target);
    copy256(curr_top, &target);
}

void or_(struct StackTop stack)
{
    struct uint256_t *tmp = StackTop_pop(&stack);
    struct uint256_t *curr_top = StackTop_top(&stack);
    struct uint256_t target = {0, 0, 0, 0};
    or256(tmp, curr_top, &target);
    copy256(curr_top, &target);
}

void xor_(struct StackTop stack)
{
    struct uint256_t *tmp = StackTop_pop(&stack);
    struct uint256_t *curr_top = StackTop_top(&stack);
    struct uint256_t target = {0, 0, 0, 0};
    xor256(tmp, curr_top, &target);
    copy256(curr_top, &target);
}

void not_(struct StackTop stack)
{
    struct uint256_t *curr_top = StackTop_top(&stack);
    struct uint256_t target = {0, 0, 0, 0};
    not256(curr_top, &target);
    copy256(curr_top, &target);
}

void byte_(struct StackTop stack)
{
    struct uint256_t *n = StackTop_pop(&stack);
    struct uint256_t *x = StackTop_top(&stack);

    bool n_valid = (UPPER(UPPER_P(n)) == 0 && LOWER(UPPER_P(n)) == 0 && UPPER(LOWER_P(n)) == 0 && LOWER(LOWER_P(n)) < 32);
    uint64_t byte_mask = (n_valid ? 0xff : 0);

    unsigned index = 31 - (unsigned)(LOWER(LOWER_P(n)) % 32);
    uint64_t word = *u256_index(x, index / 8);
    unsigned byte_index = index % 8;
    uint64_t byte = (word >> (byte_index * 8)) & byte_mask;
    *x = (struct uint256_t){0, 0, 0, byte};
}

void shl(struct StackTop stack)
{
    struct uint256_t *x = StackTop_pop(&stack);
    struct uint256_t *y = StackTop_top(&stack);

    uint32_t offset = (uint32_t)LOWER(LOWER_P(x));
    struct uint256_t target = {0, 0, 0, 0};

    shiftl256(y, offset, &target);
    copy256(y, &target);
}

void shr(struct StackTop stack)
{
    struct uint256_t *x = StackTop_pop(&stack);
    struct uint256_t *y = StackTop_top(&stack);

    uint32_t offset = (uint32_t)LOWER(LOWER_P(x));
    struct uint256_t target = {0, 0, 0, 0};
    
    shiftr256(y, offset, &target);
    copy256(y, &target);
}

void sar(struct StackTop stack)
{
    struct uint256_t *y = StackTop_pop(&stack);
    struct uint256_t *x = StackTop_top(&stack);

    // Inspect the top bit (words are LE).
    struct uint256_t sign_mask = {0,0,0,0};
    if((int64_t)*u256_index(x, 3) < 0){
        struct uint256_t target = {0, 0, 0, 0};
        not256(&sign_mask, &target);
        copy256(&sign_mask, &target);
    }

    uint8_t mask_shift = (UPPER(UPPER_P(y)) == 0 && LOWER(UPPER_P(y)) == 0 && UPPER(LOWER_P(y)) == 0 && LOWER(LOWER_P(y)) < 256) ? (256 - LOWER(LOWER_P(y))) : 0;

    uint32_t offset = (uint32_t)LOWER(LOWER_P(y));
    struct uint256_t target1 = {0, 0, 0, 0};
    shiftr256(x, offset, &target1);

    struct uint256_t target2 = {0, 0, 0, 0};
    shiftl256(&sign_mask, offset, &target2);

    struct uint256_t target = {0, 0, 0, 0};
    add256(&target1, &target2, &target);
    copy256(x, &target);
}

enum evmc_status_code keccak256(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *index = StackTop_pop(&stack);
    struct uint256_t *size = StackTop_top(&stack);

    if (!check_memory_u256(state, index, size))
        return EVMC_OUT_OF_GAS;

    size_t i = (size_t)LOWER(LOWER_P(index));
    size_t s = (size_t)LOWER(LOWER_P(size));
    int64_t w = num_words(s);
    int64_t cost = w * 6;
    if ((state->gas_left -= cost) < 0)
        return EVMC_OUT_OF_GAS;
    
    uint8_t* data = s != 0 ? Memory_get(&state->memory, i) : NULL;

    sha3_context c;
    const uint8_t *hash;
    sha3_Init256(&c);
    sha3_Update(&c, data, w);
    hash = sha3_Finalize(&c);
    uint8_t *hash_value = (uint8_t*)malloc(32);
    memcpy(hash_value, hash, 32);
    readu256BE(hash_value, size);
    return EVMC_SUCCESS;
}

void address(struct StackTop stack, struct ExecutionState *state)
{
    uint8_t *bytes;
    memcpy(bytes, state->msg->recipient.bytes, 20);
    uint8_t addr[32];
    for (int i = 20; i < 32; i++){
        addr[i] = 0;
    }
    for (int i = 19; i >= 0; i--)
    {
        addr[12 + i] = *(bytes + i);
    }
    struct uint256_t target = {0, 0, 0, 0};
    readu256BE(addr, &target);
    StackTop_push(&stack, &target);
}

enum evmc_status_code balance(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *x = StackTop_top(&stack); 
    evmc_address addr;
    u256_to_u8_20(x, addr.bytes);

    if (state->rev >= EVMC_BERLIN && access_account(&state->host_ctx, addr) == EVMC_ACCESS_COLD)
    {
        if ((state->gas_left -= additional_cold_account_access_cost) < 0)
            return EVMC_OUT_OF_GAS;
    }

    evmc_uint256be res = get_balance(&state->host_ctx, addr);
    readu256BE(res.bytes, x);
    return EVMC_SUCCESS;
}

void origin(struct StackTop stack, struct ExecutionState *state)
{
    evmc_address addr = get_tx_context(&state->host_ctx).tx_origin;
    uint8_t res[32];
    for (int i = 20; i < 32; i++){
        res[i] = 0;
    }
    for (int i = 19; i >= 0; i--)
    {
        res[12 + i] = *(addr.bytes + i);
    }
    struct uint256_t target = {0, 0, 0, 0};
    readu256BE(res, &target);
    StackTop_push(&stack, &target);
}

void caller(struct StackTop stack, struct ExecutionState *state)
{
    uint8_t *bytes = (uint8_t*)malloc(20);
    memcpy(bytes, state->msg->sender.bytes, 20);
    uint8_t addr[32];
    memset(addr, 0, 32);
    for (int i = 19; i >= 0; i--)
    {
        addr[12 + i] = *(bytes + i);
    }
    struct uint256_t target = {0, 0, 0, 0};
    readu256BE(addr, &target);
    StackTop_push(&stack, &target);
}

void callvalue(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t target = {0, 0, 0, 0};

    uint8_t tmp[32];
    memcpy(tmp, state->msg->value.bytes, 32);
    readu256BE(tmp, &target);
    StackTop_push(&stack, &target);
}

void calldataload(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *index = StackTop_top(&stack);

    struct uint256_t input_size = {0, 0, 0, (uint64_t)state->msg->input_size};

    if (gt256(index, &input_size))
    {
        *index = (struct uint256_t){0, 0, 0, 0};
    }
    else
    {
        size_t begin = LOWER(LOWER_P(index));
        size_t end = MIN(begin + 32, state->msg->input_size);

        uint8_t data[32];
        for (size_t i = 0; i < (end - begin); ++i)
            data[i] = state->msg->input_data[begin + i];
        for (size_t i = (end - begin); i < 32; ++i)
            data[i] = 0;
        readu256BE(data, index);
    }
}

void calldatasize(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t input_size = {0, 0, 0, (uint64_t)state->msg->input_size};
    StackTop_push(&stack, &input_size);
}

enum evmc_status_code calldatacopy(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *mem_index = StackTop_pop(&stack);
    struct uint256_t *input_index = StackTop_pop(&stack);
    struct uint256_t *size = StackTop_pop(&stack);

    if (!check_memory_u256(state, mem_index, size))
        return EVMC_OUT_OF_GAS;

    size_t dst = (size_t)LOWER(LOWER_P(mem_index));
    size_t src = MIN(state->msg->input_size, (size_t)LOWER(LOWER_P(input_index)));
    size_t s = (size_t)LOWER(LOWER_P(size));
    size_t copy_size = MIN(s, state->msg->input_size - src);

    int64_t copy_cost = num_words(s) * 3;
    if ((state->gas_left -= copy_cost) < 0)
        return EVMC_OUT_OF_GAS;

    if (copy_size > 0)
        memcpy(Memory_get(&state->memory, dst), &state->msg->input_data[src], copy_size);

    if (s - copy_size > 0)
        memset(Memory_get(&state->memory, dst + copy_size), 0, s - copy_size);

    return EVMC_SUCCESS;
}

void codesize(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = {0, 0, 0, (uint64_t)state->code.size};
    StackTop_push(&stack, &tmp);
}

enum evmc_status_code codecopy(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *mem_index = StackTop_pop(&stack);
    struct uint256_t *input_index = StackTop_pop(&stack);
    struct uint256_t *size = StackTop_pop(&stack);

    if (!check_memory_u64(state, mem_index, LOWER(LOWER_P(size))))
        return EVMC_OUT_OF_GAS;

    size_t code_size = state->code.size;
    size_t dst = (size_t)LOWER(LOWER_P(mem_index));
    size_t src = MIN(code_size, (size_t)LOWER(LOWER_P(input_index)));
    size_t s = (size_t)LOWER(LOWER_P(size));
    size_t copy_size = MIN(s, code_size - src);

    int64_t copy_cost = num_words(s) * 3;

    if ((state->gas_left -= copy_cost) < 0)
        return EVMC_OUT_OF_GAS;

    if (copy_size > 0)
        memcpy(Memory_get(&state->memory, dst), &state->code.data[src], copy_size);
    
    if (s - copy_size > 0)
        memset(Memory_get(&state->memory, dst + copy_size), 0, s - copy_size);
    return EVMC_SUCCESS;
}

void gasprice(struct StackTop stack, struct ExecutionState *state)
{
    evmc_uint256be tx_gp = get_tx_context(&state->host_ctx).tx_gas_price;
    struct uint256_t target = {0, 0, 0, 0};
    readu256BE(tx_gp.bytes, &target);
    StackTop_push(&stack, &target);
}

enum evmc_status_code extcodesize(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *x = StackTop_top(&stack);
    evmc_address addr;
    u256_to_u8_20(x, addr.bytes);

    if (state->rev >= EVMC_BERLIN && access_account(&state->host_ctx, addr) == EVMC_ACCESS_COLD)
    {
        if ((state->gas_left -= additional_cold_account_access_cost) < 0)
            return EVMC_OUT_OF_GAS;
    }

    size_t res = get_code_size(&state->host_ctx, addr);
    struct uint256_t tmp = {0, 0, 0, (uint64_t)res};
    copy256(x, &tmp);
    return EVMC_SUCCESS;
}

enum evmc_status_code extcodecopy(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *tmp = StackTop_pop(&stack);
    evmc_address addr;
    u256_to_u8_20(tmp, addr.bytes);

    struct uint256_t *mem_index = StackTop_pop(&stack);
    struct uint256_t *input_index = StackTop_pop(&stack);
    struct uint256_t *size = StackTop_pop(&stack);

    if (!check_memory_u256(state, mem_index, size))
        return EVMC_OUT_OF_GAS;

    size_t s = (size_t)LOWER(LOWER_P(size));
    int64_t copy_cost = num_words(s) * 3;
    if ((state->gas_left -= copy_cost) < 0)
        return EVMC_OUT_OF_GAS;

    if (state->rev >= EVMC_BERLIN && access_account(&state->host_ctx, addr) == EVMC_ACCESS_COLD)
    {
        if ((state->gas_left -= additional_cold_account_access_cost) < 0)
            return EVMC_OUT_OF_GAS;
    }

    if (s > 0)
    {
        size_t src = MIN((size_t)UINT32_MAX, (size_t)LOWER(LOWER_P(input_index)));
        size_t dst = (size_t)LOWER(LOWER_P(mem_index));
        size_t num_bytes_copied = copy_code(&state->host_ctx, addr, src, Memory_get(&state->memory, dst), s);
        size_t num_bytes_to_clear = s - num_bytes_copied;
        if (num_bytes_to_clear > 0)
            memset(Memory_get(&state->memory, dst + num_bytes_copied), 0, num_bytes_to_clear);
    }

    return EVMC_SUCCESS;
}

void returndatasize(struct StackTop stack, struct ExecutionState *state)
{
    StackTop_push(&stack, &(struct uint256_t){0, 0, 0, (uint64_t)strlen(state->return_data)});
}

enum evmc_status_code returndatacopy(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *mem_index = StackTop_pop(&stack);
    struct uint256_t *input_index = StackTop_pop(&stack);
    struct uint256_t *size = StackTop_pop(&stack);

    if (!check_memory_u256(state, mem_index, size))
        return EVMC_OUT_OF_GAS;

    size_t dst = (size_t)LOWER(LOWER_P(mem_index));
    size_t s = (size_t)LOWER(LOWER_P(size));

    if (strlen(state->return_data) < LOWER(LOWER_P(input_index)))
        return EVMC_INVALID_MEMORY_ACCESS;
    size_t src = (size_t)LOWER(LOWER_P(input_index));

    if (src + s > strlen(state->return_data))
        return EVMC_INVALID_MEMORY_ACCESS;

    int64_t copy_cost = num_words(s) * 3;
    if ((state->gas_left -= copy_cost) < 0)
        return EVMC_OUT_OF_GAS;

    if (s > 0)
        memcpy(Memory_get(&state->memory, dst), &state->return_data[src], s);
    return EVMC_SUCCESS;
}

enum evmc_status_code extcodehash(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *x = StackTop_top(&stack);
    evmc_address addr;
    u256_to_u8_20(x, addr.bytes);

    if (state->rev >= EVMC_BERLIN && access_account(&state->host_ctx, addr) == EVMC_ACCESS_COLD)
    {
        if ((state->gas_left -= additional_cold_account_access_cost) < 0)
            return EVMC_OUT_OF_GAS;
    }

    evmc_bytes32 hash = get_code_hash(&state->host_ctx, addr);
    readu256BE(hash.bytes, x);
    return EVMC_SUCCESS;
}

void blockhash(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *number = StackTop_top(&stack);

    int64_t upper_bound = get_tx_context(&state->host_ctx).block_number;
    int64_t lower_bound = (upper_bound - 256) > 0 ? (upper_bound - 256) : 0;
    int64_t n = (int64_t)LOWER(LOWER_P(number));
    evmc_bytes32 tmp;
    memset(tmp.bytes, 0, 32);
    evmc_bytes32 header = (gt256(number, &(struct uint256_t){0, 0, 0, upper_bound}) && n >= lower_bound) ? get_block_hash(&state->host_ctx, n) : tmp;
    readu256BE(header.bytes, number);
}

void coinbase(struct StackTop stack, struct ExecutionState *state)
{
    evmc_address addr = get_tx_context(&state->host_ctx).block_coinbase;
    uint8_t res[32];
    for (int i = 20; i < 32; i++){
        res[i] = 0;
    }
    for (int i = 19; i >= 0; i--)
    {
        res[12 + i] = *(addr.bytes + i);
    }
    struct uint256_t target = {0, 0, 0, 0};
    readu256BE(res, &target);
    StackTop_push(&stack, &target);

}

void timestamp(struct StackTop stack, struct ExecutionState *state)
{
    int64_t ts = get_tx_context(&state->host_ctx).block_timestamp;
    struct uint256_t tmp = {0, 0, 0, (uint64_t)ts};
    StackTop_push(&stack, &tmp);
}

void number(struct StackTop stack, struct ExecutionState *state)
{
    int64_t bn = get_tx_context(&state->host_ctx).block_number;
    struct uint256_t tmp = {0, 0, 0, (uint64_t)bn};
    StackTop_push(&stack, &tmp);
}

// Fix block_difficult
void difficulty(struct StackTop stack, struct ExecutionState *state)
{
    // struct uint256_t bd = get_tx_context(&state->host_ctx).block_difficult;
    // StackTop_push(&stack, bd);
}

void gaslimit(struct StackTop stack, struct ExecutionState *state)
{
    int64_t bgl = get_tx_context(&state->host_ctx).block_gas_limit;
    struct uint256_t tmp = {0, 0, 0, (uint64_t)bgl};
    StackTop_push(&stack, &tmp);
}

void chainid(struct StackTop stack, struct ExecutionState *state)
{
    evmc_uint256be cid = get_tx_context(&state->host_ctx).chain_id;
    struct uint256_t target = {0, 0, 0, 0};
    readu256BE(cid.bytes, &target);
    StackTop_push(&stack, &target);
}

void selfbalance(struct StackTop stack, struct ExecutionState *state)
{
    // TODO: introduce selfbalance in EVMC?
    evmc_uint256be balance = get_balance(&state->host_ctx, state->msg->recipient);
    struct uint256_t target = {0, 0, 0, 0};
    readu256BE(balance.bytes, &target);
    StackTop_push(&stack, &target);
}

void basefee(struct StackTop stack, struct ExecutionState *state)
{
    evmc_uint256be bbf = get_tx_context(&state->host_ctx).block_base_fee;
    struct uint256_t target = {0, 0, 0, 0};
    readu256BE(bbf.bytes, &target);
    StackTop_push(&stack, &target);
}

enum evmc_status_code mload(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *index = StackTop_top(&stack);

    if (!check_memory_u64(state, index, 32))
        return EVMC_OUT_OF_GAS;

    uint8_t *data = Memory_get(&state->memory, (size_t)LOWER(LOWER_P(index)));
    readu256BE(data, index);

    return EVMC_SUCCESS;
}

enum evmc_status_code mstore(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *index = StackTop_pop(&stack);
    struct uint256_t *value = StackTop_pop(&stack);

    if (!check_memory_u64(state, index, 32))
        return EVMC_OUT_OF_GAS;
    
    uint8_t out[32];
    memset(out, 0, 32);
    u256_to_u8_32(value, out);
    uint8_t *data = Memory_get(&state->memory, (size_t)LOWER(LOWER_P(index)));
    memcpy(data, out, 32);
    return EVMC_SUCCESS;
}

enum evmc_status_code mstore8(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *index = StackTop_pop(&stack);
    struct uint256_t *value = StackTop_pop(&stack);

    if (!check_memory_u64(state, index, 32))
        return EVMC_OUT_OF_GAS;
    
    uint8_t out[8];
    u256_to_u8_8(value, out);
    uint8_t *data = Memory_get(&state->memory, (size_t)LOWER(LOWER_P(index)));
    memcpy(data, out, 8);
    return EVMC_SUCCESS;
}

enum evmc_status_code sload(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t *x = StackTop_top(&stack);
    evmc_bytes32 key;
    memset(key.bytes, 0, 32);
    if(!zero256(x))
        u256_to_u8_32(x, key.bytes);
    
    if (state->rev >= EVMC_BERLIN && access_storage(&state->host_ctx, state->msg->recipient, key) == EVMC_ACCESS_COLD)
    {
        // The warm storage access cost is already applied (from the cost table).
        // Here we need to apply additional cold storage access cost.
        int16_t additional_cold_sload_cost = cold_sload_cost - warm_storage_read_cost;
        if ((state->gas_left -= additional_cold_sload_cost) < 0)
            return EVMC_OUT_OF_GAS;
    }
    evmc_bytes32 data = get_storage(&state->host_ctx, state->msg->recipient, key);
    readu256BE(data.bytes, x);
    return EVMC_SUCCESS;
}

enum evmc_status_code sstore(struct StackTop stack, struct ExecutionState *state){
    if(ExecutionState_in_static_mode(state))
        return EVMC_STATIC_MODE_VIOLATION;
    
    if(state->rev >= EVMC_ISTANBUL && state->gas_left <= 2300)
        return EVMC_OUT_OF_GAS;

    struct uint256_t *x = StackTop_pop(&stack);
    evmc_bytes32 key;
    memset(key.bytes, 0, 32);
    if(!zero256(x))
        u256_to_u8_32(x, key.bytes);
    x = StackTop_pop(&stack);
    evmc_bytes32 value;
    memset(value.bytes, 0, 32);
    if(!zero256(x))
        u256_to_u8_32(x, value.bytes);

    int cost = 0;
    if (state->rev >= EVMC_BERLIN && access_storage(&state->host_ctx, state->msg->recipient, key) == EVMC_ACCESS_COLD)
        cost = cold_sload_cost;

    enum evmc_status_code status = set_storage(&state->host_ctx, state->msg->recipient, key, value);

    switch (status)
    {
    case EVMC_STORAGE_UNCHANGED:
    case EVMC_STORAGE_MODIFIED_AGAIN:
        if (state->rev >= EVMC_BERLIN)
            cost += warm_storage_read_cost;
        else if (state->rev == EVMC_ISTANBUL)
            cost = 800;
        else if (state->rev == EVMC_CONSTANTINOPLE)
            cost = 200;
        else
            cost = 5000;
        break;
    case EVMC_STORAGE_MODIFIED:
    case EVMC_STORAGE_DELETED:
        if (state->rev >= EVMC_BERLIN)
            cost += 5000 - cold_sload_cost;
        else
            cost = 5000;
        break;
    case EVMC_STORAGE_ADDED:
        cost += 20000;
        break;
    }

    if ((state->gas_left -= cost) < 0)
        return EVMC_OUT_OF_GAS;
    return EVMC_SUCCESS;
}

code_iterator pc(struct StackTop stack, struct ExecutionState* state, code_iterator pos)
{   
    struct uint256_t tmp = {0, 0, 0, (uint64_t)(pos - state->code.data)};
    StackTop_push(&stack, &tmp);
    return pos + 1;
}

void msize(struct StackTop stack, struct ExecutionState *state)
{
    StackTop_push(&stack, &(struct uint256_t){0, 0, 0, (uint64_t)Memory_size(&state->memory)});
}

void gas(struct StackTop stack, struct ExecutionState *state)
{
    StackTop_push(&stack, &(struct uint256_t){0, 0, 0, (uint64_t)state->gas_left});
}

void push0(struct StackTop stack, struct ExecutionState *state)
{
    StackTop_push(&stack, &(struct uint256_t){0, 0, 0, 0});
}

// push

/// DUP instruction implementation.
void dup1(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 0));
    StackTop_push(&stack, &tmp);
}

void dup2(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 1));
    StackTop_push(&stack, &tmp);
}

void dup3(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 2));
    StackTop_push(&stack, &tmp);
}

void dup4(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 3));
    StackTop_push(&stack, &tmp);
}

void dup5(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 4));
    StackTop_push(&stack, &tmp);
}

void dup6(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 5));
    StackTop_push(&stack, &tmp);
}

void dup7(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 6));
    StackTop_push(&stack, &tmp);
}

void dup8(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 7));
    StackTop_push(&stack, &tmp);
}

void dup9(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 8));
    StackTop_push(&stack, &tmp);
}

void dup10(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 9));
    StackTop_push(&stack, &tmp);
}

void dup11(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 10));
    StackTop_push(&stack, &tmp);
}

void dup12(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 11));
    StackTop_push(&stack, &tmp);
}

void dup13(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 12));
    StackTop_push(&stack, &tmp);
}

void dup14(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 13));
    StackTop_push(&stack, &tmp);
}

void dup15(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 14));
    StackTop_push(&stack, &tmp);
}

void dup16(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_get(&stack, 15));
    StackTop_push(&stack, &tmp);
}

/// SWAP instruction implementation.
void swap1(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 1);
    *StackTop_get(&stack, 1) = tmp;
}

void swap2(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 2);
    *StackTop_get(&stack, 2) = tmp;
}

void swap3(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 3);
    *StackTop_get(&stack, 3) = tmp;
}

void swap4(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 4);
    *StackTop_get(&stack, 4) = tmp;
}

void swap5(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 5);
    *StackTop_get(&stack, 5) = tmp;
}

void swap6(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 6);
    *StackTop_get(&stack, 6) = tmp;
}

void swap7(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 7);
    *StackTop_get(&stack, 7) = tmp;
}

void swap8(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 8);
    *StackTop_get(&stack, 8) = tmp;
}

void swap9(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 9);
    *StackTop_get(&stack, 9) = tmp;
}

void swap10(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 10);
    *StackTop_get(&stack, 10) = tmp;
}

void swap11(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 11);
    *StackTop_get(&stack, 11) = tmp;
}

void swap12(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 12);
    *StackTop_get(&stack, 12) = tmp;
}

void swap13(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 13);
    *StackTop_get(&stack, 13) = tmp;
}

void swap14(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 14);
    *StackTop_get(&stack, 14) = tmp;
}

void swap15(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 15);
    *StackTop_get(&stack, 15) = tmp;
}

void swap16(struct StackTop stack, struct ExecutionState *state)
{
    struct uint256_t tmp = *(StackTop_top(&stack));
    *StackTop_top(&stack) = *StackTop_get(&stack, 16);
    *StackTop_get(&stack, 16) = tmp;
}

// LOG0-4
enum evmc_status_code log0(struct StackTop stack, struct ExecutionState *state)
{
    if(ExecutionState_in_static_mode(state))
        return EVMC_STATIC_MODE_VIOLATION;
    
    struct uint256_t *offset = StackTop_pop(&stack);
    struct uint256_t *size = StackTop_pop(&stack);

    if (!check_memory_u256(state, offset, size))
        return EVMC_OUT_OF_GAS;

    size_t o = (size_t)LOWER(LOWER_P(offset));
    size_t s = (size_t)LOWER(LOWER_P(size));

    int64_t cost = (int64_t)s * 8;
    if ((state->gas_left -= cost) < 0)
        return EVMC_OUT_OF_GAS;

    cvec_b32 topics;

    uint8_t *data = s != 0 ? Memory_get(&state->memory, o) : NULL;
    emit_log(&state->host_ctx, state->msg->recipient, data, s, topics.data, 0);
    return EVMC_SUCCESS;
}

enum evmc_status_code log1(struct StackTop stack, struct ExecutionState *state)
{
    if ((state->msg->flags & 1) != 0)
        return EVMC_STATIC_MODE_VIOLATION;
    struct uint256_t *offset = StackTop_pop(&stack);
    struct uint256_t *size = StackTop_pop(&stack);

    if (!check_memory_u64(state, offset, LOWER(LOWER_P(size))))
        return EVMC_OUT_OF_GAS;

    size_t o = (size_t)LOWER(LOWER_P(offset));
    size_t s = (size_t)LOWER(LOWER_P(size));

    int64_t cost = (int64_t)s * 8;
    if ((state->gas_left -= cost) < 0)
        return EVMC_OUT_OF_GAS;

    cvec_b32 topics;
    evmc_bytes32 topic;
    struct uint256_t* tmp = StackTop_pop(&stack);
    u256_to_u8_32(tmp, topic.bytes);
    cvec_b32_push(&topics, topic);

    uint8_t *data = s != 0 ? Memory_get(&state->memory, o) : NULL;
    emit_log(&state->host_ctx, state->msg->recipient, data, s, topics.data, 1);
    return EVMC_SUCCESS;
}

enum evmc_status_code log2_(struct StackTop stack, struct ExecutionState *state)
{
    if ((state->msg->flags & 1) != 0)
        return EVMC_STATIC_MODE_VIOLATION;
    struct uint256_t *offset = StackTop_pop(&stack);
    struct uint256_t *size = StackTop_pop(&stack);

    if (!check_memory_u64(state, offset, LOWER(LOWER_P(size))))
        return EVMC_OUT_OF_GAS;

    size_t o = (size_t)LOWER(LOWER_P(offset));
    size_t s = (size_t)LOWER(LOWER_P(size));

    int64_t cost = (int64_t)s * 8;
    if ((state->gas_left -= cost) < 0)
        return EVMC_OUT_OF_GAS;

    cvec_b32 topics;
    evmc_bytes32 topic;
    struct uint256_t *tmp;

    for (int i = 0; i < 2; i++)
    {
        tmp = StackTop_pop(&stack);
        u256_to_u8_32(tmp, topic.bytes);
        cvec_b32_push(&topics, topic);
    }

    uint8_t *data = s != 0 ? Memory_get(&state->memory, o) : NULL;
    emit_log(&state->host_ctx, state->msg->recipient, data, s, topics.data, 2);
    return EVMC_SUCCESS;
}

enum evmc_status_code log3(struct StackTop stack, struct ExecutionState *state)
{
    if ((state->msg->flags & 1) != 0)
        return EVMC_STATIC_MODE_VIOLATION;
    struct uint256_t *offset = StackTop_pop(&stack);
    struct uint256_t *size = StackTop_pop(&stack);

    if (!check_memory_u64(state, offset, LOWER(LOWER_P(size))))
        return EVMC_OUT_OF_GAS;

    size_t o = (size_t)LOWER(LOWER_P(offset));
    size_t s = (size_t)LOWER(LOWER_P(size));

    int64_t cost = (int64_t)s * 8;
    if ((state->gas_left -= cost) < 0)
        return EVMC_OUT_OF_GAS;

    cvec_b32 topics;
    evmc_bytes32 topic;
    struct uint256_t *tmp;

    for (int i = 0; i < 3; i++)
    {
        tmp = StackTop_pop(&stack);
        u256_to_u8_32(tmp, topic.bytes);
        cvec_b32_push(&topics, topic);
    }

    uint8_t *data = s != 0 ? Memory_get(&state->memory, o) : NULL;
    emit_log(&state->host_ctx, state->msg->recipient, data, s, topics.data, 3);
    return EVMC_SUCCESS;
}

enum evmc_status_code log4(struct StackTop stack, struct ExecutionState *state)
{
    if ((state->msg->flags & 1) != 0)
        return EVMC_STATIC_MODE_VIOLATION;
    struct uint256_t *offset = StackTop_pop(&stack);
    struct uint256_t *size = StackTop_pop(&stack);

    if (!check_memory_u64(state, offset, LOWER(LOWER_P(size))))
        return EVMC_OUT_OF_GAS;

    size_t o = (size_t)LOWER(LOWER_P(offset));
    size_t s = (size_t)LOWER(LOWER_P(size));

    int64_t cost = (int64_t)s * 8;
    if ((state->gas_left -= cost) < 0)
        return EVMC_OUT_OF_GAS;

    cvec_b32 topics;
    evmc_bytes32 topic;
    struct uint256_t *tmp;

    for (int i = 0; i < 4; i++)
    {
        tmp = StackTop_pop(&stack);
        u256_to_u8_32(tmp, topic.bytes);
        cvec_b32_push(&topics, topic);
    }

    uint8_t *data = s != 0 ? Memory_get(&state->memory, o) : NULL;
    emit_log(&state->host_ctx, state->msg->recipient, data, s, topics.data, 4);
    return EVMC_SUCCESS;
}

// PUSH

struct StopToken return_(struct StackTop stack, struct ExecutionState* state){
    struct uint256_t* offset = StackTop_get(&stack, 0);
    struct uint256_t* size = StackTop_get(&stack, 1);

    struct StopToken res;
    if(!check_memory_u256(state, offset, size)){
        res.status = EVMC_OUT_OF_GAS;
        return res;
    }
    state->output_size = (size_t)LOWER(LOWER_P(size));
    if(state->output_size != 0)
        state->output_offset = (size_t)LOWER(LOWER_P(offset));
    res.status = EVMC_SUCCESS;
    return res;
}

struct StopToken revert(struct StackTop stack, struct ExecutionState* state){
    struct StopToken res;
    
    struct uint256_t* offset = StackTop_get(&stack, 0);
    struct uint256_t* size = StackTop_get(&stack, 1);

    if(!check_memory_u256(state, offset, size)){
        res.status = EVMC_OUT_OF_GAS;
        return res;
    }

    state->output_size = (size_t)LOWER(LOWER_P(size));
    if(state->output_size != 0)
        state->output_offset = (size_t)LOWER(LOWER_P(offset));
    res.status = EVMC_REVERT;
    return res;
}

struct StopToken selfdestruct_(struct StackTop stack, struct ExecutionState* state){
    struct StopToken res;
    
    if(ExecutionState_in_static_mode(state)){
        res.status = EVMC_STATIC_MODE_VIOLATION;
        return res;
    }
    struct uint256_t* tmp = StackTop_get(&stack, 0);
    evmc_address beneficiary;
    u256_to_u8_20(tmp, beneficiary.bytes);

    if(state->rev >= EVMC_BERLIN && access_account(&state->host_ctx, beneficiary) == EVMC_ACCESS_COLD){
        if((state->gas_left -= cold_account_access_cost) < 0){
            res.status = EVMC_OUT_OF_GAS;
            return res;
        }
    }

    if(state->rev >= EVMC_TANGERINE_WHISTLE){
        struct uint256_t balance = {0, 0, 0, 0};
        readu256BE(get_balance(&state->host_ctx, state->msg->recipient).bytes, &balance);
        if(state->rev == EVMC_TANGERINE_WHISTLE || LOWER(LOWER(balance))){
            // After TANGERINE_WHISTLE apply additional cost of
            // sending value to a non-existing account.
            if(!account_exists(&state->host_ctx, beneficiary)){
                if((state->gas_left -= 25000) < 0){
                    res.status = EVMC_OUT_OF_GAS;
                }
            }
        }
    }

    selfdestruct(&state->host_ctx, state->msg->recipient, beneficiary);
    res.status = EVMC_SUCCESS;
    return res;
}

enum evmc_status_code call_(struct StackTop stack, struct ExecutionState* state){
    struct uint256_t* gas = StackTop_pop(&stack);
    struct uint256_t* tmp = StackTop_pop(&stack);
    evmc_address dst;
    memset(dst.bytes, 0, 20);
    u256_to_u8_20(tmp, dst.bytes);

    struct uint256_t* value = StackTop_pop(&stack);
    bool has_value = zero256(value);
    struct uint256_t* input_offset = StackTop_pop(&stack);
    struct uint256_t* input_size = StackTop_pop(&stack);
    struct uint256_t* output_offset = StackTop_pop(&stack);
    struct uint256_t* output_size = StackTop_pop(&stack);

    // Assume failure
    StackTop_push(&stack, &(struct uint256_t){0, 0, 0, 0});

    if(state->rev >= EVMC_BERLIN && access_account(&state->host_ctx, dst) == EVMC_ACCESS_COLD){
        if((state->gas_left -= additional_cold_account_access_cost) < 0){
            return EVMC_OUT_OF_GAS;
        }
    }

    if(!check_memory_u256(state, input_offset, input_size))
        return EVMC_OUT_OF_GAS;
        
    if(!check_memory_u256(state, output_offset, output_size))
        return EVMC_OUT_OF_GAS;

    struct evmc_message msg;
    msg.kind = EVMC_CALL;
    msg.flags = state->msg->flags;
    msg.depth = state->msg->depth + 1;
    msg.recipient = dst;
    msg.code_address = dst;
    msg.sender = state->msg->recipient;
    evmc_uint256be value_;
    memset(value_.bytes, 0, 32);
    u256_to_u8_32(value, value_.bytes);
    msg.value = value_;

    size_t input_size_ = (size_t)LOWER(LOWER_P(input_size));
    if(input_size_ > 0){
        msg.input_data = Memory_get(&state->memory, (size_t)LOWER(LOWER_P(input_offset)));
        msg.input_size = input_size_;
    }

    int64_t cost = has_value ? 9000 : 0;

    if(has_value && ExecutionState_in_static_mode(state))
        return EVMC_STATIC_MODE_VIOLATION;

    if((has_value || state->rev < EVMC_SPURIOUS_DRAGON) && !account_exists(&state->host_ctx, dst))
        cost += 25000;

    if((state->gas_left -= cost) < 0)
        return EVMC_OUT_OF_GAS;

    msg.gas = INT64_MAX;
    int64_t gas_ = (int64_t)LOWER(LOWER_P(gas)); 
    if(gas_ < msg.gas)
        msg.gas = gas_;

    if(state->rev >= EVMC_TANGERINE_WHISTLE)
        msg.gas = MIN(msg.gas, state->gas_left - state->gas_left / 64);
    else if(msg.gas > state->gas_left)
        return EVMC_OUT_OF_GAS;

    if(has_value){
        // Add stipend
        msg.gas += 2300;
        state->gas_left += 2300;
    }

    state->return_data = NULL;

    if(state->msg->depth >= 1024)
        return EVMC_SUCCESS;

    evmc_uint256be b = get_balance(&state->host_ctx, state->msg->recipient);
    struct uint256_t target = {0, 0, 0, 0};
    readu256BE(b.bytes, &target);
    if(has_value && gt256(value, &target))
        return EVMC_SUCCESS;

    struct evmc_result result = call(&state->host_ctx, msg);
    for(int i = 0; i < result.output_size; i++){
        *(state->return_data + i) = *(result.output_data + i);
    }
    *StackTop_top(&stack) = (struct uint256_t){0, 0, 0, (uint64_t)(result.status_code == EVMC_SUCCESS)};

    size_t copy_size = MIN((size_t)LOWER(LOWER_P(output_size)), result.output_size);
    if(copy_size > 0)
        memcpy(Memory_get(&state->memory, (size_t)LOWER(LOWER_P(output_offset))), result.output_data, copy_size);
    
    int64_t gas_used = msg.gas - result.gas_left;
    state->gas_left -= gas_used;
    return EVMC_SUCCESS;
}

enum evmc_status_code staticcall(struct StackTop stack, struct ExecutionState* state){
    struct uint256_t* gas = StackTop_pop(&stack);
    struct uint256_t* tmp = StackTop_pop(&stack);
    evmc_address dst;
    memset(dst.bytes, 0, 20);
    u256_to_u8_20(tmp, dst.bytes);

    struct uint256_t* value = &(struct uint256_t){0, 0, 0, 0};
    bool has_value = zero256(value);
    struct uint256_t* input_offset = StackTop_pop(&stack);
    struct uint256_t* input_size = StackTop_pop(&stack);
    struct uint256_t* output_offset = StackTop_pop(&stack);
    struct uint256_t* output_size = StackTop_pop(&stack);

    // Assume failure
    StackTop_push(&stack, &(struct uint256_t){0, 0, 0, 0});

    if(state->rev >= EVMC_BERLIN && access_account(&state->host_ctx, dst) == EVMC_ACCESS_COLD){
        if((state->gas_left -= additional_cold_account_access_cost) < 0){
            return EVMC_OUT_OF_GAS;
        }
    }

    if(!check_memory_u256(state, input_offset, input_size))
        return EVMC_OUT_OF_GAS;
        
    if(!check_memory_u256(state, output_offset, output_size))
        return EVMC_OUT_OF_GAS;

    struct evmc_message msg;
    msg.kind = EVMC_CALL;
    msg.flags = (uint32_t)EVMC_STATIC;
    msg.depth = state->msg->depth + 1;
    msg.recipient = dst;
    msg.code_address = dst;
    msg.sender = state->msg->recipient;
    evmc_uint256be value_;
    memset(value_.bytes, 0, 32);
    u256_to_u8_32(value, value_.bytes);
    msg.value = value_;

    size_t input_size_ = (size_t)LOWER(LOWER_P(input_size));
    if(input_size_ > 0){
        msg.input_data = Memory_get(&state->memory, (size_t)LOWER(LOWER_P(input_offset)));
        msg.input_size = input_size_;
    }

    int64_t cost = has_value ? 9000 : 0;

    if((state->gas_left -= cost) < 0)
        return EVMC_OUT_OF_GAS;

    msg.gas = INT64_MAX;
    int64_t gas_ = (int64_t)LOWER(LOWER_P(gas)); 
    if(gas_ < msg.gas)
        msg.gas = gas_;

    if(state->rev >= EVMC_TANGERINE_WHISTLE)
        msg.gas = MIN(msg.gas, state->gas_left - state->gas_left / 64);
    else if(msg.gas > state->gas_left)
        return EVMC_OUT_OF_GAS;

    if(has_value){
        // Add stipend
        msg.gas += 2300;
        state->gas_left += 2300;
    }

    state->return_data = NULL;

    if(state->msg->depth >= 1024)
        return EVMC_SUCCESS;

    evmc_uint256be b = get_balance(&state->host_ctx, state->msg->recipient);
    struct uint256_t target = {0, 0, 0, 0};
    readu256BE(b.bytes, &target);
    if(has_value && gt256(value, &target))
        return EVMC_SUCCESS;

    struct evmc_result result = call(&state->host_ctx, msg);
    for(int i = 0; i < result.output_size; i++){
        *(state->return_data + i) = *(result.output_data + i);
    }
    *StackTop_top(&stack) = (struct uint256_t){0, 0, 0, (uint64_t)(result.status_code == EVMC_SUCCESS)};

    size_t copy_size = MIN((size_t)LOWER(LOWER_P(output_size)), result.output_size);
    if(copy_size > 0)
        memcpy(Memory_get(&state->memory, (size_t)LOWER(LOWER_P(output_offset))), result.output_data, copy_size);
    
    int64_t gas_used = msg.gas - result.gas_left;
    state->gas_left -= gas_used;
    return EVMC_SUCCESS;
}

enum evmc_status_code delegatecall(struct StackTop stack, struct ExecutionState* state){
    struct uint256_t* gas = StackTop_pop(&stack);
    struct uint256_t* tmp = StackTop_pop(&stack);
    evmc_address dst;
    memset(dst.bytes, 0, 20);
    u256_to_u8_20(tmp, dst.bytes);

    struct uint256_t* value = &(struct uint256_t){0, 0, 0, 0};
    bool has_value = zero256(value);
    struct uint256_t* input_offset = StackTop_pop(&stack);
    struct uint256_t* input_size = StackTop_pop(&stack);
    struct uint256_t* output_offset = StackTop_pop(&stack);
    struct uint256_t* output_size = StackTop_pop(&stack);

    // Assume failure
    StackTop_push(&stack, &(struct uint256_t){0, 0, 0, 0});

    if(state->rev >= EVMC_BERLIN && access_account(&state->host_ctx, dst) == EVMC_ACCESS_COLD){
        if((state->gas_left -= additional_cold_account_access_cost) < 0){
            return EVMC_OUT_OF_GAS;
        }
    }

    if(!check_memory_u256(state, input_offset, input_size))
        return EVMC_OUT_OF_GAS;
        
    if(!check_memory_u256(state, output_offset, output_size))
        return EVMC_OUT_OF_GAS;

    struct evmc_message msg;
    msg.kind = EVMC_DELEGATECALL;
    msg.flags = state->msg->flags;
    msg.depth = state->msg->depth + 1;
    msg.recipient = state->msg->recipient;
    msg.code_address = dst;
    msg.sender = state->msg->sender;
    msg.value = state->msg->value;

    size_t input_size_ = (size_t)LOWER(LOWER_P(input_size));
    if(input_size_ > 0){
        msg.input_data = Memory_get(&state->memory, (size_t)LOWER(LOWER_P(input_offset)));
        msg.input_size = input_size_;
    }

    int64_t cost = has_value ? 9000 : 0;

    if((state->gas_left -= cost) < 0)
        return EVMC_OUT_OF_GAS;

    msg.gas = INT64_MAX;
    int64_t gas_ = (int64_t)LOWER(LOWER_P(gas)); 
    if(gas_ < msg.gas)
        msg.gas = gas_;

    if(state->rev >= EVMC_TANGERINE_WHISTLE)
        msg.gas = MIN(msg.gas, state->gas_left - state->gas_left / 64);
    else if(msg.gas > state->gas_left)
        return EVMC_OUT_OF_GAS;

    if(has_value){
        // Add stipend
        msg.gas += 2300;
        state->gas_left += 2300;
    }

    state->return_data = NULL;

    if(state->msg->depth >= 1024)
        return EVMC_SUCCESS;

    evmc_uint256be b = get_balance(&state->host_ctx, state->msg->recipient);
    struct uint256_t target = {0, 0, 0, 0};
    readu256BE(b.bytes, &target);
    if(has_value && gt256(value, &target))
        return EVMC_SUCCESS;

    struct evmc_result result = call(&state->host_ctx, msg);
    for(int i = 0; i < result.output_size; i++){
        *(state->return_data + i) = *(result.output_data + i);
    }
    *StackTop_top(&stack) = (struct uint256_t){0, 0, 0, (uint64_t)(result.status_code == EVMC_SUCCESS)};

    size_t copy_size = MIN((size_t)LOWER(LOWER_P(output_size)), result.output_size);
    if(copy_size > 0)
        memcpy(Memory_get(&state->memory, (size_t)LOWER(LOWER_P(output_offset))), result.output_data, copy_size);
    
    int64_t gas_used = msg.gas - result.gas_left;
    state->gas_left -= gas_used;
    return EVMC_SUCCESS;
}

enum evmc_status_code callcode(struct StackTop stack, struct ExecutionState* state){
    struct uint256_t* gas = StackTop_pop(&stack);
    struct uint256_t* tmp = StackTop_pop(&stack);
    evmc_address dst;
    memset(dst.bytes, 0, 20);
    u256_to_u8_20(tmp, dst.bytes);

    struct uint256_t* value = StackTop_pop(&stack);
    bool has_value = zero256(value);
    struct uint256_t* input_offset = StackTop_pop(&stack);
    struct uint256_t* input_size = StackTop_pop(&stack);
    struct uint256_t* output_offset = StackTop_pop(&stack);
    struct uint256_t* output_size = StackTop_pop(&stack);

    // Assume failure
    StackTop_push(&stack, &(struct uint256_t){0, 0, 0, 0});

    if(state->rev >= EVMC_BERLIN && access_account(&state->host_ctx, dst) == EVMC_ACCESS_COLD){
        if((state->gas_left -= additional_cold_account_access_cost) < 0){
            return EVMC_OUT_OF_GAS;
        }
    }

    if(!check_memory_u256(state, input_offset, input_size))
        return EVMC_OUT_OF_GAS;
        
    if(!check_memory_u256(state, output_offset, output_size))
        return EVMC_OUT_OF_GAS;

    struct evmc_message msg;
    msg.kind = EVMC_CALLCODE;
    msg.flags = state->msg->flags;
    msg.depth = state->msg->depth + 1;
    msg.recipient = state->msg->recipient;
    msg.code_address = dst;
    msg.sender = state->msg->recipient;
    evmc_uint256be value_;
    memset(value_.bytes, 0, 32);
    u256_to_u8_32(value, value_.bytes);
    msg.value = value_;

    size_t input_size_ = (size_t)LOWER(LOWER_P(input_size));
    if(input_size_ > 0){
        msg.input_data = Memory_get(&state->memory, (size_t)LOWER(LOWER_P(input_offset)));
        msg.input_size = input_size_;
    }

    int64_t cost = has_value ? 9000 : 0;

    if((state->gas_left -= cost) < 0)
        return EVMC_OUT_OF_GAS;

    msg.gas = INT64_MAX;
    int64_t gas_ = (int64_t)LOWER(LOWER_P(gas)); 
    if(gas_ < msg.gas)
        msg.gas = gas_;

    if(state->rev >= EVMC_TANGERINE_WHISTLE)
        msg.gas = MIN(msg.gas, state->gas_left - state->gas_left / 64);
    else if(msg.gas > state->gas_left)
        return EVMC_OUT_OF_GAS;

    if(has_value){
        // Add stipend
        msg.gas += 2300;
        state->gas_left += 2300;
    }

    state->return_data = NULL;

    if(state->msg->depth >= 1024)
        return EVMC_SUCCESS;

    evmc_uint256be b = get_balance(&state->host_ctx, state->msg->recipient);
    struct uint256_t target = {0, 0, 0, 0};
    readu256BE(b.bytes, &target);
    if(has_value && gt256(value, &target))
        return EVMC_SUCCESS;

    struct evmc_result result = call(&state->host_ctx, msg);
    for(int i = 0; i < result.output_size; i++){
        *(state->return_data + i) = *(result.output_data + i);
    }
    *StackTop_top(&stack) = (struct uint256_t){0, 0, 0, (uint64_t)(result.status_code == EVMC_SUCCESS)};

    size_t copy_size = MIN((size_t)LOWER(LOWER_P(output_size)), result.output_size);
    if(copy_size > 0)
        memcpy(Memory_get(&state->memory, (size_t)LOWER(LOWER_P(output_offset))), result.output_data, copy_size);
    
    int64_t gas_used = msg.gas - result.gas_left;
    state->gas_left -= gas_used;
    return EVMC_SUCCESS;
}

enum evmc_status_code create(struct StackTop stack, struct ExecutionState* state){
    if(ExecutionState_in_static_mode(state))
        return EVMC_STATIC_MODE_VIOLATION;

    struct uint256_t* endowment = StackTop_pop(&stack);
    struct uint256_t* init_code_offset = StackTop_pop(&stack);
    struct uint256_t* init_code_size = StackTop_pop(&stack);

    if(!check_memory_u256(state, init_code_offset, init_code_size))
        return EVMC_OUT_OF_GAS;
    
    struct uint256_t salt = (struct uint256_t){0, 0, 0, 0};

    StackTop_push(&stack, &salt);
    state->return_data = NULL;

    if(state->msg->depth >= 1024)
        return EVMC_SUCCESS;
    
    evmc_uint256be b = get_balance(&state->host_ctx, state->msg->recipient);
    struct uint256_t tmp = (struct uint256_t){0, 0, 0, 0};
    readu256BE(b.bytes, &tmp);
    if(!zero256(endowment) && gt256(endowment, &tmp))
        return EVMC_SUCCESS;
    
    struct evmc_message msg;
    msg.gas = state->gas_left;
    if(state->rev >= EVMC_TANGERINE_WHISTLE)
        msg.gas = msg.gas - msg.gas / 64;

    msg.kind = EVMC_CREATE;
    if((size_t)LOWER(LOWER_P(init_code_size)) > 0){
        msg.input_size = (size_t)LOWER(LOWER_P(init_code_size));
        memcpy(msg.input_data, Memory_get(&state->memory, (size_t)LOWER(LOWER_P(init_code_offset))), msg.input_size);
    }
    msg.sender = state->msg->recipient;
    msg.depth = state->msg->depth + 1;
    u256_to_u8_32(&salt, msg.create2_salt.bytes);
    u256_to_u8_32(endowment, msg.value.bytes);
    
    struct evmc_result result = call(&state->host_ctx, msg);
    state->gas_left -= msg.gas - result.gas_left;

    memcpy(state->return_data, result.output_data, result.output_size);
    if(result.status_code == EVMC_SUCCESS){
        evmc_address addr = result.create_address;
        uint8_t tmp[32];
        for (int i = 20; i < 32; i++){
            tmp[i] = 0;
        }
        for (int i = 19; i >= 0; i--){
            tmp[12 + i] = *(addr.bytes + i);
        }
        struct uint256_t target = {0, 0, 0, 0};
        readu256BE(tmp, &target);
        StackTop_push(&stack, &target);
    }
    
    return EVMC_SUCCESS;
}

enum evmc_status_code create2(struct StackTop stack, struct ExecutionState* state){
    if(ExecutionState_in_static_mode(state))
        return EVMC_STATIC_MODE_VIOLATION;

    struct uint256_t* endowment = StackTop_pop(&stack);
    struct uint256_t* init_code_offset = StackTop_pop(&stack);
    struct uint256_t* init_code_size = StackTop_pop(&stack);

    if(!check_memory_u256(state, init_code_offset, init_code_size))
        return EVMC_OUT_OF_GAS;
    
    struct uint256_t salt = *StackTop_pop(&stack);
    size_t salt_cost = num_words((size_t)LOWER(LOWER_P(init_code_size))) * 6;
    if((state->gas_left -= salt_cost) < 0)
        return EVMC_OUT_OF_GAS;

    StackTop_push(&stack, &salt);
    state->return_data = NULL;

    if(state->msg->depth >= 1024)
        return EVMC_SUCCESS;
    
    evmc_uint256be b = get_balance(&state->host_ctx, state->msg->recipient);
    struct uint256_t tmp = (struct uint256_t){0, 0, 0, 0};
    readu256BE(b.bytes, &tmp);
    if(!zero256(endowment) && gt256(endowment, &tmp))
        return EVMC_SUCCESS;
    
    struct evmc_message msg;
    msg.gas = state->gas_left;
    if(state->rev >= EVMC_TANGERINE_WHISTLE)
        msg.gas = msg.gas - msg.gas / 64;

    msg.kind = EVMC_CREATE2;
    if((size_t)LOWER(LOWER_P(init_code_size)) > 0){
        msg.input_size = (size_t)LOWER(LOWER_P(init_code_size));
        memcpy(msg.input_data, Memory_get(&state->memory, (size_t)LOWER(LOWER_P(init_code_offset))), msg.input_size);
    }
    msg.sender = state->msg->recipient;
    msg.depth = state->msg->depth + 1;
    u256_to_u8_32(&salt, msg.create2_salt.bytes);
    u256_to_u8_32(endowment, msg.value.bytes);
    
    struct evmc_result result = call(&state->host_ctx, msg);
    state->gas_left -= msg.gas - result.gas_left;

    memcpy(state->return_data, result.output_data, result.output_size);
    if(result.status_code == EVMC_SUCCESS){
        evmc_address addr = result.create_address;
        uint8_t tmp[32];
        for (int i = 20; i < 32; i++){
            tmp[i] = 0;
        }
        for (int i = 19; i >= 0; i--){
            tmp[12 + i] = *(addr.bytes + i);
        }
        struct uint256_t target = {0, 0, 0, 0};
        readu256BE(tmp, &target);
        StackTop_push(&stack, &target);
    }
    
    return EVMC_SUCCESS;
}