#pragma once
#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
namespace ravenloft::data {
struct NpcDefinition { std::array<std::uint8_t,69> raw{}; std::string label; std::uint16_t baseSprite=0; std::uint16_t collisionHalfExtent=0; std::uint8_t facingStride=0, viewCount=0, actionGroup=0, light=0, reactionGroup=0, hitDie=0, damageDice=0, damageDie=0, damageBonus=0; std::int8_t armorClass=0; std::uint8_t thac0=0; std::uint16_t xp=0; std::uint8_t traits39=0,traits3A=0; std::uint16_t ambientSound=0,attackSound=0,hitSound=0,deathSound=0; };
struct ObjectDefinition { std::array<std::uint8_t,65> raw{}; std::string label; };
class NpcDatabase { public: explicit NpcDatabase(const std::filesystem::path& path);std::size_t count()const{return records_.size();}const NpcDefinition& at(std::size_t i)const{return records_.at(i);}private:std::vector<NpcDefinition> records_;};
class ObjectDatabase { public: explicit ObjectDatabase(const std::filesystem::path& path);std::size_t count()const{return records_.size();}const ObjectDefinition& at(std::size_t i)const{return records_.at(i);}private:std::vector<ObjectDefinition> records_;};
}
