#include "rloft/type7_vga_plane_masks.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

constexpr std::size_t kDataFileStart = 0x75F24U;
constexpr std::array<std::size_t, 3> kLocations{
    0xC668U, 0xC688U, 0xC6A8U
};

// These 4*8 bytes for each plane are transcribed from the immutable
// CODE.2 object-2 data; all active length-1..7 slots are exercised.
constexpr std::array<std::uint8_t, 32> kFirst{
    0, 1, 3, 7, 15, 15, 15, 15,
    15, 2, 6, 14, 14, 14, 14, 14,
    14, 4, 12, 12, 12, 12, 12, 12,
    12, 8, 8, 8, 8, 8, 8, 8
};
constexpr std::array<std::uint8_t, 32> kSecond{
    8, 0, 0, 0, 0, 1, 3, 7,
    15, 0, 0, 0, 1, 3, 7, 15,
    15, 0, 0, 1, 3, 7, 15, 15,
    15, 0, 1, 3, 7, 15, 15, 15
};
constexpr std::array<std::uint8_t, 32> kThird{
    15, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 1,
    3, 0, 0, 0, 0, 0, 1, 3
};

std::vector<std::uint8_t> fake_code2() {
    std::vector<std::uint8_t> code2(
        kDataFileStart + 0xEF4DU + 1U, 0);
    constexpr std::array<std::array<std::uint8_t, 32>, 3> tables{
        kFirst, kSecond, kThird
    };
    for (std::size_t i = 0; i < tables.size(); ++i) {
        const auto start = kDataFileStart + kLocations[i];
        for (std::size_t j = 0; j < 32U; ++j) {
            code2[start + j] = tables[i][j];
        }
    }
    code2[kDataFileStart + 0xC029U] = 1U;
    return code2;
}

unsigned count_bits(std::uint8_t value) {
    unsigned sum = 0U;
    while (value != 0U) {
        sum += (value & 1U);
        value = static_cast<std::uint8_t>(value >> 1U);
    }
    return sum;
}

} // namespace

int main() {
    const auto code2 = fake_code2();
    const auto model =
        rloft::Type7VgaPlaneMaskModel::from_code2(code2);
    assert(model.initial_individual_columns_mode());
    assert(model.initial_video_page() == 0U);

    // Every 369D8 mask table slot in the active domain is checked against
    // the certified bytes and every plane union covers its run width.
    for (std::uint32_t phase = 0U; phase < 4U; ++phase) {
        for (std::uint8_t n = 1U; n <= 7U; ++n) {
            const auto masks = model.for_run(phase, n);
            const auto at =
                static_cast<std::size_t>(phase) * 8U + n;
            assert(masks.screen_phase == phase);
            assert(masks.column_count == n);
            assert(masks.planes[0] == kFirst[at]);
            assert(masks.planes[1] == kSecond[at]);
            assert(masks.planes[2] == kThird[at]);
            assert(count_bits(masks.planes[0]) +
                   count_bits(masks.planes[1]) +
                   count_bits(masks.planes[2]) == n);
        }
    }

    // Original CODE.2 3263C uses these SAME arrays for ordinary
    // walls. Its unsplit eight-column reads for phases 0,1,2 use
    // slots 8,16,24 (the next phase group's zero-index entry).
    for (std::uint32_t phase=0U; phase<3U; ++phase) {
        const auto m=model.for_surface_main_run(phase,8U);
        const auto at=static_cast<std::size_t>(phase)*8U+8U;
        assert(m.column_count==8U && m.screen_phase==phase);
        assert(m.planes[0]==kFirst[at]);
        assert(m.planes[1]==kSecond[at]);
        assert(m.planes[2]==kThird[at]);
        assert(count_bits(m.planes[0])+
               count_bits(m.planes[1])+
               count_bits(m.planes[2])==8U);
    }
    // Phase 3 width 8 indexes byte 32, past each captured
    // 32-byte array. No synthetic split or mask alias is valid.
    bool eight_rejected=false;
    try {(void)model.for_surface_main_run(3U,8U);}
    catch(const std::out_of_range&) {eight_rejected=true;}
    assert(eight_rejected);
    eight_rejected=false;
    try {(void)model.for_surface_main_run(0U,9U);}
    catch(const std::out_of_range&) {eight_rejected=true;}
    assert(eight_rejected);
    // Sprite callers continue to use the independently verified 1..7
    // domain; do not inadvertently widen their dispatch.
    eight_rejected=false;
    try {(void)model.for_run(0U,8U);}
    catch(const std::out_of_range&) {eight_rejected=true;}
    assert(eight_rejected);

    // The first 369D8 call always occurs; follow-up calls depend on mask.
    assert(model.for_run(0, 1).active_call_count() == 1U);
    assert(model.for_run(0, 5).active_call_count() == 2U);
    assert(model.for_run(2, 7).active_call_count() == 3U);
    assert(model.for_run(3, 7).planes[2] == 0x03U);
    assert(model.for_run(0xFFFFU, 1).planes[0] == 0x08U);

    bool rejected = false;
    try { (void)model.for_run(0, 0); }
    catch (const std::out_of_range&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try { (void)model.for_run(0, 8); }
    catch (const std::out_of_range&) { rejected = true; }
    assert(rejected);

    auto truncated = code2;
    truncated.pop_back();
    rejected = false;
    try { (void)rloft::Type7VgaPlaneMaskModel::from_code2(truncated); }
    catch (const std::runtime_error&) { rejected = true; }
    assert(rejected);

    // CODE.2's initial 0xC029 byte determines the default 36B14
    // dispatcher, and is toggled later by the executable at 0x6617.
    auto grouped_initial = code2;
    grouped_initial[kDataFileStart + 0xC029U] = 0U;
    assert(!rloft::Type7VgaPlaneMaskModel::
        from_code2(grouped_initial).
        initial_individual_columns_mode());
    grouped_initial[kDataFileStart + 0xC029U] = 2U;
    rejected = false;
    try {
        (void)rloft::Type7VgaPlaneMaskModel::
            from_code2(grouped_initial);
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);

    auto wrong_page = code2;
    wrong_page[kDataFileStart + 0xEF4DU] = 2U;
    rejected = false;
    try {
        (void)rloft::Type7VgaPlaneMaskModel::from_code2(wrong_page);
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);

    auto corrupt = code2;
    // A mutated eight-column lane must also fail at original load.
    corrupt[kDataFileStart+kLocations[2]+16U] = 0U;
    rejected=false;
    try {(void)rloft::Type7VgaPlaneMaskModel::from_code2(corrupt);}
    catch (const std::runtime_error&) {rejected=true;}
    assert(rejected);
    corrupt = code2;
    corrupt[kDataFileStart + kLocations[0] + 1U] = 0xFFU;
    rejected = false;
    try { (void)rloft::Type7VgaPlaneMaskModel::from_code2(corrupt); }
    catch (const std::runtime_error&) { rejected = true; }
    assert(rejected);
    return 0;
}
