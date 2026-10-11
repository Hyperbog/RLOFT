#pragma once
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft_recomp {
struct OriginalLeObject {
    std::uint32_t virtual_size=0, base=0, flags=0;
    std::uint32_t first_page=0, page_count=0;
    std::vector<std::uint8_t> raw;       // Original LE bytes + BSS; DGROUP-relative for canonical source.
    std::vector<std::uint8_t> relocated; // Original loader's flat-address 32-bit fixup projection.
};
struct OriginalLeImage {
    std::uint32_t entry_object=0, entry_offset=0, stack_object=0, stack_offset=0;
    std::uint32_t page_size=0, applied_fixups=0, backward_page_fixups=0;
    std::vector<OriginalLeObject> objects;
    // Reads the supplied original bound DOS/4GW LE. Fail closed for unsupported records.
    static OriginalLeImage parse(const std::vector<std::uint8_t>& file);
};
} // namespace rloft_recomp
