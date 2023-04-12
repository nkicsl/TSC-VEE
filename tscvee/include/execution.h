#pragma once

#include "analysis_impl.h"

struct evmc_result execute_analyzed_code(struct AdvancedExecutionState *astate, struct CodeAnalysis *analysis)
{
    struct ExecutionState* state = &astate->estate;
    // Allow accessing the analysis by instructions.
    state->analysis.advanced = analysis;

    // Start with the first instruction.
    struct Instruction *instr = cvec_instr_begin(&state->analysis.advanced->instrs).ref;
    while (instr != NULL){
        instr = instr->fn(instr, astate);
    }
    size_t gas_left = (state->status == EVMC_SUCCESS || state->status == EVMC_REVERT) ? state->gas_left : 0;

    assert(state->output_size != 0 || state->output_offset == 0);
    return evmc_make_result(state->status, gas_left, Memory_data(&(state->memory)) + state->output_offset, state->output_size);
}

struct evmc_result execute(struct evmc_vm* vm, struct evmc_host_context *ctx, enum evmc_revision rev, struct evmc_message *msg, uint8_t *code, size_t code_size)
{
    struct CodeAnalysis analysis = analyze(rev, code, code_size);
    struct AdvancedExecutionState* astate = (struct AdvancedExecutionState*)malloc(sizeof(struct AdvancedExecutionState));
    AdvancedExecutionState_init_v(astate, msg, rev, ctx, code, code_size);
    return execute_analyzed_code(astate, &analysis);
}