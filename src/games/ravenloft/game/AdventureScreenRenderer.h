#pragma once
#include "engine/render/IndexedFrameBuffer.h"
#include "games/ravenloft/data/InitDatabase.h"
#include "games/ravenloft/data/ResourceStore.h"
#include <cstdint>

namespace ravenloft::game {

struct AdventureScreenStats {
    std::uint16_t primaryResource=0;
    std::uint16_t secondaryResource=0;
    std::uint32_t worldPixels=0;
};

// Static adventure-screen composition recovered for the opening level.
// RES0 theme resources provide the original 320x200 frame; the original
// world aperture is x=32..287, y=4..131 (256x128).
class AdventureScreenRenderer {
public:
    AdventureScreenRenderer(const data::InitDatabase& init,
                            const data::ResourceStore& resources);

    AdventureScreenStats renderOpening(
        ssi::render::IndexedFrameBuffer& screen320x200,
        const ssi::render::IndexedFrameBuffer& world256x256,
        std::uint8_t level) const;

private:
    const data::InitDatabase& init_;
    const data::ResourceStore& resources_;
};

} // namespace ravenloft::game
