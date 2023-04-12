// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2019 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "advanced_execution.hpp"
#include "advanced_analysis.hpp"
#include <memory>
#include <sys/time.h>
#include <stdio.h> 

namespace evmone::advanced
{
evmc_result execute(AdvancedExecutionState& state, const AdvancedCodeAnalysis& analysis) noexcept
{
    state.analysis.advanced = &analysis;  // Allow accessing the analysis by instructions.

    const auto* instr = &state.analysis.advanced->instrs[0];  // Start with the first instruction.
    while (instr != nullptr){
        instr = instr->fn(instr, state);
    }

    const auto gas_left =
        (state.status == EVMC_SUCCESS || state.status == EVMC_REVERT) ? state.gas_left : 0;

    assert(state.output_size != 0 || state.output_offset == 0);
    return evmc::make_result(
        state.status, gas_left, state.memory.data() + state.output_offset, state.output_size);
}

evmc_result execute(evmc_vm* /*unused*/, const evmc_host_interface* host, evmc_host_context* ctx,
    evmc_revision rev, const evmc_message* msg, const uint8_t* code, size_t code_size) noexcept
{
    const auto analysis = analyze(rev, {code, code_size});
    auto state = std::make_unique<AdvancedExecutionState>(
        *msg, rev, *host, ctx, bytes_view{code, code_size});
    return execute(*state, analysis);
}
}  // namespace evmone::advanced
