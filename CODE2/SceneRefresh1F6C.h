#pragma once
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct SceneRefreshState1F6C {
    std::uint8_t valueC050 = 0;
    std::uint16_t value12750 = 0;
    std::int32_t value12740 = 0;
    std::int32_t value12744 = 0;
};

struct SceneRefreshOps1F6C {
    std::function<const void*(std::int32_t)> contextAt258DC;
    std::function<void(std::uint8_t)> call291B0;
    std::function<void(std::uint16_t)> call290D0;
    std::function<std::uint32_t(std::int32_t,std::int32_t,const void*,std::int32_t)> call30CA2;
    std::function<void(std::int32_t,std::int32_t,std::int32_t,const void*,std::int32_t)> call2975C;
    std::function<void(std::int32_t,std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call139C;
    std::function<void(std::uint32_t)> call1A510;
};

class SceneRefresh1F6C {
public:
    // Direct reconstruction of CODE.2 0x1F6C..0x2078.
    static void run(SceneRefreshState1F6C& state,
                    const SceneRefreshOps1F6C& ops,
                    std::int32_t overlayFlag);
};

} // namespace ravenloft::game
