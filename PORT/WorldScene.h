#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/InitDatabase.h"
#include "games/ravenloft/data/LevelDatabase.h"
#include "games/ravenloft/data/WorkLevel.h"
#include <cstdint>
#include <optional>
#include <vector>

namespace ravenloft::game {

struct WallMaterialRef {
    std::uint16_t slot=0;
    std::uint16_t xLower=0, xUpper=0, yLower=0, yUpper=0;
    bool upperPass=false;
    bool noDrawXLower=false, noDrawYLower=false;
};

struct SceneryRenderRef {
    std::uint16_t type=0;
    double x=0.0,y=0.0;
    std::int32_t xFixed=0,yFixed=0;
    std::uint8_t placementFlags=0;
    std::uint16_t placementField14=0; // runtime orientation/state word; pristine flags are flags<<3
    std::uint16_t baseResource=0;
    std::uint16_t collisionExtent=0;
    std::uint8_t viewStride=0;
    std::uint8_t animationFrames=0;
    std::uint8_t effect=0;
    bool hasPairedUpper=false;
    std::uint16_t pairedUpperResource=0;
    std::uint8_t pairedVerticalOffsetRows=0;
};

struct NpcRenderRef {
    std::uint16_t type=0;
    double x=0.0,y=0.0;
    std::int32_t xFixed=0,yFixed=0;
    std::uint16_t placementField14=0;
    std::uint16_t baseResource=0;
    std::uint16_t collisionExtent=0;
    std::uint8_t facingStride=0;
    std::uint8_t viewCount=0;
};

// Port-facing, renderer-neutral interpretation of one Ravenloft level.  This class
// intentionally stops at facts proved by the decomp: it identifies the exact
// resource banks/roles and entity source records without inventing camera scale, perspective constants, or special geometry.
class WorldScene {
public:
    WorldScene(const data::LevelRecord& level,
               const data::InitDatabase& init,
               const data::NpcDatabase& npcs,
               const std::optional<data::RuntimePlacement22>& runtimeAssassin=std::nullopt);

    std::uint16_t width() const { return width_; }
    std::uint16_t height() const { return height_; }
    std::optional<std::uint16_t> floorResource() const { return floorResource_; }
    std::uint8_t paletteIndex() const { return paletteIndex_; }
    std::uint8_t skySelector() const { return skySelector_; }
    std::uint8_t ceilingFallbackColor() const { return ceilingFallbackColor_; }
    std::uint8_t floorFallbackColor() const { return floorFallbackColor_; }
    std::uint8_t upperSourceRows() const { return upperSourceRows_; }
    std::optional<std::uint16_t> ceilingResource() const { return ceilingResource_; }
    const std::vector<std::uint16_t>& cells() const { return cells_; }
    const std::vector<WallMaterialRef>& wallMaterials() const { return walls_; }
    const std::vector<SceneryRenderRef>& scenery() const { return scenery_; }
    const std::vector<NpcRenderRef>& npcs() const { return npcs_; }

private:
    std::uint16_t width_=0,height_=0;
    std::uint8_t paletteIndex_=0,skySelector_=0xFF,ceilingFallbackColor_=0,floorFallbackColor_=0,upperSourceRows_=0;
    std::optional<std::uint16_t> floorResource_,ceilingResource_;
    std::vector<std::uint16_t> cells_;
    std::vector<WallMaterialRef> walls_;
    std::vector<SceneryRenderRef> scenery_;
    std::vector<NpcRenderRef> npcs_;
};

} // namespace ravenloft::game
