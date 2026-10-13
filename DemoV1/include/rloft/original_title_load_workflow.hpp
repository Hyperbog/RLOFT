#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace rloft {

// Original CODE.2 SHA-256:
// 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314
// Object-1 logical 07C70..07D42, physical 0x3AF24 + logical.
//
// PURE ORIGINAL CONTROL-FLOW MODEL: never opens/copies/deletes a file.
// Inputs stand for the observed return values of original DOS functions.
// Do not connect this to native title Load until the entire SAVE## -> WORK
// and saved-world restoration behavior is implemented and source-verified.
enum class OriginalTitleLoadStep : std::uint8_t {
    SetTensDigit,            // 07C75 -> byte[6690]
    MaskTemplateSlash,       // 07C8C -> byte[6692] = 0
    SetOnesDigit,            // 07C93 -> byte[6691]
    EnterWork,               // 07C99 -> 33A5A("WORK")
    ClearWorkFiles,          // 07CA9 -> 07378(); result unused
    ReturnToParent,          // 07CB3 -> 33A5A("..")
    ParentDirectoryError,    // 07CC5 -> 2F580([60B0]); then continue
    CopySlotFiles,           // 07CD7 -> 073A0("SAVE##", "WORK")
    RestoreTemplateSlash,    // 07CDC -> byte[6692] = '\\'
    CopyErrorAndRetry,       // 07D15 -> 2F580([60C0]); jump 07BA4
    EnterSharedLoader,       // 07CEB -> 07938() if copy result != 1
    CloseHandle,             // 07CF4 or 07D28 -> 30040([12B30+4*i])
};

struct OriginalTitleLoadStepRecord {
    OriginalTitleLoadStep step{};
    int value{}; // ASCII digit, slot, handle index, or zero.
};

enum class OriginalTitleLoadEnd : std::uint8_t {
    InvalidCallerSlot,     // native guard only; original hit-test excludes it
    WorkChdirFailed,       // original returns 1 with separator still NUL
    CopyFailedRetry,       // original loops without closing ten handles
    SharedLoaderReturned,  // original returns exact 07938 status
};

struct OriginalTitleLoadObservation {
    std::uint8_t selected_slot{}; // selected by 07BF9..07C18: 1..10
    int enter_work_result{};      // 33A5A succeeds only when ==0
    int return_to_parent_result{};
    int copy_result{};            // 073A0 considers ==1 failure ONLY
    int shared_loader_result{};   // 07938 unmodified return value
};

struct OriginalTitleLoadAttempt {
    OriginalTitleLoadEnd end{OriginalTitleLoadEnd::InvalidCallerSlot};
    int returned_status{}; // meaningful only for WorkChdirFailed/LoaderReturned
    bool returns_to_caller{};
    bool template_slash_is_nul{};
    bool parent_error_displayed{};
    std::uint8_t handles_closed{};
    char slot_tens{};
    char slot_ones{};
    std::array<OriginalTitleLoadStepRecord, 24> steps{};
    std::size_t count{};

    constexpr void record(OriginalTitleLoadStep step, int value=0) noexcept {
        steps[count++]={step,value};
    }
};

// Performs exactly ONE accepted slot attempt after original mouse polling,
// page selection and 07C18 column decoding. Rendering/IRQ/file effects
// are deliberately not fabricated; the steps describe the required calls.
// The returned trace is evidence/planning, not a gameplay or IO engine.
[[nodiscard]] constexpr OriginalTitleLoadAttempt
observe_original_title_load_attempt(
    OriginalTitleLoadObservation input) noexcept {
    OriginalTitleLoadAttempt out{};
    if (input.selected_slot < 1U || input.selected_slot > 10U) {
        return out;
    }
    out.slot_tens=static_cast<char>('0'+input.selected_slot/10U);
    out.slot_ones=static_cast<char>('0'+input.selected_slot%10U);
    out.record(OriginalTitleLoadStep::SetTensDigit,out.slot_tens);
    out.record(OriginalTitleLoadStep::MaskTemplateSlash);
    out.template_slash_is_nul=true;
    out.record(OriginalTitleLoadStep::SetOnesDigit,out.slot_ones);
    out.record(OriginalTitleLoadStep::EnterWork);
    if (input.enter_work_result != 0) {
        // 07CA3 -> 07D1F; original does NOT execute 07CDC slash restore.
        // It closes ten globals and returns EAX=1, not the DOS error code.
        for (int i=0;i<10;++i) {
            out.record(OriginalTitleLoadStep::CloseHandle,i);
            ++out.handles_closed;
        }
        out.end=OriginalTitleLoadEnd::WorkChdirFailed;
        out.returns_to_caller=true;
        out.returned_status=1;
        return out;
    }
    // Original 07378 return value is ignored. It clears WORK BEFORE
    // the game knows whether selected SAVE## can be copied.
    out.record(OriginalTitleLoadStep::ClearWorkFiles);
    out.record(OriginalTitleLoadStep::ReturnToParent);
    if (input.return_to_parent_result != 0) {
        // 07CBB: show an error but intentionally proceed to wildcard copy.
        out.record(OriginalTitleLoadStep::ParentDirectoryError);
        out.parent_error_displayed=true;
    }
    out.record(OriginalTitleLoadStep::CopySlotFiles,input.selected_slot);
    out.record(OriginalTitleLoadStep::RestoreTemplateSlash);
    out.template_slash_is_nul=false;
    if (input.copy_result == 1) {
        out.record(OriginalTitleLoadStep::CopyErrorAndRetry);
        out.end=OriginalTitleLoadEnd::CopyFailedRetry;
        return out; // 07D1A JMP 07BA4, no 30040 close loop here
    }
    out.record(OriginalTitleLoadStep::EnterSharedLoader);
    for (int i=0;i<10;++i) {
        out.record(OriginalTitleLoadStep::CloseHandle,i);
        ++out.handles_closed;
    }
    out.end=OriginalTitleLoadEnd::SharedLoaderReturned;
    out.returns_to_caller=true;
    out.returned_status=input.shared_loader_result;
    return out;
}

} // namespace rloft
