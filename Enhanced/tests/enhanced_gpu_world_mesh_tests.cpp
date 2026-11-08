#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/enhanced_gpu_world_mesh.hpp"
#include <cassert>
#include <stdexcept>

int main() {
    rloft::LevelState l{};
    l.width=3;l.height=3;l.cell_words.assign(9U,0U);
    // Original resource bank [0,2] entries for authored code 1.
    for(unsigned bank:{0U,2U}) {
        const auto at=0x0AU+bank*128U+2U;
        l.header[at]=42U;
    }
    // Flat outdoor sky (-1), RES4 narrow floor selector 2.
    l.header[5]=0xFFU;l.header[6]=0xFFU;
    l.header[7]=2U;
    l.cell_words[4]=1U; // occupied central authored wall
    auto scene=rloft::EnhancedGpuWorldMeshModel::build(l);
    assert(scene.width==3 && scene.height==3);
    assert(scene.floor_faces==8U && scene.ceiling_faces==0U);
    assert(scene.lower_faces==4U && scene.upper_faces==0U);
    assert(scene.textured.count(42U)==1U);
    assert(scene.textured.at(42U).size()==24U);
    assert(scene.textured.at(602U).size()==48U);
    // Mutable CODE.2 map change immediately changes mesh topology.
    l.cell_words[4]=0U;
    scene=rloft::EnhancedGpuWorldMeshModel::build(l);
    assert(scene.floor_faces==9U && scene.lower_faces==0U);
    // Special-center cell blocks until original group-2 kind-1 opens.
    l.cell_words[4]=0x0080U;
    scene=rloft::EnhancedGpuWorldMeshModel::build(l);
    assert(scene.floor_faces==8U);
    rloft::CellControlRecord rec{};
    rec.raw[0]=1U;rec.raw[4]=1U;rec.raw[5]=1U;rec.raw[6]=0U;
    l.group2.push_back(rec);
    scene=rloft::EnhancedGpuWorldMeshModel::build(l);
    assert(scene.floor_faces==9U);
    l.group2[0].raw[6]=8U;
    scene=rloft::EnhancedGpuWorldMeshModel::build(l);
    assert(scene.floor_faces==8U);
    bool rejected=false;
    l.cell_words.pop_back();
    try{(void)rloft::EnhancedGpuWorldMeshModel::build(l);}
    catch(const std::invalid_argument&){rejected=true;}
    assert(rejected);
    return 0;
}
