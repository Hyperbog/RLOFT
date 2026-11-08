#include "rloft/original_dos_keyboard_session.hpp"

#include "rloft/original_dos_feedback_runtime.hpp"

#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

namespace rloft {

OriginalDosKeyboardSession::OriginalDosKeyboardSession(
    OriginalDosInputState initial) {
    state_.input=std::move(initial);
    // Mutable CODE.2 BSS keyboard globals are freshly zero-initialized.
    // A SAVE.DAT restore must populate its own live state separately.
}

OriginalDosKeyboardSession
OriginalDosKeyboardSession::from_installed_code2(
    const std::vector<std::uint8_t>& code2_file) {
    return OriginalDosKeyboardSession(
        OriginalDosFeedbackRuntimeModel::capture_initial_f590_from_code2(
            code2_file));
}

bool OriginalDosKeyboardSession::receive_port60(std::uint8_t raw_scan) {
    return OriginalDosKeyboardIrqModel::dispatch_30950(state_,raw_scan);
}

bool OriginalDosKeyboardSession::held(std::uint8_t scan) const {
    return OriginalDosKeyboardIrqModel::held_308bc(state_,scan);
}

std::uint32_t OriginalDosKeyboardSession::modifiers(
    std::uint32_t mask) const {
    return OriginalDosKeyboardIrqModel::modifiers_30abc(state_,mask);
}

std::uint8_t OriginalDosKeyboardSession::consume_space() {
    return OriginalDosKeyboardIrqModel::consume_space_30ac8(state_);
}

bool OriginalDosKeyboardSession::input_ready(
    const std::function<void()>& draw_2f868) {
    return OriginalDosFeedbackRuntimeModel::input_ready_30ad8(
        state_.input,draw_2f868);
}

std::uint32_t OriginalDosKeyboardSession::peek_key(
    const std::function<void()>& draw_2f868) {
    return OriginalDosFeedbackRuntimeModel::peek_key_30b40(
        state_.input,draw_2f868);
}

std::uint32_t OriginalDosKeyboardSession::consume_key(
    const std::function<void()>& draw_2f868) {
    return OriginalDosFeedbackRuntimeModel::poll_30aec(
        state_.input,draw_2f868);
}

const OriginalDosKeyboardIrqState&
OriginalDosKeyboardSession::state() const noexcept {
    return state_;
}

} // namespace rloft
