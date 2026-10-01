#pragma once

#include "UiTextComposite00EC.h"

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterPanelCacheState1A04 {
    // DS:095C / DS:0960 are the two cached handles owned by this routine.
    std::uintptr_t handle95C = 0;
    std::uintptr_t handle960 = 0;

    // DS:7138 is DWORD storage.  0x1A4A deliberately reads only its low byte
    // before passing the zero-extended value to 0x6588.
    std::uint32_t buffer7138 = 0;

    // Original byte flags at DS:04C4 and DS:04C5.
    std::uint8_t flag4C4 = 0;
    std::uint8_t flag4C5 = 0;

    // Original in-place records at DS:67F8 and DS:6A40.  They are passed
    // directly to 0x7D80; no C++ text/database abstraction is involved.
    const char* record67F8 = nullptr;
    std::int8_t field680A = 0;
    std::uint8_t field680B = 0;
    const char* record6A40 = nullptr;
    std::int8_t field6A52 = 0;
    std::uint8_t field6A53 = 0;

    std::int32_t global713C = 0;
};

struct CharacterPanelCacheOps1A04 {
    UiTextDrawOps00EC draw;

    std::function<void(std::uintptr_t)> call7830;
    std::function<void(std::uint32_t)> call6588;
    std::function<std::uintptr_t(std::int32_t, std::int32_t,
                                 std::int32_t, std::int32_t,
                                 std::int32_t)> call7868;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::uintptr_t)> call67C8;

    // Exact CODE.1 calls used by the flag==0 branches.  0x004C decodes
    // record zero into the caller-owned DS:55F8 buffer, then 0x00EC consumes
    // that buffer.  Keeping these calls explicit preserves their ordering.
    std::function<void(char*, std::int32_t)> call004C;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, const char*, std::int32_t,
                       std::int32_t)> call00EC;
    char* textBuffer55F8 = nullptr;
};

class CharacterPanelCache1A04 {
public:
    // Direct-binary-verified reconstruction of CODE.1 0x1A04..0x1BD3.
    // 0x1BD1..0x1BD3 is alignment padding (LEA EAX,[EAX+0]).
    static void rebuild(CharacterPanelCacheState1A04& state,
                        const CharacterPanelCacheOps1A04& ops);
};

} // namespace ravenloft::game
