#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/enhanced_gpu_backdrop.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    using rloft::EnhancedGpuBackdrop;
    using B=EnhancedGpuBackdrop;
    rloft::Palette6 p{};
    p.rgb[3U]=63U; // index 1, red
    p.rgb[7U]=32U; // index 2, green
    const std::size_t world=B::kWorldWidth*B::kWorldHeight;
    const std::size_t canvas_size=B::kCanvasWidth*B::kCanvasHeight*4U;
    std::vector<std::uint8_t> indices(world,1U);
    std::vector<std::uint8_t> canvas(canvas_size,0xA5U);
    indices[0U]=2U; // world top-left at (32,4)

    B::compose(indices,p,canvas,false);
    const auto a=B::rgba_offset(4U,32U);
    assert(canvas[a]==0U);
    assert(canvas[a+1U]==static_cast<std::uint8_t>((32U*255U+31U)/63U));
    assert(canvas[a+2U]==0U && canvas[a+3U]==255U);
    const auto b=B::rgba_offset(259U,287U);
    assert(canvas[b]==255U && canvas[b+1U]==0U &&
           canvas[b+2U]==0U && canvas[b+3U]==255U);
    assert(canvas[B::rgba_offset(4U,31U)]==0xA5U);
    assert(canvas[B::rgba_offset(260U,32U)]==0xA5U);

    // When the top-edge menu is active the actual source menu pixels,
    // not original panorama, must remain above the OpenGL world.
    canvas.assign(canvas_size,0xA5U);
    B::compose(indices,p,canvas,true);
    assert(canvas[B::rgba_offset(4U,32U)]==0xA5U);
    assert(canvas[B::rgba_offset(63U,32U)]==0xA5U);
    assert(canvas[B::rgba_offset(64U,32U)]==255U);

    // GPU failure after replacing the backdrop restores the precise
    // pre-existing source software world without touching HUD chrome.
    std::vector<std::uint32_t> source(320U*400U,0x00123456U);
    source[4U*320U+32U]=0x00AB31CDU;
    B::restore(source,canvas);
    assert(canvas[a]==0xABU && canvas[a+1U]==0x31U &&
           canvas[a+2U]==0xCDU && canvas[a+3U]==255U);
    assert(canvas[b]==0x12U && canvas[b+1U]==0x34U &&
           canvas[b+2U]==0x56U && canvas[b+3U]==255U);
    assert(canvas[B::rgba_offset(260U,32U)]==0xA5U);

    bool rejected=false;
    indices.pop_back();
    try{B::compose(indices,p,canvas,false);}
    catch(const std::invalid_argument&){rejected=true;}
    assert(rejected);
    indices.push_back(1U);
    canvas.pop_back();
    rejected=false;
    try{B::compose(indices,p,canvas,false);}
    catch(const std::invalid_argument&){rejected=true;}
    assert(rejected);
    rejected=false;
    try{B::restore(source,canvas);}
    catch(const std::invalid_argument&){rejected=true;}
    assert(rejected);
}
