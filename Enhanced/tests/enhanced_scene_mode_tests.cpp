#ifdef NDEBUG
#undef NDEBUG
#endif

#include "rloft/exploration_runtime.hpp"
#include <cassert>
#include <vector>

int main() {
    // Legacy/forensic source tests retain VGA trace generation by default.
    // Only player-facing native startup explicitly opts into bypass.
    rloft::ExplorationRuntime rt(
        rloft::ExplorationSessionState{},
        rloft::RendererTables{},
        std::vector<rloft::EnvironmentProfile>{});
    assert(!rt.enhanced_pc_scene_rendering());
    rt.set_enhanced_pc_scene_rendering(true);
    assert(rt.enhanced_pc_scene_rendering());
    rt.set_enhanced_pc_scene_rendering(false);
    assert(!rt.enhanced_pc_scene_rendering());
}
