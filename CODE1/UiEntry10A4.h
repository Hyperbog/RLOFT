#pragma once

#include "UiTextComposite00EC.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

// 0x954C returns EAX as a pointer to the original resource object.  The
// animation reads WORD +4 and BYTE +6 from that object and passes the pointer
// itself to 0xA5C8/0x6D54.
struct ResourceView10A4 {
    std::uintptr_t pointer = 0;
    std::uint16_t word4 = 0;
    std::uint8_t byte6 = 0;
};

struct UiEntryState10A4 {
    std::int32_t mode4D0 = 0;
    std::array<std::int32_t, 9> x87C{};
    std::array<std::int32_t, 9> y880{};
    std::array<std::int32_t, 9> current8F8{};
    std::array<std::uint8_t, 12> animation964{};
    std::array<std::uint8_t, 12> flag970{};

    // DS:7138 is dword storage.  0x10A4 deliberately mixes MOVZX BYTE and
    // PUSH/MOV DWORD accesses, while XOR changes only its low byte.
    std::uint32_t buffer7138 = 0;
    const char* text7140 = "";
};

struct UiEntryOps10A4 {
    UiTextDrawOps00EC draw;

    std::function<std::uintptr_t(std::size_t)> call71C8;
    std::function<void(std::uintptr_t)> call7240;
    std::function<void(std::uint32_t)> call6588;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call86C4;
    std::function<void(std::int32_t)> call04B0;
    std::function<void(std::uint32_t)> call53E4;
    std::function<ResourceView10A4(std::int32_t, std::int32_t)> call954C;
    std::function<void(std::int32_t, std::uintptr_t)> callA5C8;
    std::function<void(std::uintptr_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call66C0;
    std::function<void(std::uintptr_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call6744;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::uintptr_t)> call6D54;
};

class UiEntry10A4 {
public:
    // Fidelity-audited reconstruction of CODE.1 0x10A4..0x1463.
    static void animate(std::int32_t characterPanelStage,
                        UiEntryState10A4& state,
                        const UiEntryOps10A4& ops);
};

} // namespace ravenloft::game
