#include <inttypes.h>

#include "analysis.h"
#include "opcode_helpers.h"

struct BlockAnalysis
{
    int64_t gas_cost;

    int stack_req;
    int stack_max_growth;
    int stack_change;

    /// The index of the beginblock instruction that starts the block.
    /// This is the place where the analysis data is going to be dumped.
    size_t begin_block_index;
};

void BlockAnalysis_init(struct BlockAnalysis *ba)
{
    ba->gas_cost = 0;
    ba->stack_req = 0;
    ba->stack_max_growth = 0;
    ba->stack_change = 0;
    ba->begin_block_index = 0;
}

void BlockAnalysis_init_v(struct BlockAnalysis *ba, size_t index)
{
    ba->gas_cost = 0;
    ba->stack_req = 0;
    ba->stack_max_growth = 0;
    ba->stack_change = 0;
    ba->begin_block_index = index;
}

/// Close the current block by producing compressed information about the block.
struct BlockInfo BlockAnalysis_close(struct BlockAnalysis *ba)
{
    struct BlockInfo bi;
    uint32_t gc = (uint32_t)ba->gas_cost <= UINT32_MAX ? (uint32_t)ba->gas_cost : UINT32_MAX;
    int16_t sr = (int16_t)ba->stack_req <= INT16_MAX ? (int16_t)ba->stack_req : INT16_MAX;
    int16_t smg = (int16_t)ba->stack_max_growth <= INT16_MAX ? (int16_t)ba->stack_max_growth : INT16_MAX;
    BlockInfo_init_v(&bi, gc, sr, smg);
    return bi;
}

struct CodeAnalysis analyze(enum evmc_revision rev, uint8_t* code, size_t code_size)
{
    OpTable op_tbl = get_op_table(rev);
    instruction_exec_fn opx_beginblock_fn = op_tbl.table[OPX_BEGINBLOCK].fn;

    struct CodeAnalysis analysis;
    CodeAnalysis_init(&analysis);

    int max_instrs_size = code_size + 1;
    cvec_instr_reserve(&analysis.instrs, max_instrs_size);

    // This is 2x more than needed but using (code.size() / 2 + 1) increases page-faults 1000x.
    int max_args_storage_size = code_size + 1;
    cvec_u256_reserve(&analysis.push_values, max_args_storage_size);

    // Create the first block.
    struct Instruction ins;
    Instrcution_init_v(&ins, opx_beginblock_fn);
    cvec_instr_push(&analysis.instrs, ins);
    struct BlockAnalysis block;
    BlockAnalysis_init(&block);

