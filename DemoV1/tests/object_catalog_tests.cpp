#include "rloft/object_catalog.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
void put16(std::vector<std::uint8_t>& b,std::size_t o,std::uint16_t v){
 b[o]=static_cast<std::uint8_t>(v);
 b[o+1]=static_cast<std::uint8_t>(v>>8U);
}
void put32(std::vector<std::uint8_t>& b,std::size_t o,std::uint32_t v){
 for(int i=0;i<4;++i)b[o+static_cast<std::size_t>(i)]=static_cast<std::uint8_t>(v>>(i*8));
}
}

int main(){
 std::vector<std::uint8_t> bytes(4U+2U*0x41U,0);
 put32(bytes,0,2);
 auto rec0=4U;
 bytes[rec0+0]='K';bytes[rec0+1]='E';bytes[rec0+2]='Y';
 put16(bytes,rec0+0x28,686);
 bytes[rec0+0x2A]=8;
 bytes[rec0+0x2B]=3;
 put16(bytes,rec0+0x2D,17);
 bytes[rec0+0x2F]=3;bytes[rec0+0x30]=5;bytes[rec0+0x31]=0xFF;bytes[rec0+0x37]=2;
 auto rec1=4U+0x41U;
 bytes[rec1]='B';put16(bytes,rec1+0x2D,99);
 const auto cat=rloft::ObjectCatalog::parse(bytes);
 assert(cat.size()==2);
 assert(cat.at(0).name=="KEY");
 assert(cat.at(0).scene_shape_base_local==686);
 assert(cat.at(0).scene_shape_stride==8);
 assert(cat.at(0).scene_influence_step==3);
 assert(cat.at(0).ui_graphic_index==17);
 assert(cat.at(0).item_category==3);
 assert(cat.at(0).weight_half_units==5);
 assert(cat.at(0).container_state_index==-1);
 assert(cat.at(0).enchantment==2);
 assert(cat.at(1).ui_graphic_index==99);
 bool rejected=false;
 try{(void)cat.at(2);}catch(const std::out_of_range&){rejected=true;}
 assert(rejected);
}
