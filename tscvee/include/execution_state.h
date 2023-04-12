#pragma once

#include <stdio.h>
#include <assert.h>

#include "mocked_host.h"
#include "uint256.h"

/// Provides memory for EVM stack.
struct StackSpace
{
    /// The storage allocated for maximum possible number of items.
    /// Items are aligned to 256 bits for better packing in cache lines.
    struct uint256_t m_stack_space[1024];
};

/// Returns the pointer to the "bottom", i.e. below the stack space.
struct uint256_t *StackSpace_bottom(struct StackSpace *stackspace)
{
    return stackspace->m_stack_space - 1;
}

/// The EVM memory.
///
/// The implementations uses initial allocation of 4k and then grows capacity with 2x factor.
/// Some benchmarks has been done to confirm 4k is ok-ish value.
struct Memory
{
    /// The size of allocation "page".
    size_t page_size;
    /// Pointer to allocated memory.
    uint8_t *m_data;
    /// The "virtual" size of the memory.
    size_t m_size;
    /// The size of allocated memory. The initialization value is the initial capacity.
    size_t m_capacity;
};

void Memory_handle_out_of_memory(struct Memory *memory)
{
    printf("Error: out of memory.\n");
    exit(1);
}

void Memory_allocate_capacity(struct Memory *memory)
{
    memory->m_data = (uint8_t*)realloc(memory->m_data, memory->m_capacity);
    if (memory->m_data == NULL)
        Memory_handle_out_of_memory(memory);
}

/// Creates Memory object with initial capacity allocation.
void Memory_init(struct Memory *memory)
{
    memory->page_size = 4 * 1024;
    memory->m_data = NULL;
    memory->m_size = 0;
    memory->m_capacity = memory->page_size;
    Memory_allocate_capacity(memory);
    
}

/// Frees all allocated memory.
void Memory_free(struct Memory *memory)
{
    free(memory->m_data);
}

uint8_t *Memory_get(struct Memory *memory, size_t index)
{
    return memory->m_data + index;
}

uint8_t *Memory_data(struct Memory *memory)
{
    return memory->m_data;
}

size_t Memory_size(struct Memory *memory)
{
    return memory->m_size;
}

/// Grows the memory to the given size. The extend is filled with zeros.
///
/// @param new_size  New memory size. Must be larger than the current size and multiple of 32.
void Memory_grow(struct Memory *memory, size_t new_size)
{
    // Restriction for future changes. EVM always has memory size as multiple of 32 bytes.
    assert(new_size % 32 == 0);

    // Allow only growing memory. Include hint for optimizing compiler.
    assert(new_size > memory->m_size);
    
    if (new_size > memory->m_capacity)
    {
        // Double the capacity.
        memory->m_capacity *= 2;
        // If not enough.
        if (memory->m_capacity < new_size)
        {
            // Set capacity to required size rounded to multiple of page_size.
            memory->m_capacity = ((new_size + (memory->page_size - 1)) / memory->page_size) * memory->page_size;
        }
        Memory_allocate_capacity(memory);
    }
    memset(memory->m_data + memory->m_size, 0, new_size - memory->m_size);
    memory->m_size = new_size;
}

/// Virtually clears the memory by setting its size to 0. The capacity stays unchanged.
void Memory_clear(struct Memory *memory)
{
    memory->m_size = 0;
}

//-------------ExecutionState-------------
typedef uint8_t* bytes;

typedef struct bytes_view
{
    uint8_t *data;
    int size;
} bytes_view;

void bytes_view_init(struct bytes_view *bv)
{
    bv->data = NULL;
    bv->size = 0;
}

void bytes_view_init_v(struct bytes_view *bv, uint8_t *byte, uint64_t size)
{
    bv->data = byte;
    bv->size = size;
}

/// Generic execution state for generic instructions implementations.
struct ExecutionState
{
    int64_t gas_left;
    struct Memory memory;
    struct evmc_message *msg;
    // struct HostContext host;
    struct evmc_host_context host_ctx;
    enum evmc_revision rev;
    bytes return_data;

    /// Reference to original EVM code.
    /// TODO: Code should be accessed via code analysis only and this should be removed.
    bytes_view code;

    enum evmc_status_code status;
    size_t output_offset;
    size_t output_size;

    struct evmc_tx_context m_tx;

    /// Pointer to code analysis.
    /// This should be set and used internally by execute() function of a particular interpreter.
    union
    {
        struct CodeAnalysis *advanced;
    } analysis;

    /// Stack space allocation.
    ///
    /// This is the last field to make other fields' offsets of reasonable values.
    struct StackSpace stack_space;
};

void ExecutionState_init(struct ExecutionState* execution_state)
{
    execution_state->gas_left = 0;
    Memory_init(&execution_state->memory);
    execution_state->msg = NULL;

    MockedHost_init(&execution_state->host_ctx);
    execution_state->rev = EVMC_LONDON;
    execution_state->return_data = NULL;

    bytes_view_init(&execution_state->code);

    execution_state->status = EVMC_SUCCESS;
    execution_state->output_offset = 0;
    execution_state->output_size = 0;
}

void ExecutionState_init_v(struct ExecutionState* execution_state, struct evmc_message* message, enum evmc_revision revision, struct evmc_host_context* host_ctx, uint8_t* code, size_t code_size)
{
    execution_state->gas_left = message->gas;
    Memory_init(&execution_state->memory);
    execution_state->msg = message;
    
    execution_state->host_ctx = *host_ctx;
    execution_state->rev = revision;
    execution_state->return_data = NULL;
    execution_state->code.data = code;
    execution_state->code.size = code_size;

    execution_state->status = EVMC_SUCCESS;
    execution_state->output_offset = 0;
    execution_state->output_size = 0;
}

void ExecutionState_reset(struct ExecutionState* execution_state, struct evmc_message* message, enum evmc_revision revision, struct evmc_host_context* host_ctx, uint8_t* code, size_t code_size)
{
    execution_state->gas_left = message->gas;
    Memory_clear(&execution_state->memory);
    execution_state->msg = message;
    
    MockedHost_init(host_ctx);
    execution_state->rev = revision;
    execution_state->return_data = NULL;
    execution_state->code.data = code;
    execution_state->code.size = code_size;

    execution_state->status = EVMC_SUCCESS;
    execution_state->output_offset = 0;
    execution_state->output_size = 0;
    execution_state->analysis.advanced = NULL;
    evmcContext_init(execution_state->m_tx);
}

bool ExecutionState_in_static_mode(struct ExecutionState* execution_state){
    return (execution_state->msg->flags & EVMC_STATIC != 0);
    // return false;
}

struct evmc_tx_context ExecutionState_get_tx_context(struct ExecutionState* execution_state)
{
    if(execution_state->m_tx.block_timestamp == 0)
        execution_state->m_tx = get_tx_context(&execution_state->host_ctx);
    return execution_state->m_tx;
}