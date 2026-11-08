// Code.2 source-verifiable SAVE## to WORK control-flow model.
// NO actual filesystem mutation or live native Load Game integration.
#undef NDEBUG
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include "rloft/original_title_load_workflow.hpp"

using namespace rloft;

static_assert(observe_original_title_load_attempt({0,0,0,0,0}).end ==
              OriginalTitleLoadEnd::InvalidCallerSlot,
              "invalid slot may not launch unsafe IO");
static_assert(observe_original_title_load_attempt({1,1,0,0,0}).template_slash_is_nul,
              "original failed WORK chdir leaves SAVE?? separator NUL");
static_assert(observe_original_title_load_attempt({1,0,0,1,0}).end ==
              OriginalTitleLoadEnd::CopyFailedRetry,
              "original copy return value 1 retries, not returns");
static_assert(observe_original_title_load_attempt({10,0,0,0,73}).returned_status == 73,
              "original forwards shared 07938 return unchanged");

static bool exact_sequence(
    const OriginalTitleLoadAttempt& result,
    std::initializer_list<OriginalTitleLoadStep> expected) {
    if (result.count!=expected.size()) return false;
    std::size_t at=0;
    for (const auto step : expected) {
        if (result.steps[at++].step != step) return false;
    }
    return true;
}

static void check_handle_cleanup(
    const OriginalTitleLoadAttempt& r, std::size_t prefix) {
    assert(r.count==prefix+10U);
    assert(r.handles_closed==10);
    for (int handle=0;handle<10;++handle) {
        const auto entry=r.steps[prefix+static_cast<std::size_t>(handle)];
        assert(entry.step==OriginalTitleLoadStep::CloseHandle);
        assert(entry.value==handle);
    }
}

int main() {
    // Every original numbered save maps to decimal digits in the
    // DGROUP template at 6690 and 6691; never SAVE00 in this selector.
    for (int slot=1;slot<=10;++slot) {
        const auto ok=observe_original_title_load_attempt({
            static_cast<std::uint8_t>(slot),0,0,0,slot*13});
        assert(ok.slot_tens=='0'+slot/10);
        assert(ok.slot_ones=='0'+slot%10);
        assert(ok.steps[0].step==OriginalTitleLoadStep::SetTensDigit);
        assert(ok.steps[0].value=='0'+slot/10);
        assert(ok.steps[1].step==OriginalTitleLoadStep::MaskTemplateSlash);
        assert(ok.steps[2].step==OriginalTitleLoadStep::SetOnesDigit);
        assert(ok.steps[3].step==OriginalTitleLoadStep::EnterWork);
        assert(ok.steps[4].step==OriginalTitleLoadStep::ClearWorkFiles);
        assert(ok.steps[5].step==OriginalTitleLoadStep::ReturnToParent);
        assert(ok.steps[6].step==OriginalTitleLoadStep::CopySlotFiles);
        assert(ok.steps[6].value==slot);
        assert(ok.steps[7].step==OriginalTitleLoadStep::RestoreTemplateSlash);
        assert(ok.steps[8].step==OriginalTitleLoadStep::EnterSharedLoader);
        check_handle_cleanup(ok,9);
        assert(ok.end==OriginalTitleLoadEnd::SharedLoaderReturned);
        assert(ok.returns_to_caller && ok.returned_status==slot*13);
        assert(!ok.template_slash_is_nul && !ok.parent_error_displayed);
    }

    const auto work_failure=observe_original_title_load_attempt({
        4,-11,9,1,7});
    assert(work_failure.end==OriginalTitleLoadEnd::WorkChdirFailed);
    assert(work_failure.returns_to_caller && work_failure.returned_status==1);
    assert(work_failure.template_slash_is_nul);
    assert(!work_failure.parent_error_displayed);
    check_handle_cleanup(work_failure,4);
    assert(work_failure.steps[3].step==OriginalTitleLoadStep::EnterWork);

    const auto copy_failure=observe_original_title_load_attempt({
        5,0,0,1,-22});
    assert(copy_failure.end==OriginalTitleLoadEnd::CopyFailedRetry);
    assert(!copy_failure.returns_to_caller && copy_failure.handles_closed==0);
    assert(!copy_failure.template_slash_is_nul);
    assert(exact_sequence(copy_failure,{
        OriginalTitleLoadStep::SetTensDigit,
        OriginalTitleLoadStep::MaskTemplateSlash,
        OriginalTitleLoadStep::SetOnesDigit,
        OriginalTitleLoadStep::EnterWork,
        OriginalTitleLoadStep::ClearWorkFiles,
        OriginalTitleLoadStep::ReturnToParent,
        OriginalTitleLoadStep::CopySlotFiles,
        OriginalTitleLoadStep::RestoreTemplateSlash,
        OriginalTitleLoadStep::CopyErrorAndRetry}));

    // Original 07CBB warns on bad 'chdir ..' but still tries to copy,
    // even when the error-code status is nonzero.
    const auto parent_failure=observe_original_title_load_attempt({
        7,0,-1,0,17});
    assert(parent_failure.parent_error_displayed);
    assert(parent_failure.steps[6].step==
           OriginalTitleLoadStep::ParentDirectoryError);
    assert(parent_failure.steps[7].step==
           OriginalTitleLoadStep::CopySlotFiles);
    assert(parent_failure.steps[8].step==
           OriginalTitleLoadStep::RestoreTemplateSlash);
    assert(parent_failure.end==OriginalTitleLoadEnd::SharedLoaderReturned);
    check_handle_cleanup(parent_failure,10);

    const auto both_errors=observe_original_title_load_attempt({
        8,0,5,1,0});
    assert(both_errors.parent_error_displayed);
    assert(both_errors.end==OriginalTitleLoadEnd::CopyFailedRetry);
    assert(!both_errors.returns_to_caller);
    assert(both_errors.steps[9].step==
           OriginalTitleLoadStep::CopyErrorAndRetry);

    // Copy return !=1 is treated as non-failure by 07CE6, not 0 only.
    for (int copier : {-1,0,2,73}) {
        const auto r=observe_original_title_load_attempt({
            3,0,0,copier,-83});
        assert(r.end==OriginalTitleLoadEnd::SharedLoaderReturned);
        assert(r.returned_status==-83 && r.handles_closed==10);
    }
    for (int invalid : {0,11,255}) {
        const auto r=observe_original_title_load_attempt({
            static_cast<std::uint8_t>(invalid),0,0,0,0});
        assert(r.end==OriginalTitleLoadEnd::InvalidCallerSlot);
        assert(r.count==0 && r.handles_closed==0);
    }
    return 0;
}
