#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace rloft {

// Original CODE.2 11AD8..11B68 ONLY. Subsequent 11B6B..11C19 item
// actions are deliberately not implemented by this bounded adapter.
// 6F90 is four DWORD contexts (not four independent host booleans).
struct OriginalActorModalModeState final {
    std::array<std::uint32_t,4> mode_6f90{};
    std::uint32_t c050{};
    std::uint32_t lane_7f6c{};
};

class OriginalActorModalTogglePrefix final {
public:
    static constexpr std::uint32_t kSourceF790Call=0x11B63U;

    // select_291b0 receives source zero-extended BYTE C050; it may
    // mutate the shared state. draw_11268 receives source actor index.
    // f790_11b63 MUST read 8600/8602 *after* redraw and handle original
    // saved-page restore, RNG and message through the ONE game runtime.
    // These are source callbacks, never Win32 host-frame callbacks.
    template<class Select291B0,class Draw11268,class InvokeF790>
    static void execute_11ad8_to_11b68(
        OriginalActorModalModeState& state,
        std::size_t selected_actor,
        std::int32_t original_input_flags,
        Select291B0&& select_291b0,
        Draw11268&& draw_11268,
        InvokeF790&& f790_11b63) {
        if(selected_actor>=state.mode_6f90.size())
            throw std::out_of_range("CODE.2 11AD8 actor index outside four contexts");

        if((original_input_flags & 2)!=0) {
            // 11AF0 reads the entire selected DWORD before any call;
            // 11B02 toggles DI once. 11B2E loops to 11B0E, NOT 11B02.
            const std::uint32_t mode=state.mode_6f90[selected_actor]^1U;
            const auto page=static_cast<std::uint8_t>(state.c050);
            std::forward<Select291B0>(select_291b0)(page);
            for(std::size_t actor=0;actor<state.mode_6f90.size();++actor) {
                state.mode_6f90[actor]=mode;
                // 11B0E IMUL actor,50h then 11B1B updates global 7F6C.
                state.lane_7f6c=static_cast<std::uint32_t>(actor*0x50U);
                draw_11268(actor);
            }
        } else {
            // 11B30 XOR BYTE mode_6f90[selected]. Other bytes stay
            // unchanged, as does 7F6C (set earlier by 12E70).
            state.mode_6f90[selected_actor]^=1U;
            const auto page=static_cast<std::uint8_t>(state.c050);
            std::forward<Select291B0>(select_291b0)(page);
            std::forward<Draw11268>(draw_11268)(selected_actor);
        }
        // 11B53..11B63 reloads held WORD 8600 and known BYTE 8602
        // AFTER all callbacks; no speculative early argument snapshot.
        std::forward<InvokeF790>(f790_11b63)();
    }
};

} // namespace rloft
