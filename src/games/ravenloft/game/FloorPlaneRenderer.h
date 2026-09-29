#pragma once

#include "engine/render/IndexedFrameBuffer.h"
#include "games/ravenloft/data/GraphicsDecode.h"
#include "games/ravenloft/data/TrigTable.h"
#include <array>
#include <cstdint>

namespace ravenloft::game {

struct FloorPlaneStats {
    std::size_t rows=0;
    std::size_t pixels=0;
};

// Reimplementation of the textured lower-plane path built by CODE.2
// 0x34998 and consumed by 0x34A74.  The DOS routine interleaves four VGA
// planes; this version emits the same 256 logical pixels in ordinary row-major
// form while preserving the shipped SINTAB stepping and 64x64 texture wrap.
class FloorPlaneRenderer {
public:
    explicit FloorPlaneRenderer(const data::TrigTable& trig):trig_(trig){}

    FloorPlaneStats render(ssi::render::IndexedFrameBuffer& fb,
                           const data::IndexedImage& texture,
                           std::int32_t cameraXFixed,
                           std::int32_t cameraYFixed,
                           int headingDegrees,
                           const std::array<std::uint8_t,128>* shade=nullptr) const;

private:
    const data::TrigTable& trig_;
};

} // namespace ravenloft::game
