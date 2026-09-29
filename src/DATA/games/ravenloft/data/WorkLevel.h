#pragma once
#include "games/ravenloft/data/LevelDatabase.h"
#include <array>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace ravenloft::data {

// Expanded 22-byte actor/object/scenery record used by the mutable LEVxxx.DAT
// work files. Field names are limited to semantics proven from CODE.2.
struct RuntimePlacement22 {
    std::array<std::uint8_t,22> raw{};
    std::int32_t yFixed = 0; // record +0x00
    std::int32_t xFixed = 0; // record +0x04
    std::uint32_t state = 0; // record +0x08; preserved until all bit semantics are needed
    std::uint16_t field0C = 0;
    std::uint16_t type = 0;  // record +0x0E
    std::uint16_t field10 = 0;
    std::int16_t hitPoints = 0; // record +0x12 for NPCs
    std::uint16_t field14 = 0;

    double x() const { return xFixed / 65536.0; }
    double y() const { return yFixed / 65536.0; }
};

struct WorkLevelSnapshot {
    std::uint8_t marker = 0;
    std::array<std::uint32_t,7> sections{};
    std::uint16_t width = 0, height = 0;
    std::int16_t ceilingPlane = -1, floorPlane = -1;
    std::vector<std::uint16_t> cells;
    std::vector<std::uint8_t> interactionRuntimeBytes;
    std::vector<RuntimePlacement22> npcs;
    std::vector<RuntimePlacement22> scenery;
    std::vector<RuntimePlacement22> objects;
    std::array<EventBlock,50> events{};
    AutomapInitialState automap;
};

class WorkLevel {
public:
    explicit WorkLevel(const std::filesystem::path& path);
    const WorkLevelSnapshot& snapshot() const { return snapshot_; }
private:
    WorkLevelSnapshot snapshot_{};
};

} // namespace ravenloft::data
