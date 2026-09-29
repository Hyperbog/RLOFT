#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

namespace ravenloft::game {

// Direct reconstruction of CODE.1 0x004C..0x00EA.
//
// The original function used the global text-file handle at DS:04C8. It:
//   1. sought to 4 + recordIndex * 4,
//   2. read a 32-bit record offset,
//   3. sought to that record,
//   4. read a 16-bit length,
//   5. read exactly that many bytes,
//   6. appended a NUL byte,
//   7. XOR-decoded every payload byte with 0xDC.
//
// This source-port form operates on the already loaded file bytes while
// preserving the shipped-file semantics exactly for valid input.
class Code1TextRecord004C {
public:
    static std::string decode(std::span<const std::uint8_t> textFile,
                              std::int32_t recordIndex);
};

} // namespace ravenloft::game
