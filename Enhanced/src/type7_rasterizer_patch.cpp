#include "rloft/type7_rasterizer_patch.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {

std::size_t code_offset(
    const RendererScaleBlock& block,
    std::size_t patch_index,
    std::size_t code_size,
    std::size_t bytes_needed) {
    if (patch_index >= block.code_patch_offset.size()) {
        throw std::out_of_range(
            "type-7 rasterizer patch-table index");
    }
    const auto offset = static_cast<std::size_t>(
        block.code_patch_offset[patch_index]);
    if (offset < 0x204U ||
        offset - 0x204U > code_size ||
        bytes_needed > code_size - (offset - 0x204U)) {
        throw std::runtime_error(
            "type-7 rasterizer patch is outside generated code");
    }
    return offset - 0x204U;
}

void modify(
    Type7PatchedRasterizer& out,
    std::size_t offset,
    std::vector<std::uint8_t> replacement) {
    if (offset > out.code.size() ||
        replacement.size() > out.code.size() - offset) {
        throw std::out_of_range(
            "type-7 generated-code mutation is out of bounds");
    }
    Type7CodeMutation journal{};
    journal.code_offset = offset;
    journal.replacement = std::move(replacement);
    journal.previous.reserve(
        journal.replacement.size());
    for (std::size_t i = 0;
         i < journal.replacement.size();
         ++i) {
        journal.previous.push_back(
            out.code[offset + i]);
        out.code[offset + i] =
            journal.replacement[i];
    }
    out.modifications.push_back(
        std::move(journal));
}

} // namespace

Type7PatchedRasterizer Type7RasterizerPatchModel::stage(
    const RendererScaleBlock& block,
    const Type7RasterizerSetup& setup) {
    Type7PatchedRasterizer out{};
    out.code = block.rasterizer_x86;
    if (!setup.drawable) {
        return out;
    }

    if (!setup.nonzero_mode) {
        if (setup.patch_from > 128U) {
            throw std::out_of_range(
                "type-7 RET patch start exceeds 128");
        }
        // CODE.2 36C10 writes 0xC3 at every patch entry from the
        // 36B9C-selected position to index 127.
        for (std::size_t i = setup.patch_from;
             i < 128U;
             ++i) {
            const auto offset = code_offset(
                block, i, out.code.size(), 1U);
            modify(out, offset, {0xC3U});
        }
        return out;
    }

    if (setup.patch_before > 128U) {
        throw std::out_of_range(
            "type-7 short-jump patch count exceeds 128");
    }
    // CODE.2 36C44 patches the first [0, count) scale segments. It
    // deliberately truncates (next-current-2) to the low 8 bits when
    // emitting EB xx, even if the encoded displacement is unusual.
    for (std::size_t i = 0U;
         i < setup.patch_before;
         ++i) {
        const auto offset = code_offset(
            block, i, out.code.size(), 2U);
        const auto next = code_offset(
            block, i + 1U, out.code.size(), 0U);
        const auto displacement =
            static_cast<std::uint8_t>(
                next - offset - 2U);
        modify(
            out,
            offset,
            {0xEBU, displacement});
    }
    return out;
}

void Type7RasterizerPatchModel::restore(
    Type7PatchedRasterizer& rasterizer) {
    // 36C90/36CBC restore the patch journal in reverse, including
    // offset aliases and partially overlapping 1/2-byte mutations.
    for (auto it = rasterizer.modifications.rbegin();
         it != rasterizer.modifications.rend();
         ++it) {
        if (it->code_offset > rasterizer.code.size() ||
            it->previous.size() >
                rasterizer.code.size() -
                    it->code_offset) {
            throw std::runtime_error(
                "type-7 rasterizer restoration leaves code buffer");
        }
        for (std::size_t i = 0U;
             i < it->previous.size();
             ++i) {
            rasterizer.code[
                it->code_offset + i] =
                it->previous[i];
        }
    }
    rasterizer.modifications.clear();
}

} // namespace rloft
