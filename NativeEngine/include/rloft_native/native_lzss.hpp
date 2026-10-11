#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft_native {
// Shared independently implemented DreamForge 4KiB LZSS-4K decoder used
// by native EH1 rasters and EH6/EH7 texture/geometry payloads.
[[nodiscard]] std::vector<std::uint8_t> native_lzss4k(
    const std::uint8_t* input, std::size_t input_size, std::size_t decoded_size);
}
