#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/renderer_tables.hpp"
#include "rloft/type7_rasterizer_setup.hpp"

namespace rloft {

// Temporary code mutation performed by CODE.2 36C10/36C44.
// One-byte RET patches correspond to mode 0; two-byte EB displacement
// patches correspond to a nonzero render mode. Overlapping patch offsets
// are preserved in their original order and must be restored in reverse.
struct Type7CodeMutation final {
    std::size_t code_offset{};
    std::vector<std::uint8_t> previous;
    std::vector<std::uint8_t> replacement;
};

struct Type7PatchedRasterizer final {
    std::vector<std::uint8_t> code;
    std::vector<Type7CodeMutation> modifications;
};

class Type7RasterizerPatchModel final {
public:
    // Makes a private copy of the supplied SINTAB generated-code bytes,
    // applies 36C10 or 36C44 exactly, and retains a restoration journal.
    // No executable-memory permissions, host function pointers, or changes
    // to the canonical RendererScaleBlock are required.
    [[nodiscard]] static Type7PatchedRasterizer stage(
        const RendererScaleBlock& block,
        const Type7RasterizerSetup& setup);

    // Corresponds to 36C90 / 36CBC restoration in reverse patch order.
    static void restore(Type7PatchedRasterizer& rasterizer);
};

} // namespace rloft
