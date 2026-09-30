#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct TextRecords1C08State {
    std::span<std::uint8_t> records5760;
};

struct TextRecords1C08Ops {
    std::function<std::int32_t()> call3058C;
    std::function<std::int32_t()> call30594;
    std::function<void(std::int32_t,std::int32_t)> call304F4;
    std::function<void(std::int32_t,std::int32_t)> call30354;
    std::function<std::int32_t(std::int32_t,std::int32_t)> call30318;
};

class TextRecords1C08 {
public:
    // CODE.2 0x1C08..0x1C27.
    static std::size_t endOffset(const TextRecords1C08State& state);

    // CODE.2 0x1C28..0x1C4C.
    static std::size_t usedBytes(const TextRecords1C08State& state);

    // CODE.2 0x1C50..0x1CA6.
    static void draw(const TextRecords1C08Ops& ops,
                     const std::uint8_t* text,
                     std::int32_t arg1,
                     std::int32_t count);

    // CODE.2 0x1CA8..0x1CE1.
    static std::int32_t width(const TextRecords1C08Ops& ops,
                              const std::uint8_t* text,
                              std::int32_t arg1);
};

} // namespace ravenloft::game
