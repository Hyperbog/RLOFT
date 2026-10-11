#include "rloft/original_dos_feedback_runtime.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

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
