#include "rloft/original_dos_feedback_runtime.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

OriginalDosInputState
OriginalDosFeedbackRuntimeModel::capture_initial_f590_from_code2(
    const std::vector<std::uint8_t>& code2_file) {
    // Applies exclusively to the archived 558,689-byte CODE.2
    // object layout. Neither an arbitrary LE executable nor a
    // different patch version may safely reuse these offsets.
    constexpr std::size_t kOriginalSize=558689U;
    constexpr std::size_t kF572=0x75F24U+0xF572U;
    constexpr std::size_t kF590=0x75F24U+0xF590U;
    constexpr std::size_t kEntryCount=89U;
    if(code2_file.size()!=kOriginalSize)
        throw std::invalid_argument(
            "unsupported CODE.2 size for F590 capture");
    OriginalDosInputState result{};
    result.pending_f572=code2_file[kF572];
    result.mapping_f590.reserve(kEntryCount);
    for(std::size_t i=0U;i<kEntryCount;++i) {
        const auto at=kF590+i*4U;
        const auto value=static_cast<std::uint32_t>(code2_file[at]) |
            (static_cast<std::uint32_t>(code2_file[at+1U])<<8U) |
            (static_cast<std::uint32_t>(code2_file[at+2U])<<16U) |
            (static_cast<std::uint32_t>(code2_file[at+3U])<<24U);
        result.mapping_f590.push_back(value);
    }
    return result;
}

std::uint32_t OriginalDosFeedbackRuntimeModel::poll_30aec(
    OriginalDosInputState& state,
    const std::function<void()>& render_2f868) {
    // Original 30AEC checks live F572 again after EVERY graphics callback.
    while(state.pending_f572==0U) {
        if(!render_2f868)
            throw std::invalid_argument("30AEC requires live 2F868 service while waiting");
        render_2f868();
    }
    const auto index=static_cast<std::size_t>(state.pending_f572);
    // The original does not bounds-check F590. Reject an incomplete
    // host capture rather than fabricating an out-of-range DOS read.
    if(index>=state.mapping_f590.size())
        throw std::invalid_argument("uncaptured F590 input mapping DWORD");
    const auto result=state.mapping_f590[index];
    state.pending_f572=0U;
    return result;
}

bool OriginalDosFeedbackRuntimeModel::input_ready_30ad8(
    OriginalDosInputState& state,
    const std::function<void()>& render_2f868) {
    if(!render_2f868)
        throw std::invalid_argument("30AD8 requires original 2F868 service");
    render_2f868();
    return state.pending_f572!=0U;
}

std::uint32_t OriginalDosFeedbackRuntimeModel::peek_key_30b40(
    OriginalDosInputState& state,
    const std::function<void()>& render_2f868) {
    if(!render_2f868)
        throw std::invalid_argument("30B40 requires original 2F868 service");
    render_2f868();
    const auto index=static_cast<std::size_t>(state.pending_f572);
    // Original 30B4C reads a DWORD directly. A short captured host
    // table is an explicit unsupported snapshot, not an invented
    // original DOS bounds check. Do not consume F572.
    if(index>=state.mapping_f590.size())
        throw std::invalid_argument("uncaptured 30B40 F590 DWORD");
    return state.mapping_f590[index];
}

void OriginalDosFeedbackRuntimeModel::print_311e8(
    std::vector<std::uint8_t>& original_c_string,
    const std::function<void(const std::vector<std::uint8_t>&,
                             std::size_t)>& dos_int21_ah09) {
    if(!dos_int21_ah09)
        throw std::invalid_argument("311E8 requires DOS INT 21h AH=09 service");
    std::size_t terminator=0U;
    while(terminator<original_c_string.size() &&
          original_c_string[terminator]!=0U)
        ++terminator;
    if(terminator==original_c_string.size())
        throw std::invalid_argument("uncaptured 311E8 source-string terminator");

    for(std::size_t i=0U;i<terminator;++i)
        if(original_c_string[i]==0x24U)
            original_c_string[i]=0x2AU;
    // 31205..31212: DOS service observes '$' in the original buffer.
    original_c_string[terminator]=0x24U;
    dos_int21_ah09(original_c_string,terminator);
    original_c_string[terminator]=0U;
}

} // namespace rloft
