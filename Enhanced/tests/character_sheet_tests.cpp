#include "rloft/character_sheet.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {
void put16(std::vector<std::uint8_t>& b,std::size_t o,std::uint16_t v){
 b[o]=static_cast<std::uint8_t>(v);
 b[o+1U]=static_cast<std::uint8_t>(v>>8U);
}
void put32(std::vector<std::uint8_t>& b,std::size_t o,std::uint32_t v){
 for(int i=0;i<4;++i)b[o+static_cast<std::size_t>(i)]=static_cast<std::uint8_t>(v>>(i*8));
}
}

int main(){
 std::vector<std::uint8_t> bytes(rloft::PartyImage::kSize,0xFF);
 for(std::size_t i=0;i<16;++i)bytes[i]=0;
 bytes[0]='D';bytes[1]='E';bytes[2]='L';
 put16(bytes,0x010,0x0100);
 bytes[0x012]=3;
 bytes[0x013]=1;
 bytes[0x014]=5;
 bytes[0x015]=0;
 bytes[0x016]=4;
 bytes[0x017]=0xFF;
 put16(bytes,0x018,123);
 bytes[0x01A]=4;
 bytes[0x01B]=static_cast<std::uint8_t>(-2);
 put16(bytes,0x01C,27);
 put16(bytes,0x01E,31);
 put16(bytes,0x020,6);
 put16(bytes,0x022,5);
 put16(bytes,0x024,0xFFFF);
 put32(bytes,0x026,12345);
 put32(bytes,0x02A,67890);
 put32(bytes,0x02E,0xFFFFFFFFU);
 for(std::size_t i=0;i<6;++i){
   bytes[0x032+i*2U]=static_cast<std::uint8_t>(10+i);
   bytes[0x033+i*2U]=static_cast<std::uint8_t>(11+i);
 }
 bytes[0x03E]=18;
 bytes[0x03F]=76;

 const auto party=rloft::PartyImage::from_bytes(bytes);
 const auto sheet=rloft::CharacterSheetModel::actor(party,0);

 assert(sheet.name=="DEL");
 assert(sheet.active);
 assert(sheet.status_word==0x0100);
 assert(sheet.portrait_index==3);
 assert(sheet.sex_code==1);
 assert(sheet.race_code==5);
 assert(sheet.age==123);
 assert(sheet.alignment_code==4);
 assert(sheet.base_armor_class==-2);
 assert(sheet.current_hp==27);
 assert(sheet.max_hp==31);

 assert(sheet.classes[0].present());
 assert(sheet.classes[0].class_code==0);
 assert(sheet.classes[0].level==6);
 assert(sheet.classes[0].progress==12345);
 assert(sheet.classes[0].text_resource_index()==0x15);
 assert(sheet.classes[1].class_code==4);
 assert(sheet.classes[1].text_resource_index()==0x19);
 assert(!sheet.classes[2].present());
 assert(sheet.classes[2].text_resource_index()==-1);
 assert(sheet.class_track_count()==2);

 assert(sheet.race_text_resource_index()==0x14);
 assert(sheet.sex_text_resource_index()==0x1C);
 assert(sheet.alignment_text_resource_index()==0x21);

 for(std::size_t i=0;i<6;++i){
   assert(sheet.abilities[i].base==10+i);
   assert(sheet.abilities[i].current==11+i);
   assert(sheet.abilities[i].label_text_resource==0x2F+static_cast<int>(i));
 }
 assert(sheet.exceptional_strength_base==18);
 assert(sheet.exceptional_strength_current==76);
}
