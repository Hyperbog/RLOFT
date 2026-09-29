#include "TextRecord004C.h"

#include "Binary.h"

namespace ravenloft::game {

std::string TextRecord004C::decode(std::span<const std::uint8_t> textFile,
                                        std::int32_t recordIndex) {
    // 0x005A..0x0068: byte offset = recordIndex * 4 + 4.
    const auto tableOffsetBits =
        static_cast<std::uint32_t>(recordIndex) * 4u + 4u;
    const auto tableOffset = static_cast<std::size_t>(tableOffsetBits);

    // 0x0070..0x0095: read the 32-bit record-file offset and seek to it.
    const std::uint32_t recordOffset = Binary::u32(textFile, tableOffset);

    // 0x0098..0x00A9: read the 16-bit encoded payload length.
    const std::uint16_t payloadLength = Binary::u16(textFile, recordOffset);
    const std::size_t payloadOffset = static_cast<std::size_t>(recordOffset) + 2u;
    Binary::requireRange(textFile, payloadOffset, payloadLength,
                                  "CODE.1 0x004C text record");

    // 0x00AC..0x00CA: read payload and append the NUL terminator. std::string
    // stores the payload length explicitly, so the terminator is implicit at
    // c_str()[size()] rather than being part of the logical result.
    std::string decoded(payloadLength, '\0');
    for (std::size_t i = 0; i < payloadLength; ++i) {
        decoded[i] = static_cast<char>(textFile[payloadOffset + i]);
    }

    // 0x00CD..0x00E5: original code walked backwards and XORed each byte.
    for (std::size_t remaining = payloadLength; remaining != 0; --remaining) {
        const std::size_t i = remaining - 1u;
        decoded[i] = static_cast<char>(
            static_cast<std::uint8_t>(decoded[i]) ^ 0xDCu);
    }

    return decoded;
}

} // namespace ravenloft::game
