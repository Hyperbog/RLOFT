#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct TextSelectState21D8 {
    std::span<std::uint8_t> records5760;
    std::int32_t viewX12744 = 0;
    std::int32_t viewY12740 = 0;
    std::int16_t mouseX9E31 = 0, mouseY9E33 = 0;
    std::int16_t exitLeft5F8A = 0, exitTop5F8C = 0;
    std::int16_t exitRight5F8E = 0, exitBottom5F90 = 0;
    std::uint8_t valueC050 = 0;
};

struct TextSelectOps21D8 {
    std::function<std::int32_t()> call1A1F4;
    std::function<std::int32_t(const std::uint8_t*,std::int32_t,std::int32_t)> call302CC;
    std::function<std::size_t()> call1C08;
    std::function<void(std::size_t,std::size_t,std::ptrdiff_t)> call30CD4;
    std::function<void(std::size_t,std::int32_t,std::size_t)> call30D30;
    std::function<void(std::int32_t,std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call139C;
    std::function<void(std::uint8_t)> call1A510;
    std::function<void()> call22D70;
};

class TextSelect21D8 {
public:
    // Direct reconstruction of CODE.2 0x21D8..0x2352.
    static void run(TextSelectState21D8& state,const TextSelectOps21D8& ops);
};

} // namespace ravenloft::game
