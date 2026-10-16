#pragma once
#include <cstdint>
#include <optional>

namespace rloft {
// An additive Windows input adapter for the ORIGINAL four CODE.2 menu
// entries. It never changes their resource IDs, order, or jump targets.
// Pure logic: pointer hit testing and DOS supervisor state remain elsewhere.
struct TitleKeyboardResult final {
    int highlighted{-1};
    std::optional<int> activate;
};
class TitleKeyboardModel final {
public:
    [[nodiscard]] static TitleKeyboardResult key_down(
        std::uint32_t virtual_key,int current,bool repeated=false) noexcept {
        if(current<-1 || current>3)current=-1;
        switch(virtual_key) {
            case 0x26U: // VK_UP
                return {current<0?0:(current+3)%4,std::nullopt};
            case 0x28U: // VK_DOWN
                return {current<0?0:(current+1)%4,std::nullopt};
            case 0x24U: // VK_HOME
                return {0,std::nullopt};
            case 0x23U: // VK_END
                return {3,std::nullopt};
            case 0x0DU: // VK_RETURN
                return {current,(!repeated && current>=0)
                    ?std::optional<int>(current):std::nullopt};
            default:break;
        }
        // Direct 1..4 is intentionally a modern shortcut, and key auto-
        // repeats must not launch duplicate cinematics or game sessions.
        if(!repeated && virtual_key>=0x31U && virtual_key<=0x34U) {
            const int entry=static_cast<int>(virtual_key-0x31U);
            return {entry,entry};
        }
        if(!repeated && virtual_key>=0x61U && virtual_key<=0x64U) {
            const int entry=static_cast<int>(virtual_key-0x61U);
            return {entry,entry};
        }
        return {current,std::nullopt};
    }
};
} // namespace rloft
