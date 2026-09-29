#include "TextDatabase.h"

#include "Binary.h"
#include "TextRecord004C.h"

namespace ravenloft::game {

TextDatabase TextDatabase::decode(
    std::span<const std::uint8_t> encodedTextFile) {
    TextDatabase out;

    const std::uint32_t count = Binary::u32(encodedTextFile, 0);
    out.entries_.reserve(count);

    for (std::uint32_t i = 0; i < count; ++i) {
        out.entries_.push_back(
            TextRecord004C::decode(
                encodedTextFile,
                static_cast<std::int32_t>(i)));
    }

    return out;
}

} // namespace ravenloft::game
