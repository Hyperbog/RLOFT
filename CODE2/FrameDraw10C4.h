#pragma once
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct FrameDrawState10C4 {
    const void* context36 = nullptr;
    const void* context3C = nullptr;
    const void* context3D = nullptr;
};

struct FrameDrawOps10C4 {
    std::function<void(std::int32_t,std::int32_t,std::int32_t,
                       const void*,std::int32_t)> call2975C;
};

class FrameDraw10C4 {
public:
    // Direct reconstruction of CODE.2 0x10C4..0x11A4.
    static void run(const FrameDrawState10C4& state,
                    const FrameDrawOps10C4& ops);
};

} // namespace ravenloft::game
