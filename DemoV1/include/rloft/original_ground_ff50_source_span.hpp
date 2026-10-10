#pragma once

#include <cstdint>
#include <optional>

namespace rloft {

// Original CODE.2 03838..03A64 initializes the mutable array at 15618:
// [0]=0 then [n]=4*n for 1<=n<=C738. C738 is the count of actual
// original SINTAB scale blocks, not SINTAB.runtime_lookup_max.
// CODE.2 1F255..1F2B1 derives FF50 from the nearest wall 22220,
// then 1F372..1F384 OVERWRITES FF50 with the lower textured span.
// These are original game rows, NOT host framebuffer clipping guesses.
//
// This is an isolated source-derived projection, NOT a live 22220
// estimator. Once 335C8/330F5 nearest wall state is implemented in
// the native renderer, the caller must supply that exact source value.
// Refuse out-of-table depths rather than read guessed adjacent memory.
struct OriginalGroundFF50Span final {
    std::int32_t nearest_22220{};
    std::uint32_t source_15618_value{}; // original EBX after 1F273
    std::int32_t upper_ff50{};
    std::int32_t lower_ff50{};
    bool sentinel_open_field{};

    [[nodiscard]] bool upper_has_textured_rows() const noexcept {
        return upper_ff50 > 0;
    }
    [[nodiscard]] bool lower_has_textured_rows() const noexcept {
        return lower_ff50 > 0;
    }
    // Original 34ABF caps the FIRST phase at 0x50=80 rows and
    // 34B4D draws any remaining FF50-80 rows with paired planes.
    [[nodiscard]] std::uint32_t lower_four_plane_rows() const noexcept {
        return lower_ff50 <= 0 ? 0U :
            static_cast<std::uint32_t>(lower_ff50 < 80 ? lower_ff50 : 80);
    }
    [[nodiscard]] std::uint32_t lower_paired_plane_rows() const noexcept {
        return lower_ff50 <= 80 ? 0U :
            static_cast<std::uint32_t>(lower_ff50 - 80);
    }
};

class OriginalGroundFF50SourceModel final {
public:
    static constexpr std::int32_t kNoWallSentinel=0x800;
    // From source CODE.2 SINTAB C738 (127 for supplied original).
    // Parameter is the original C738 read from actual SINTAB,
    // not host sweep count, rendered pixel width, or lookup_max=508.
    [[nodiscard]] static constexpr std::optional<OriginalGroundFF50Span>
    from_22220(std::int32_t nearest_depth,
               std::uint32_t original_c738_scale_count) noexcept {
        if(nearest_depth==kNoWallSentinel)
            return OriginalGroundFF50Span{
                nearest_depth,0U,128,128,true};
        // Original 335C8 initializes nearest depth to 0x800 and
        // 330F5 only lowers it. Negative/beyond-sentinel are not
        // a known drawable source-depth state. Without a table
        // entry, do not fabricate original FF50.
        if(nearest_depth<0 || nearest_depth>kNoWallSentinel)
            return std::nullopt;
        const auto index=static_cast<std::uint32_t>(nearest_depth/8);
        if(index>original_c738_scale_count ||
           index>static_cast<std::uint32_t>(kNoWallSentinel/8))
            return std::nullopt;
        // The source-built lookup stores DWORD [15618+4*index]=
        // index*4, as verified at original 03A20, including [0]=0.
        const auto ebx=static_cast<std::int32_t>(index*4U);
        const auto upper=128-ebx/2+ebx/8;
        // 1F377 adds the *previous* upper FF50 to EBX, and 1F37D
        // subtracts that sum from 0x100. Preserve both spans.
        const auto lower=256-(ebx+upper);
        return OriginalGroundFF50Span{
            nearest_depth,static_cast<std::uint32_t>(ebx),
            upper,lower,false};
    }
};

} // namespace rloft
