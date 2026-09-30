#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct ProjectedPoint0BFC {
    std::int32_t x = 0;
    std::int32_t y = 0;
};

struct ProjectedDrawState0BFC {
    const void* drawContext = nullptr; // original DS:0x258DC + 0x34
    const void* projectionData = nullptr; // original DS:0xC658
};

struct ProjectedDrawOps0BFC {
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       const void*, std::int32_t)> call2975C;

    // Direct representation of 0x2DF2C. It writes the two DWORD deltas used
    // eight times by the original accumulation loop.
    std::function<void(std::int32_t&, std::int32_t&,
                       std::int32_t, const void*)> call2DF2C;

    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call29CEC;
};

class ProjectedDraw0BFC {
public:
    // Direct reconstruction of CODE.2 0x0BFC..0x0CAF.
    static void run(const ProjectedDrawState0BFC& state,
                    const ProjectedDrawOps0BFC& ops,
                    std::int32_t arg0,
                    std::int32_t arg1,
                    std::int32_t x,
                    std::int32_t y);
};

} // namespace ravenloft::game
