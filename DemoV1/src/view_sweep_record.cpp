#include "rloft/view_sweep_record.hpp"
#include <algorithm>
#include <stdexcept>
namespace rloft { namespace {
const CellControlRecord* group1_at(const LevelState& level,CellPosition cell) noexcept {
 const auto it=std::find_if(level.group1.begin(),level.group1.end(),[&](const CellControlRecord& r){return r.column()==cell.column&&r.row()==cell.row;});
 return it==level.group1.end()?nullptr:&*it;
}
}
ViewSweepRecord ViewSweepRecordModel::materialize(LevelState& level,const SweepMaterializationInput& in){
 const auto index=ExplorationWorldModel::cell_index(level,in.cell);
 if(index>=level.cell_words.size())throw std::runtime_error("level cell array does not match dimensions");
 auto& raw=level.cell_words[index];
 raw=static_cast<std::uint16_t>(raw|ExplorationWorldModel::kVisibleAndExplored);
 std::uint16_t source_word=raw;
 std::uint8_t code=static_cast<std::uint8_t>(source_word&0x003FU);
 std::uint32_t extra=0;
 std::uint32_t offset=in.texture_base;
 if(in.side==SweepSide::Left){
   if(in.axis_step_negative)offset=0x0FC0U-in.texture_base;
 }else{
   if(!in.axis_step_negative)offset=0x0FC0U-in.texture_base;
 }
 if((source_word&0x0100U)!=0U && in.special_neighbor){
   if(!ExplorationWorldModel::in_bounds(level,*in.special_neighbor))throw std::out_of_range("special sweep neighbor");
   const auto alternate=ExplorationWorldModel::cell_word(level,*in.special_neighbor);
   if((alternate&0x0080U)!=0U){
     code=static_cast<std::uint8_t>(alternate&0x003FU);
     offset=in.texture_base;
     extra=1;
   }
 }
 if((source_word&0x1000U)!=0U && extra==0U){
   if(const auto* r=group1_at(level,in.cell))extra=static_cast<std::uint32_t>(r->control()&0x03U);
 }
 const auto banks=EnvironmentViewModel::resource_banks(level);
 ViewSweepRecord out{};
 out.depth=in.depth;
 out.prepass=in.prepass?1U:0U;
 out.texture_column_offset=offset;
 out.cell_index=static_cast<std::uint32_t>(index);
 out.shade_decrement=(source_word&0xC000U)>>14U;
 if(in.side==SweepSide::Left){
   out.resource_outer=static_cast<std::uint32_t>(banks.alternate_b[code])+extra;
   out.resource_inner=static_cast<std::uint32_t>(banks.primary_b[code])+extra;
 }else{
   out.resource_outer=static_cast<std::uint32_t>(banks.alternate_a[code])+extra;
   out.resource_inner=static_cast<std::uint32_t>(banks.primary_a[code])+extra;
 }
 return out;
}
}  // namespace rloft
