#include "rloft_native/native_assets.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>
#include <utility>

namespace rloft_native {
namespace {
std::int16_t signed_word(const std::uint8_t* bytes) {
    const auto word = static_cast<std::uint16_t>(bytes[0]) |
                      static_cast<std::uint16_t>(
                          static_cast<std::uint16_t>(bytes[1]) << 8U);
    return static_cast<std::int16_t>(word);
}

// Original CODE.2 4 KiB LZSS history starts at 0xFEE. Only 0..0xFED
// is initialized to FE; an unset read elsewhere is corrupt data.
std::vector<std::uint8_t> uncompress_original_lzss(
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
                throw std::runtime_error("original EH1 LZSS flags truncated");
            flags = static_cast<std::uint32_t>(compressed[input++]) | 0xFF00U;
        }
        if (flags & 1U) {
            if (input == length)
                throw std::runtime_error("original EH1 LZSS literal truncated");
            emit(compressed[input++]);
        } else {
            if (length - input < 2)
                throw std::runtime_error("original EH1 LZSS reference truncated");
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
                        "original EH1 LZSS referenced uninitialized history");
                emit(static_cast<std::uint8_t>(history[index]));
            }
        }
    }
    return decoded;
}
} // namespace

NativeType1Raster decode_native_type1_raster(
    const std::vector<std::uint8_t>& entry) {
    if (entry.size() < 13 || entry[0] != 'E' ||
        entry[1] != 'H' || entry[2] != 1)
        throw std::runtime_error("original RES record is not EH type-1");

    const int header_bytes = signed_word(entry.data() + 3);
    if (header_bytes < 8 ||
        static_cast<std::size_t>(header_bytes) > entry.size() - 5)
        throw std::runtime_error("invalid original EH type-1 header length");

    const auto* descriptor = entry.data() + 5;
    const int frame_count = signed_word(descriptor);
    const int rows = signed_word(descriptor + 2);
    const int width = static_cast<int>(descriptor[4]) * 8;
    const std::uint8_t transparent = descriptor[5];
    const std::uint8_t compression = descriptor[6];
    const std::uint8_t transform = descriptor[7];

    if (frame_count <= 0 || rows <= 0 || width == 0)
        throw std::runtime_error("nonpositive original EH raster dimensions");

    // Limits serve untrusted-media safety, not an invented game asset size.
    constexpr std::uint64_t kMaxPixels = 32ULL * 1024ULL * 1024ULL;
    const auto pixels_needed = static_cast<std::uint64_t>(frame_count) *
                               static_cast<std::uint64_t>(rows) *
                               static_cast<std::uint64_t>(width);
    if (pixels_needed > kMaxPixels)
        throw std::runtime_error("original EH raster exceeds 32 Mi-pixel safety bound");

    const auto payload_offset = static_cast<std::size_t>(header_bytes) + 5;
    const auto* payload = entry.data() + payload_offset;
    const auto available = entry.size() - payload_offset;
    std::vector<std::uint8_t> pixels;

    if (compression == 0) {
        if (available != pixels_needed)
            throw std::runtime_error("raw original EH type-1 pixel count mismatch");
        pixels.assign(payload, payload + available);
    } else if (compression == 3) {
        pixels = uncompress_original_lzss(
            payload, available, static_cast<std::size_t>(pixels_needed));
    } else {
        throw std::runtime_error("unsupported original EH type-1 codec");
    }

    NativeType1Raster result;
    result.frames = static_cast<std::uint16_t>(frame_count);
    result.width = static_cast<std::uint16_t>(width);
    result.height = static_cast<std::uint16_t>(rows);
    result.transparent_index = transparent;
    result.pixels = std::move(pixels);

    // Original CODE.1 DC90 one-row descriptor transformation changes the
    // [count][one row] interpretation to [one image][count rows] without
    // changing decoded pixel sequence.
    if (transform != 0 && rows == 1) {
        result.frames = 1;
        result.height = static_cast<std::uint16_t>(frame_count);
    }
    return result;
}

NativeCursorResource load_native_default_cursor(const NativeMedia& source) {
    const auto init = source.read("INIT");
    if (init.size() < 2)
        throw std::runtime_error("original INIT is too short for cursor ID");
    const auto id = static_cast<std::size_t>(init[0]) |
                    (static_cast<std::size_t>(init[1]) << 8U);
    if (id > static_cast<std::size_t>(-1) - 0x2FU)
        throw std::runtime_error("cursor RES0 ID overflow");
    const auto res0_id = id + 0x2FU;
    NativeResArchive archive(source.read("RES0"));
    if (res0_id >= archive.entry_count())
        throw std::runtime_error("original INIT cursor RES0 ID out of range");
    NativeCursorResource output;
    output.res0_id = res0_id;
    output.image = decode_native_type1_raster(archive.entry(res0_id));
    return output;
}

} // namespace rloft_native
