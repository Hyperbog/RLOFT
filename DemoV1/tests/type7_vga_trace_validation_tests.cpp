#include "rloft/type7_vga_trace_validation.hpp"

#include <cassert>
#include <cstdint>
#include <functional>
#include <stdexcept>

namespace {

rloft::Type7VgaSceneRun grouped() {
    rloft::Type7VgaSceneRun run{};
    run.source_x=31U;
    run.viewport_x=130U;
    run.width=4U;
    run.individual_columns_mode=false;
    run.dispatch.calls={
        {130,0xA0120U,0x1234U,0x0CU,0U},
        {130,0xA0121U,0x1234U,0x03U,1U},
    };
    run.dispatch.lookup_after_dispatch=0x3412U;
    return run;
}

rloft::Type7VgaSceneRun individual() {
    auto run=grouped();
    run.individual_columns_mode=true;
    run.dispatch.calls={
        {130,0xA0120U,0x1234U,0x04U,0U},
        {131,0xA0120U,0x3412U,0x08U,0U},
        {132,0xA0121U,0x1234U,0x01U,0U},
        {133,0xA0121U,0x3412U,0x02U,0U},
    };
    run.dispatch.lookup_after_dispatch=0x1234U;
    return run;
}

bool rejected(const rloft::Type7VgaSceneRun& run) {
    try {
        rloft::Type7VgaTraceValidator::validate(run);
    } catch(const std::invalid_argument&) {
        return true;
    } catch(const std::out_of_range&) {
        return true;
    }
    return false;
}

} // namespace

int main() {
    auto run=grouped();
    rloft::Type7VgaTraceValidator::validate(run);
    run=individual();
    rloft::Type7VgaTraceValidator::validate(run);

    auto bad=grouped();
    bad.dispatch.calls[0].sequencer_mask=0x08U;
    assert(rejected(bad));
    bad=grouped();
    bad.dispatch.calls[1].pass_index=2U;
    assert(rejected(bad));
    bad=grouped();
    bad.dispatch.calls[1].physical_address+=1U;
    assert(rejected(bad));
    bad=grouped();
    bad.dispatch.calls[0].ecx_lookup^=1U;
    assert(rejected(bad));
    bad=grouped();
    bad.dispatch.calls.push_back(bad.dispatch.calls.back());
    assert(rejected(bad));
    bad=grouped();
    bad.dispatch.calls.pop_back();
    assert(rejected(bad));
    bad=grouped();
    bad.dispatch.lookup_after_dispatch=0x1234U;
    assert(rejected(bad));

    bad=individual();
    bad.dispatch.calls[1].destination_column=130;
    assert(rejected(bad));
    bad=individual();
    bad.dispatch.calls[2].physical_address=0xA0120U;
    assert(rejected(bad));
    bad=individual();
    bad.dispatch.calls[1].ecx_lookup=0x1234U;
    assert(rejected(bad));
    bad=individual();
    bad.dispatch.lookup_after_dispatch=0x3412U;
    assert(rejected(bad));
    bad=individual();
    bad.individual_columns_mode=false;
    assert(rejected(bad));

    // A one-pixel call looks geometrically identical in both CODE.2
    // branches, but has different subsequent 105E7 lookup state.
    bad=grouped();
    bad.width=1U;
    bad.viewport_x=130U;
    bad.dispatch.calls.resize(1U);
    bad.dispatch.calls[0].sequencer_mask=0x04U;
    rloft::Type7VgaTraceValidator::validate(bad);
    bad.individual_columns_mode=true;
    assert(rejected(bad)); // needs restored, not swapped ECX
    bad.dispatch.lookup_after_dispatch=0x1234U;
    rloft::Type7VgaTraceValidator::validate(bad);

    bad=grouped();
    bad.width=8U;
    assert(rejected(bad));

    // Original lookup is carried between source-column draws, not
    // reloaded independently from each run. Two grouped runs swap the
    // bytes twice, restoring the original scene-level lookup word.
    rloft::WorldSceneRenderPlan plan{};
    plan.traversal.lookup_value=0x1234U;
    plan.visible_runs={
        {31U,0U,130U,4U},
        {32U,1U,134U,1U},
    };
    rloft::Type7VgaSceneTrace trace{};
    trace.runs.push_back(grouped());
    auto second=grouped();
    second.source_x=32U;
    second.source_column_index=1U;
    second.viewport_x=134U;
    second.width=1U;
    second.dispatch.calls={
        {134,0xA0121U,0x3412U,0x04U,0U},
    };
    second.dispatch.lookup_after_dispatch=0x1234U;
    trace.runs.push_back(second);
    trace.final_lookup_word=0x1234U;
    rloft::Type7VgaTraceValidator::validate_scene(trace,plan);

    auto corrupted=trace;
    corrupted.runs[1].dispatch.calls[0].ecx_lookup=0x1234U;
    bool caught=false;
    try {
        rloft::Type7VgaTraceValidator::validate_scene(corrupted,plan);
    } catch(const std::invalid_argument&) {caught=true;}
    assert(caught);

    corrupted=trace;
    corrupted.final_lookup_word=0x3412U;
    caught=false;
    try {
        rloft::Type7VgaTraceValidator::validate_scene(corrupted,plan);
    } catch(const std::invalid_argument&) {caught=true;}
    assert(caught);

    corrupted=trace;
    corrupted.runs[0].width=3U;
    caught=false;
    try {
        rloft::Type7VgaTraceValidator::validate_scene(corrupted,plan);
    } catch(const std::invalid_argument&) {caught=true;}
    assert(caught);

    corrupted=trace;
    corrupted.runs.pop_back();
    caught=false;
    try {
        rloft::Type7VgaTraceValidator::validate_scene(corrupted,plan);
    } catch(const std::invalid_argument&) {caught=true;}
    assert(caught);
}
