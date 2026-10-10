#pragma once

#include <cstdint>
#include <optional>

#include "rloft/original_ground_ff50_source_span.hpp"

namespace rloft {

// Bounded source-stage plan for original CODE.2 function_1f1c8.
// Original 1F2B1..1F369 makes an UPPER choice; 1F369..1F3DA
// decides the LOWER path *after* any upper overlay callbacks.
// This does not invoke DOS drawing, palette mutation, 291B0,
// original RNG or Win32 rendering.
enum class OriginalUpperSurfaceAction : std::uint8_t {
    Overlay29B40,            // active 258A8 and kind 258B0==0x2C
    Overlay314B0,            // active 258A8, kind !=2C, C028 !=0
    Fill33E28,               // other active / C027==0
    Textured34A74,           // active==FF, C027 !=0, upper FF50>0
    SkipNonpositiveFF50,     // active==FF, C027 !=0, upper FF50<=0
};
enum class OriginalLowerSurfaceAction : std::uint8_t {
    Fill33DE0,               // C026==0, no lower FF50 recomputation
    Textured34A74,           // C026!=0, recomputed FF50>0
    SkipNonpositiveFF50,     // C026!=0, recomputed FF50<=0
};

// The caller must get 22220 from original 335C8/330F5 materializers,
// not infer nearest depth by min(native sweep depth), and must preserve
// the mutation/call sequence of upper callbacks before plan_lower().
struct OriginalUpperSurfaceInput final {
    std::int32_t original_nearest_22220{};
    std::uint32_t original_c738_scale_count{};
    std::uint32_t original_heading_c658{};
    std::uint32_t original_last_ff58_heading_bff0{};
    std::uint8_t original_c026{};
    std::uint8_t original_c027{};
    std::uint8_t original_c028{};
    std::uint8_t original_selected_258a8{0xFFU};
    std::uint8_t original_kind_258b0{};
};

struct OriginalUpperSurfaceStage final {
    OriginalUpperSurfaceAction action{};
    bool recompute_original_34998{};
    std::uint32_t original_ebx_15618{};
    std::int32_t initial_upper_ff50{};
    std::uint8_t original_selection_258a8{};
    std::uint8_t original_kind_258b0{};
};

struct OriginalLowerSurfaceStage final {
    OriginalLowerSurfaceAction action{};
    std::int32_t post_upper_ff50{};
    std::int32_t computed_lower_ff50{};
    std::uint8_t original_flat_fill_kind_258b1{};
};

class OriginalGroundSurfaceDispatch final {
public:
    [[nodiscard]] static constexpr
    std::optional<OriginalUpperSurfaceStage> plan_upper(
        const OriginalUpperSurfaceInput& input) noexcept {
        const auto span=OriginalGroundFF50SourceModel::from_22220(
            input.original_nearest_22220,
            input.original_c738_scale_count);
        if(!span)return std::nullopt;

        OriginalUpperSurfaceStage plan{};
        plan.original_ebx_15618=span->source_15618_value;
        plan.initial_upper_ff50=span->upper_ff50;
        plan.original_selection_258a8=input.original_selected_258a8;
        plan.original_kind_258b0=input.original_kind_258b0;
        // 1F227..1F250 compares original byte modes first, then the
        // cached BFF0 heading. No update/call if modes both zero.
        plan.recompute_original_34998=
            (input.original_c026!=0U || input.original_c027!=0U) &&
            input.original_last_ff58_heading_bff0!=input.original_heading_c658;

        // 1F2B1..1F30E first prioritizes active overlay 258A8.
        if(input.original_selected_258a8!=0xFFU) {
            plan.action=input.original_kind_258b0==0x2CU
                ? OriginalUpperSurfaceAction::Overlay29B40
                : input.original_c028!=0U
                    ? OriginalUpperSurfaceAction::Overlay314B0
                    : OriginalUpperSurfaceAction::Fill33E28;
        } else if(input.original_c027==0U) {
            plan.action=OriginalUpperSurfaceAction::Fill33E28;
        } else {
            plan.action=span->upper_ff50>0
                ? OriginalUpperSurfaceAction::Textured34A74
                : OriginalUpperSurfaceAction::SkipNonpositiveFF50;
        }
        return plan;
    }

    [[nodiscard]] static constexpr OriginalLowerSurfaceStage plan_lower(
        const OriginalUpperSurfaceStage& upper,
        std::int32_t original_ff50_after_upper_callbacks,
        std::uint8_t original_c026,
        std::uint8_t original_kind_258b1) noexcept {
        OriginalLowerSurfaceStage lower{};
        lower.post_upper_ff50=original_ff50_after_upper_callbacks;
        lower.original_flat_fill_kind_258b1=original_kind_258b1;
        if(original_c026==0U) {
            // 1F369/1F370 jumps to 1F3CA: source does NOT change FF50.
            lower.action=OriginalLowerSurfaceAction::Fill33DE0;
            lower.computed_lower_ff50=original_ff50_after_upper_callbacks;
            return lower;
        }
        // 1F372 MOV 100h, 1F377 ADD EBX,[FF50], 1F37D SUB
        // original x86 DWORD arithmetic wraps mod 2^32.
        const auto raw=0x100U-(
            upper.original_ebx_15618+
            static_cast<std::uint32_t>(original_ff50_after_upper_callbacks));
        const auto signed_value=(raw&0x80000000U)!=0U
            ? static_cast<std::int32_t>(
                static_cast<std::int64_t>(raw)-0x100000000LL)
            : static_cast<std::int32_t>(raw);
        lower.computed_lower_ff50=signed_value;
        lower.action=signed_value>0
            ? OriginalLowerSurfaceAction::Textured34A74
            : OriginalLowerSurfaceAction::SkipNonpositiveFF50;
        return lower;
    }
};

} // namespace rloft
