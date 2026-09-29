#pragma once
#include <array>
#include <cstdint>
#include <filesystem>
#include <vector>
namespace ravenloft::data {
struct SceneryDefinition { std::uint16_t base=0,radius=0; std::uint8_t views=0,frames=0,effect=0; };
struct TransientDefinition { std::array<std::uint8_t,7> raw{}; };
struct SkyElementDefinition {
    std::int16_t resource=0;
    std::int16_t angle=0;
    std::int16_t width=0;
    std::int16_t vertical=0;
};
struct SkyGroupDefinition {
    std::int16_t resourceBase=0;
    std::vector<SkyElementDefinition> elements;
};
class InitDatabase {
public: explicit InitDatabase(const std::filesystem::path& path);
const std::array<std::uint16_t,18>& globals()const{return globals_;}
const std::vector<SceneryDefinition>& scenery()const{return scenery_;}
std::uint16_t interfaceResourceBase() const { return globals_[0]; }
std::uint16_t sceneryResourceBase() const { return globals_[4]; }
// CODE.2 0x16DD8 seeds the render-category base table DS:0x27100
// from INIT global[3].  CODE.2 0x2B6D0 builds NPC records in category 0,
// so NPCS.DAT +0x14 is already relative to a zero resource base.
std::uint16_t npcResourceBase() const { return globals_[3]; }
// Render category 2 is INIT global[5] (DS:0x27104); this is the
// ordinary world-object sprite namespace.
std::uint16_t objectResourceBase() const { return globals_[5]; }
std::uint16_t flatResourceBase() const { return globals_[7]; }
const std::vector<SkyGroupDefinition>& skyGroups()const{return skyGroups_;}
private: std::array<std::uint16_t,18> globals_{};std::vector<SceneryDefinition> scenery_;std::vector<TransientDefinition> transient_;std::vector<SkyGroupDefinition> skyGroups_;
};
}
