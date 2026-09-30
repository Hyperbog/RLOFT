#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

namespace ravenloft::game {

// Fidelity-audited reconstruction of CODE.1 0x004C..0x00EA.
//
// Original call contract:
//   arg +08: caller-owned output buffer
//   arg +0C: signed 32-bit record index
//   DS:04C8: global text-file handle
//
// The routine performs its seeks/reads in the original order and decodes the
// payload in place.  The FileOps boundary represents CODE.1 0x633C (seek) and
// 0x6304 (read); it is not a replacement for the routine's file semantics.
class TextRecord004C {
public:
    struct FileOps {
        virtual ~FileOps() = default;
        virtual void seek(std::uint32_t offset,
                          std::int32_t origin,
                          std::int32_t handle) = 0;
        virtual void read(void* destination,
                          std::uint32_t byteCount,
                          std::int32_t handle) = 0;
    };

    static void decodeTo(char* output,
                         std::int32_t recordIndex,
                         std::int32_t textFileHandle,
                         FileOps& io);

    // Convenience adapter for tooling/tests that already hold the complete
    // encoded file in memory.  This is not the original 0x004C call contract.
    static std::string decode(std::span<const std::uint8_t> textFile,
                              std::int32_t recordIndex);
};

} // namespace ravenloft::game
