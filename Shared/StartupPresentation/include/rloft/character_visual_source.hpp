#pragma once
#include "rloft/startup_visual_source.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

namespace rloft {
// Neutral original CODE.1 character-art/data seam. Both Windows hosts
// obtain these bytes from their own read-only original-media loaders.
// This interface deliberately owns neither a party nor a save destination.
class CharacterVisualSource : public StartupVisualSource {
public:
    [[nodiscard]] virtual Type1Raster type1(int archive,int index) const=0;
    [[nodiscard]] virtual std::uint8_t code1_u8(std::size_t dgroup_offset) const=0;
    [[nodiscard]] virtual std::uint16_t code1_u16(std::size_t dgroup_offset) const=0;
    [[nodiscard]] virtual std::int32_t code1_s32(std::size_t dgroup_offset) const=0;
    [[nodiscard]] virtual std::string code1_cstring(std::size_t dgroup_offset) const=0;
};
} // namespace rloft
