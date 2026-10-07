#include "rloft/view_sweep_record.hpp"
#include <cassert>
namespace {
void put16(std::array<std::uint8_t,0x2A4>& b,std::size_t o,std::uint16_t v){b[o]=static_cast<std::uint8_t>(v);b[o+1]=static_cast<std::uint8_t>(v>>8);}
}
int main(){
 rloft::LevelState l{};l.width=3;l.height=2;l.cell_words.assign(6,0);
 constexpr std::uint8_t code=2;
 put16(l.header,0x00A+code*2,10);put16(l.header,0x08A+code*2,20);put16(l.header,0x10A+code*2,30);put16(l.header,0x18A+code*2,40);
 l.cell_words[1]=static_cast<std::uint16_t>(code|0x0200|0x1000|0x8000);
 rloft::CellControlRecord r{};r.raw[1]=2;r.raw[4]=0;r.raw[5]=1;l.group1.push_back(r);
 auto rec=rloft::ViewSweepRecordModel::materialize(l,{rloft::SweepSide::Right,{1,0},std::nullopt,123,0x200,false,true});
 assert(rec.depth==123&&rec.resource_outer==22&&rec.resource_inner==12);
 assert(rec.prepass==1&&rec.texture_column_offset==0x0DC0&&rec.cell_index==1&&rec.shade_decrement==2);
 assert((l.cell_words[1]&0x0440)==0x0440);
 rec=rloft::ViewSweepRecordModel::materialize(l,{rloft::SweepSide::Left,{1,0},std::nullopt,9,0x200,true,false});
 assert(rec.resource_outer==42&&rec.resource_inner==32&&rec.texture_column_offset==0x0DC0);
 // Frame substitution: source code2 uses adjacent special-center code3, +1 variant, and base offset.
 put16(l.header,0x00A+3*2,100);put16(l.header,0x08A+3*2,200);put16(l.header,0x10A+3*2,300);put16(l.header,0x18A+3*2,400);
 l.cell_words[1]=static_cast<std::uint16_t>(code|0x0100|0x1000);
 l.cell_words[2]=static_cast<std::uint16_t>(3|0x0080);
 rec=rloft::ViewSweepRecordModel::materialize(l,{rloft::SweepSide::Right,{1,0},rloft::CellPosition{2,0},1,0x240,false,false});
 assert(rec.resource_outer==201&&rec.resource_inner==101&&rec.texture_column_offset==0x240);
}
