#include "PackedSelector50DC.h"
namespace ravenloft::game {
namespace {std::uint16_t word(std::span<const std::uint8_t>b,std::size_t p){return std::uint16_t(b[p])|(std::uint16_t(b[p+1])<<8);}}
int PackedSelector50DC::select(std::span<const std::uint8_t>b,int mode,int object){
 const auto p=static_cast<std::size_t>(object)*0x248;
 if((word(b,p+0x10)&0xF240)!=0)return -1;
 if(mode==0){
  const auto a=static_cast<std::int8_t>(b[p+0x15]);
  if(a==3)return 0;
  if(static_cast<std::int8_t>(b[p+0x16])==3)return 1;
  if(static_cast<std::int8_t>(b[p+0x17])==3)return 2;
  if(a==1&&static_cast<std::int16_t>(word(b,p+0x20))>=9)return 0;
  return -1;
 }
 if(static_cast<std::int8_t>(b[p+0x15])==4)return 0;
 if(static_cast<std::int8_t>(b[p+0x16])==4)return 1;
 if(static_cast<std::int8_t>(b[p+0x17])==4)return 2;
 return -1;
}
}
