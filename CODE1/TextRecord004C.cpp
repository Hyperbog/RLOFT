#include "TextRecord004C.h"

#include "Binary.h"

#include <cstring>
#include <stdexcept>
#include <vector>

namespace ravenloft::game {

void TextRecord004C::decodeTo(char* output,
                              std::int32_t recordIndex,
                              std::int32_t textFileHandle,
                              FileOps& io) {
    // 0x0052..0x006D
    // LEA EAX,[recordIndex*4]; ADD EAX,4; seek(EAX,0,DS:04C8).
    const std::uint32_t tableOffset =
        static_cast<std::uint32_t>(recordIndex) * 4u + 4u;
    io.seek(tableOffset, 0, textFileHandle);

    // 0x0070..0x0081: read exactly four bytes into [EBP-8].
    std::uint32_t recordOffset = 0;
    io.read(&recordOffset, 4u, textFileHandle);

    // 0x0084..0x0095: seek to the record offset.
    io.seek(recordOffset, 0, textFileHandle);

    // 0x0098..0x00A9: read two bytes into the low word of [EBP-4].
    // Only that low word controls the externally observable loop/pointers.
    std::uint16_t payloadLength = 0;
    io.read(&payloadLength, 2u, textFileHandle);

    // 0x00AC..0x00C0: read the encoded payload directly into caller memory.
    io.read(output, static_cast<std::uint32_t>(payloadLength), textFileHandle);

    // 0x00C3..0x00CA: append the NUL byte at output[payloadLength].
    output[payloadLength] = '\0';

    // 0x00CD..0x00E5: CMP uses the low word, address calculation zero-extends
    // that word, XOR touches one byte, and the original DEC then repeats.
    while (payloadLength != 0u) {
        output[static_cast<std::uint16_t>(payloadLength - 1u)] =
            static_cast<char>(
                static_cast<std::uint8_t>(
                    output[static_cast<std::uint16_t>(payloadLength - 1u)]) ^
                0xDCu);
        payloadLength = static_cast<std::uint16_t>(payloadLength - 1u);
    }
}

std::string TextRecord004C::decode(std::span<const std::uint8_t> textFile,
                                   std::int32_t recordIndex) {
    // Tooling adapter.  Keep the faithful routine above as the authoritative
    // reconstruction; this helper only supplies an in-memory FileOps backend.
    class MemoryFileOps final : public FileOps {
    public:
        explicit MemoryFileOps(std::span<const std::uint8_t> bytes)
            : bytes_(bytes) {}

        void seek(std::uint32_t offset,
                  std::int32_t origin,
                  std::int32_t) override {
            if (origin != 0) {
                throw std::runtime_error("TextRecord004C adapter: unsupported seek origin");
            }
            Binary::requireRange(bytes_, offset, 0u,
                                 "CODE.1 0x004C seek");
            position_ = offset;
        }

        void read(void* destination,
                  std::uint32_t byteCount,
                  std::int32_t) override {
            Binary::requireRange(bytes_, position_, byteCount,
                                 "CODE.1 0x004C read");
            std::memcpy(destination, bytes_.data() + position_, byteCount);
            position_ += byteCount;
        }

    private:
        std::span<const std::uint8_t> bytes_;
        std::size_t position_ = 0;
    } io(textFile);

    const std::uint32_t tableOffset =
        static_cast<std::uint32_t>(recordIndex) * 4u + 4u;
    const std::uint32_t recordOffset = Binary::u32(textFile, tableOffset);
    const std::uint16_t payloadLength = Binary::u16(textFile, recordOffset);
    std::vector<char> output(static_cast<std::size_t>(payloadLength) + 1u);
    decodeTo(output.data(), recordIndex, 0, io);
    return std::string(output.data(), payloadLength);
}

} // namespace ravenloft::game
