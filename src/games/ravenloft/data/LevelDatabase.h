#pragma once
#include <array>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace ravenloft::data {
struct FixedPointCoord { std::uint16_t integer=0, fraction=0; double value() const { return integer + fraction/65536.0; } };
struct Placement { std::uint16_t type=0; FixedPointCoord x{},y{}; std::uint8_t flags=0; std::uint32_t recordOffset=0; };
struct Interaction { std::uint8_t type=0; std::array<std::uint8_t,10> raw{}; std::uint16_t inputColumn=0; std::uint32_t recordOffset=0; };
struct EventInstruction { std::uint8_t opcode=0; std::array<std::uint8_t,4> operands{}; };
struct EventBlock { std::array<EventInstruction,5> instructions{}; std::uint8_t tail=0; bool nonEmpty=false; };
struct AutomapInitialState { std::uint16_t width=0,height=0; std::vector<std::uint8_t> cells; std::vector<std::uint8_t> annotations; };
struct LevelRecord {
    std::uint32_t index=0;
    std::uint16_t width=0,height=0;
    std::array<std::uint32_t,7> sections{};
    std::array<std::array<std::uint16_t,64>,4> wallBanks{};
    std::int16_t ceilingPlane=-1, floorPlane=-1;
    std::uint8_t upperSourceRows=0, skySelector=0xFF, ceilingFallbackColor=0, floorFallbackColor=0, paletteIndex=0;
    std::vector<std::uint16_t> cells;
    std::vector<Interaction> interactions;
    std::vector<Placement> npcs, scenery, objects;
    std::array<EventBlock,50> events{};
    AutomapInitialState automap;
};
class LevelDatabase {
public:
    explicit LevelDatabase(const std::filesystem::path& path);
    std::size_t count() const { return offsets_.size(); }
    LevelRecord load(std::size_t index) const;
private:
    std::vector<std::uint8_t> bytes_;
    std::vector<std::uint32_t> offsets_;
};
}
