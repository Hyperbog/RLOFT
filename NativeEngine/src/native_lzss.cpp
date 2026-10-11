#include "rloft_native/native_lzss.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <utility>

namespace rloft_native {
NativeLzssResult native_lzss4k_decode(
    const std::uint8_t* compressed, std::size_t length,
    std::size_t wanted) {
    std::array<int, 4096> history{};
    history.fill(-1);
    for (std::size_t i = 0; i < 0xFEE; ++i)
        history[i] = 0xFE;

    std::vector<std::uint8_t> decoded;
    decoded.reserve(wanted);
    std::size_t input = 0, write = 0xFEE;
    std::uint32_t flags = 0;

    auto emit = [&](std::uint8_t pixel) {
        decoded.push_back(pixel);
        history[write] = pixel;
        write = (write + 1) & 0xFFF;
    };

    while (decoded.size() != wanted) {
        flags >>= 1U;
        if ((flags & 0x100U) == 0) {
            if (input == length)
                throw std::runtime_error("original LZSS flags truncated");
            flags = static_cast<std::uint32_t>(compressed[input++]) | 0xFF00U;
        }
        if (flags & 1U) {
            if (input == length)
                throw std::runtime_error("original LZSS literal truncated");
            emit(compressed[input++]);
        } else {
            if (length - input < 2)
                throw std::runtime_error("original LZSS reference truncated");
            const auto lower = compressed[input++];
            const auto upper = compressed[input++];
            const std::size_t offset =
                static_cast<std::size_t>(lower) |
                ((static_cast<std::size_t>(upper) & 0xF0U) << 4U);
            const std::size_t count = (upper & 0x0FU) + 3U;
            for (std::size_t j = 0; j < count && decoded.size() < wanted; ++j) {
                const auto index = (offset + j) & 0xFFFU;
                if (history[index] < 0)
                    throw std::runtime_error(
                        "original LZSS referenced uninitialized history");
                emit(static_cast<std::uint8_t>(history[index]));
            }
        }
    }
    return {std::move(decoded),input};
}

std::vector<std::uint8_t> native_lzss4k(
    const std::uint8_t* compressed, std::size_t length, std::size_t wanted) {
    return native_lzss4k_decode(compressed,length,wanted).bytes;
}
} // namespace rloft_native
