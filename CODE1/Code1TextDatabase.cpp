#include "Code1TextDatabase.h"

#include "Code1Binary.h"
#include "Code1TextRecord004C.h"

namespace ravenloft::game {

Code1TextDatabase Code1TextDatabase::decode(
    std::span<const std::uint8_t> encodedTextFile) {
    Code1TextDatabase out;

    const std::uint32_t count = Code1Binary::u32(encodedTextFile, 0);
    out.entries_.reserve(count);

    for (std::uint32_t i = 0; i < count; ++i) {
        out.entries_.push_back(
            Code1TextRecord004C::decode(
                encodedTextFile,
                static_cast<std::int32_t>(i)));
    }

    return out;
}

} // namespace ravenloft::game
