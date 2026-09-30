#include "PackedAccounting43F4.h"
namespace ravenloft::game {
void PackedAccounting43F4::adjust(std::span<const std::uint8_t>b,const PackedAccounting43F4Ops&o,int obj,int*primary,int*secondary){
 const int skipA=o.call50DC(0,obj),skipB=o.call50DC(1,obj);
 const auto base=static_cast<std::size_t>(obj)*0x248+0x218E0;
 for(int row=0;row<8;++row)for(int col=0;col<10;++col){
  if(col!=skipA){
   const auto v=b[base+row*10+col+0x6A],lo=v&15,hi=v>>4;
   if(lo!=15&&lo!=hi&&hi<lo)*primary-=int(lo-hi);
  }
  if(col!=skipB){
   const auto v=b[base+row*10+col+0xBA],lo=v&15,hi=v>>4;
   if(lo!=15&&lo!=hi&&hi<lo)--*secondary;
  }
 }
}
}
