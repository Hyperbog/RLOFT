#pragma once
#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct InterfaceDrawState1834 {
    std::uint8_t valueC050 = 0;
    std::uint8_t value258A8 = 0;
    std::uint8_t value5FD9 = 0;
    std::span<const std::uint8_t> table5FCC;
    void* scratch769C = nullptr;
    std::uint32_t value12748 = 0;
    std::uint32_t value12750 = 0;
};

struct InterfaceDrawOps1834 {
    std::function<const void*(std::int32_t)> contextAt258DC;
    std::function<void(std::int32_t)> call291B0;
    std::function<void(std::int32_t,std::int32_t,std::int32_t,
                       const void*,std::int32_t)> call2975C;
    std::function<std::uint32_t()> call30700;
    std::function<void(std::uint16_t)> call290D0;
    std::function<std::uint32_t(std::int32_t,std::int32_t,
                                const void*,std::int32_t)> call30CA2;
    std::function<void(void*,std::int32_t)> call1C368;
    std::function<void(std::int32_t,std::int32_t,void*,std::int32_t)> call3060C;
};

class InterfaceDraw1834 {
public:
    // Direct reconstruction of CODE.2 0x1834..0x1C07.
    static void run(InterfaceDrawState1834& state,
                    const InterfaceDrawOps1834& ops);
};

} // namespace ravenloft::game
