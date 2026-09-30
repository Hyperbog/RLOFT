#pragma once
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct SceneRenderState139C {
    std::int16_t width12756 = 0;
    std::int16_t height12754 = 0;
    bool drawText5FD9 = false;
    bool buffer6058Ready = false;
    std::uintptr_t value1274C = 0;
    void* buffer6058 = nullptr;
};

struct SceneRenderOps139C {
    std::function<void()> call11A8;
    std::function<void(std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call0B6C;
    std::function<void(std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call0CB0;
    std::function<void(std::int32_t,std::int32_t)> call1228;
    std::function<void(void*,std::uintptr_t)> call1C368;
    std::function<void(std::int32_t,std::int32_t,std::int32_t,void*,std::int32_t)> call30508;
    std::function<void()> call10C4;
};

class SceneRender139C {
public:
    // Direct reconstruction of CODE.2 0x139C..0x1556.
    static void run(const SceneRenderState139C& state,
                    const SceneRenderOps139C& ops,
                    std::int32_t startX,
                    std::int32_t startY,
                    std::int32_t spanX,
                    std::int32_t spanY,
                    std::int32_t overlays);
};

} // namespace ravenloft::game