    // TODO: Iterators are not used here because push_end may point way outside of code
    //       and this is not allowed and MSVC will detect it with instrumented iterators.
    uint8_t *code_begin = code;
    uint8_t *code_end = code_begin + code_size;
    uint8_t *code_pos = code_begin;
    while (code_pos != code_end)
    {
        uint8_t opcode = *code_pos++;
        struct OpTableEntry opcode_info = op_tbl.table[opcode];

        if(opcode == 247){
            opcode_info.stack_req = 0;
            opcode_info.stack_change = 0;
            opcode_info.gas_cost = 0;
        }

        block.stack_req = max(block.stack_req, opcode_info.stack_req - block.stack_change);
        block.stack_change += opcode_info.stack_change;
        block.stack_max_growth = max(block.stack_max_growth, block.stack_change);

        block.gas_cost += opcode_info.gas_cost;

        if (opcode == OP_JUMPDEST)
        {
            // The JUMPDEST is always the first instruction in the block.
            // We don't have to insert anything to the instruction table.
            cvec_int32_push(&analysis.jumpdest_offsets, (int32_t)(code_pos - code_begin - 1));
            cvec_int32_push(&analysis.jumpdest_targets, (int32_t)(cvec_instr_size(analysis.instrs) - 1));
        }
        else
        {
            struct Instruction ins;
            Instrcution_init_v(&ins, opcode_info.fn);
            cvec_instr_push(&analysis.instrs, ins);
        }

        struct Instruction* curr_ins = cvec_instr_back(&analysis.instrs);

        // A flag whenever this is a block terminating instruction.
        bool is_terminator = false;
        switch (opcode)
        {
        default:
            break;

        case OP_JUMP:
        case OP_JUMPI:
        case OP_STOP:
        case OP_RETURN:
        case OP_REVERT:
        case OP_SELFDESTRUCT:
            is_terminator = true;
            break;

        case ANY_SMALL_PUSH:
        {
            size_t push_size = (size_t)(opcode - OP_PUSH1) + 1;
            uint8_t *push_end = min(code_pos + push_size, code_end);

            uint64_t value = 0;
            size_t insert_bit_pos = (push_size - 1) * 8;
            while (code_pos < push_end)
            {
                value |= (uint64_t)*((code_pos++)) << insert_bit_pos;
                insert_bit_pos -= 8;
            }
            curr_ins->arg.small_push_value = value;
            break;
        }

        case ANY_LARGE_PUSH:
        {
            size_t push_size = (size_t)(opcode - OP_PUSH1) + 1;
            uint8_t *push_end = code_pos + push_size;

            // Copy bytes to the deticated storage in the order to match native endianness.
            // The condition `code_pos < code_end` is to handle the edge case of PUSH being at
            // the end of the code with incomplete value bytes.
            // This condition can be replaced with single `push_end <= code_end` done once before
            // the loop. Then the push value will stay 0 but the value is not reachable
            // during the execution anyway.
            // This seems like a good micro-optimization but we were not able to show
            // this is faster, at least with GCC 8 (producing the best results at the time).
            // FIXME: Add support for big endian architectures.
            uint8_t buffer[32];
            memset(buffer, 0, 32);
            uint8_t index = 32 - push_size;
            while(index < 32 && code_pos < push_end && code_pos < code_end){
                buffer[index++] = *code_pos++;
            }

            struct uint256_t* push_value = (struct uint256_t*)malloc(sizeof(struct uint256_t));
            readu256BE(buffer, push_value);

            cvec_u256_push(&analysis.push_values, *push_value);
            curr_ins->arg.push_value = push_value;
            break;
        }

        case OP_GAS:
        case OP_CALL:
        case OP_CALLCODE:
        case OP_DELEGATECALL:
        case OP_STATICCALL:
        case OP_CREATE:
        case OP_CREATE2:
        case OP_SSTORE:
            curr_ins->arg.number = block.gas_cost;
            break;

        case OP_PC:
            curr_ins->arg.number = code_pos - code_begin - 1;
            break;
        }

        // If this is a terminating instruction or the next instruction is a JUMPDESTWW
        if (is_terminator || (code_pos != code_end && *code_pos == OP_JUMPDEST))
        {
            // Save current block.
            cvec_instr_at_mut(&analysis.instrs, block.begin_block_index)->arg.block = BlockAnalysis_close(&block);

            // Create a new block.
            struct Instruction tmp_ins;
            Instrcution_init_v(&tmp_ins, opx_beginblock_fn);
            cvec_instr_push(&analysis.instrs, tmp_ins);
            BlockAnalysis_init_v(&block, cvec_instr_size(analysis.instrs) - 1);
        }
    }

    // Save current block.
    cvec_instr_at_mut(&analysis.instrs, block.begin_block_index)->arg.block = BlockAnalysis_close(&block);

    // Make sure the last block is terminated.
    // TODO: This is not needed if the last instruction is a terminating one.
    struct Instruction ins_stop;
    Instrcution_init_v(&ins_stop, op_tbl.table[OP_STOP].fn);
    cvec_instr_push(&analysis.instrs, ins_stop);

    // Make sure the push_values has not been reallocated. Otherwise iterators are invalid.
    assert(cvec_u256_size(analysis.push_values) <= max_args_storage_size);

    return analysis;
}