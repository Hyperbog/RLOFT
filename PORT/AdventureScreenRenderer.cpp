#include "games/ravenloft/game/AdventureScreenRenderer.h"
#include "games/ravenloft/game/AdventureInterfaceTheme.h"
#include <stdexcept>

namespace ravenloft::game {

AdventureScreenRenderer::AdventureScreenRenderer(const data::InitDatabase& init,
                                                 const data::ResourceStore& resources)
    :init_(init),resources_(resources){}

AdventureScreenStats AdventureScreenRenderer::renderOpening(
    ssi::render::IndexedFrameBuffer& screen,
    const ssi::render::IndexedFrameBuffer& world,
    std::uint8_t level) const {
    if(screen.width()!=320 || screen.height()!=200)
        throw std::runtime_error("Ravenloft adventure screen requires 320x200 logical output");
    if(world.width()!=256 || world.height()!=256)
        throw std::runtime_error("Opening world staging buffer must be 256x256");

    const auto refs=AdventureInterfaceTheme::resourcesForLevel(level,init_);
    const auto primary=resources_.uiBitmap(refs.workspacePrimary);
    const auto secondary=resources_.uiBitmap(refs.workspaceSecondary);
    if(primary.width!=320||primary.height!=200||secondary.width!=320||secondary.height!=200)
        throw std::runtime_error("Adventure workspace resources must be 320x200");

    // The opening adventure layout is split at the recovered world aperture
    // bottom (y=132): primary owns the gothic viewport surround; secondary
    // owns the lower control/inventory workspace. The world itself overwrites
    // exactly x=32..287, y=4..131.
    for(int y=0;y<132;++y)
        for(int x=0;x<320;++x)
            screen.set(x,y,primary.pixels[static_cast<std::size_t>(y)*320u+x]);
    for(int y=132;y<200;++y)
        for(int x=0;x<320;++x)
            screen.set(x,y,secondary.pixels[static_cast<std::size_t>(y)*320u+x]);

    // WorldRenderer currently preserves the executable's 256-row staging
    // geometry used by the reconstructed projection/raster routines. Until the
    // remaining VGA presentation transfer is reconstructed, present each pair
    // of staging rows as one visible adventure-viewport row. This bridge is
    // deliberately isolated here; the frame/aperture geometry above is the
    // original 320x200 interface layout and does not depend on the bridge.
    AdventureScreenStats stats;
    stats.primaryResource=refs.workspacePrimary;
    stats.secondaryResource=refs.workspaceSecondary;
    const auto src=world.pixels();
    for(int y=0;y<128;++y){
        const auto sy=y*2;
        for(int x=0;x<256;++x){
            screen.set(32+x,4+y,src[static_cast<std::size_t>(sy)*256u+x]);
            ++stats.worldPixels;
        }
    }
    return stats;
}

} // namespace ravenloft::game
