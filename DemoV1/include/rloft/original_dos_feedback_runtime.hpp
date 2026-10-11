#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace rloft {

// A read-only/captured subset of original CODE.2 DGROUP. An incomplete
// host capture is an error; no synthetic DOS keyboard or scan-code mapping.
struct OriginalDosInputState final {
    std::uint8_t pending_f572{};
    std::vector<std::uint32_t> mapping_f590;
};

class OriginalDosFeedbackRuntimeModel final {
public:
    // Read the original 89 DWORD keyboard translations from the
    // supplied original CODE.2 install asset's known LE/DGROUP layout.
    // Source-file SHA must be verified externally: this is a byte
    // decoder, not a universal patch/EXE format detector.
    // F590[0..88] occupies file offsets 0x854B4..0x85617;
    // F6F4 begins the next unrelated DWORD global.
    [[nodiscard]] static OriginalDosInputState
    capture_initial_f590_from_code2(const std::vector<std::uint8_t>& code2_file);

    // CODE.2 30AEC: while F572==0 invoke 2F868 (which can invoke F0C8),
    // then use BYTE[F572] as the DWORD index at F590 and clear F572.
    // The injected render callback may update the input or mapping state.
    // No host iteration cap: the DOS code also waits until input arrives.
    [[nodiscard]] static std::uint32_t poll_30aec(
        OriginalDosInputState& state,
        const std::function<void()>& render_2f868);

    // CODE.2 311E8: edit caller-owned DOS bytes before AH=09 service.
    // Converts embedded '$' to '*', overwrites the final NUL with '$',
    // calls the service on this SAME mutable backing, then restores NUL.
    // An incomplete host capture (no NUL) is rejected before mutation.
    // The callback sees the original backing and the temporary '$'.
    static void print_311e8(
        std::vector<std::uint8_t>& original_c_string,
        const std::function<void(const std::vector<std::uint8_t>&,
                                 std::size_t)>& dos_int21_ah09);
};

} // namespace rloft
