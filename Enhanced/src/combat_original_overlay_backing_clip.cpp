#include "rloft/combat_original_overlay_backing_clip.hpp"

#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {
std::int64_t signed_dword(std::uint32_t bits) noexcept {
    return bits<=0x7FFFFFFFU
        ? static_cast<std::int64_t>(bits)
        : static_cast<std::int64_t>(bits)-0x100000000LL;
}
CombatOriginalOverlayClippedCall project(
    const CombatOriginalOverlayClipLimits& bounds,
    CombatOriginalOverlayBackingRequest args,
    bool restore) noexcept {
    // 292D1 and 29341 signed TEST EDX before clipping row.
    if(signed_dword(args.row_bits)<0) {
        args.row_count_bits+=args.row_bits;
        args.row_bits=0U;
    }
    // 292D9 and 29349 signed TEST EBX before clipping column.
    if(signed_dword(args.column_bits)<0) {
        args.column_count_bits+=args.column_bits;
        args.column_bits=0U;
    }

    // Original LEA row + count wraps as unsigned DWORD, then
    // CMP/JGE interprets both operands as SIGNED DWORD.
    const auto row_end=static_cast<std::uint32_t>(
        args.row_bits+args.row_count_bits);
    if(static_cast<std::int64_t>(bounds.last_row_ef3f)<
       signed_dword(row_end))
        args.row_count_bits=static_cast<std::uint32_t>(
            static_cast<std::uint32_t>(
                static_cast<std::int32_t>(bounds.last_row_ef3f))-
            args.row_bits);

    const auto column_end=static_cast<std::uint32_t>(
        args.column_bits+args.column_count_bits);
    if(static_cast<std::int64_t>(bounds.last_column_ef41)<
       signed_dword(column_end))
        args.column_count_bits=static_cast<std::uint32_t>(
            static_cast<std::uint32_t>(
                static_cast<std::int32_t>(bounds.last_column_ef41))-
            args.column_bits);

    // Original 292BC: BOTH EAX and ECX must be >0.
    // Original 2932C: EAX must be >0 but ECX only >=0.
    // Preserve this unusual zero-column restore behavior.
    const auto rows=signed_dword(args.row_count_bits);
    const auto columns=signed_dword(args.column_count_bits);
    return {rows>0 && (restore ? columns>=0 : columns>0),args};
}
} // namespace

CombatOriginalOverlayClippedCall
CombatOriginalOverlayClipModel::inspect_292bc(
    const CombatOriginalOverlayClipLimits& bounds,
    const CombatOriginalOverlayBackingRequest& args) noexcept {
    return project(bounds,args,false);
}

CombatOriginalOverlayClippedCall
CombatOriginalOverlayClipModel::inspect_2932c(
    const CombatOriginalOverlayClipLimits& bounds,
    const CombatOriginalOverlayBackingRequest& args) noexcept {
    return project(bounds,args,true);
}

CombatOriginalOverlayClippedCall
CombatOriginalOverlayClipModel::capture_292bc(
    const CombatOriginalOverlayClipLimits& bounds,
    const CombatOriginalOverlayBackingRequest& args,
    const CombatOriginalOverlayBackingServices& services) {
    const auto projection=inspect_292bc(bounds,args);
    if(projection.backend_called) {
        if(!services.capture_2a114)
            throw std::invalid_argument("original 2A114 VGA capture unavailable");
        services.capture_2a114(projection.args);
    }
    return projection;
}

CombatOriginalOverlayClippedCall
CombatOriginalOverlayClipModel::restore_2932c(
    const CombatOriginalOverlayClipLimits& bounds,
    const CombatOriginalOverlayBackingRequest& args,
    const CombatOriginalOverlayBackingServices& services) {
    const auto projection=inspect_2932c(bounds,args);
    if(projection.backend_called) {
        if(!services.restore_2a178)
            throw std::invalid_argument("original 2A178 VGA restore unavailable");
        services.restore_2a178(projection.args);
    }
    return projection;
}

} // namespace rloft
