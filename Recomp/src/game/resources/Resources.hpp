#pragma once
// Dependency-free scene palette selection; usable without compiling CODE.2.
// Original CODE.2 scene-to-COLORS palette bank resolver.
// The immutable retail CODE.2 object 2 contains 3-byte scene descriptors
// at 0x81AC+3*level. The first byte selects a COLORS bank. The other
// descriptor bytes are not treated as colors or palette overrides.
//
// This is a pure presentation-side resolver: it intentionally does NOT write
// the shared original guest's 0xC034 state, which also controls the existing
// compiled 1F8FC/1F914 RES0 gameplay-frame base in this partial native port.
// Executing original scene setup and its full call order remains separate
// work; don't fabricate game state merely to select a render palette.
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft { namespace original_scene_palette {

constexpr std::size_t kSceneTableOffset = 0x81ACu;
constexpr unsigned kSceneDescriptorStride = 3u;
constexpr unsigned kOriginalSceneCount = 32u;
constexpr unsigned kOriginalColorsBanks = 15u;

unsigned select_bank(const std::vector<std::uint8_t>& original_dgroup,
                     unsigned level_id);

}} // namespace rloft::original_scene_palette
