#include "rloft/type7_rasterizer_patch.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

rloft::RendererScaleBlock block() {
    rloft::RendererScaleBlock out{};
    out.rasterizer_x86.assign(32U, 0x90U);
    for (std::size_t i = 0U; i < 129U; ++i) {
        out.code_patch_offset[i] =
            static_cast<std::uint16_t>(
                0x204U + (i < 30U ? i : 30U));
    }
    return out;
}

} // namespace

int main() {
    auto original = block();
    const auto original_bytes = original.rasterizer_x86;

    rloft::Type7RasterizerSetup normal{};
    normal.drawable = true;
    normal.patch_from = 126U;
    auto normal_copy =
        rloft::Type7RasterizerPatchModel::stage(
            original, normal);
    // Duplicate CODE.2 patch-table offsets are legal. Each patch captures
    // the then-current byte, allowing exact reverse restoration.
    assert(normal_copy.modifications.size() == 2U);
    assert(normal_copy.modifications[0].code_offset == 30U);
    assert(normal_copy.modifications[1].code_offset == 30U);
    assert(normal_copy.modifications[1].previous[0] == 0xC3U);
    assert(normal_copy.code[30] == 0xC3U);
    assert(original.rasterizer_x86 == original_bytes);
    rloft::Type7RasterizerPatchModel::restore(normal_copy);
    assert(normal_copy.code == original_bytes);
    assert(normal_copy.modifications.empty());

    rloft::Type7RasterizerSetup alternate{};
    alternate.drawable = true;
    alternate.nonzero_mode = true;
    alternate.patch_before = 3U;
    auto alternate_copy =
        rloft::Type7RasterizerPatchModel::stage(
            original, alternate);
    assert(alternate_copy.modifications.size() == 3U);
    assert(alternate_copy.code[0] == 0xEBU);
    // 0x204->0x205 jump length 1, relative displacement is 1-2 = -1.
    assert(alternate_copy.code[1] == 0xFFU);
    assert(alternate_copy.code[2] == 0xFFU);
    assert(alternate_copy.code[3] == 0x90U);
    rloft::Type7RasterizerPatchModel::restore(alternate_copy);
    assert(alternate_copy.code == original_bytes);

    // A same-offset short jump encodes the original modular 8-bit -2.
    original.code_patch_offset[1] = original.code_patch_offset[0];
    alternate.patch_before = 1U;
    auto wrapped =
        rloft::Type7RasterizerPatchModel::stage(
            original, alternate);
    assert(wrapped.code[0] == 0xEBU);
    assert(wrapped.code[1] == 0xFEU);
    rloft::Type7RasterizerPatchModel::restore(wrapped);
    assert(wrapped.code == original_bytes);

    rloft::Type7RasterizerSetup disabled{};
    auto skipped =
        rloft::Type7RasterizerPatchModel::stage(
            original, disabled);
    assert(skipped.code == original_bytes);
    assert(skipped.modifications.empty());

    original.code_patch_offset[126] = 0xFFFFU;
    normal.patch_from = 126U;
    bool rejected = false;
    try {
        (void)rloft::Type7RasterizerPatchModel::stage(
            original, normal);
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);
    return 0;
}
