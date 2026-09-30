#include "MapLoad2AE8.h"
#include <algorithm>
#include <cstring>

namespace ravenloft::game {

std::int32_t MapLoad2AE8::run(MapLoad2AE8State& s,const MapLoad2AE8Ops& o,std::int32_t n){
 if(s.savedRecords.empty()){
   s.savedRecordBytes6054=static_cast<std::int32_t>(o.call1C08());
   s.savedRecords.assign(s.records5760.begin(),s.records5760.begin()+s.savedRecordBytes6054);
 }
 std::fill(s.map1760.begin(),s.map1760.end(),0xFF);
 std::fill(s.records5760.begin(),s.records5760.end(),0);

 if(n==s.currentMapC6C8){
   o.call1558();
   std::fill(s.records5760.begin(),s.records5760.end(),0);
   std::copy(s.savedRecords.begin(),s.savedRecords.end(),s.records5760.begin());
   s.savedRecords.clear(); s.savedRecordBytes6054=0;
   return 1;
 }

 const std::int32_t f=o.openNumbered(n);
 if(f==-1) return 0;

 std::int32_t block=0;
 o.call3131C(0x0D,0,f);
 o.call312EC(reinterpret_cast<std::uint8_t*>(&block),4,f);
 o.call3131C(block,0,f);

 o.call312EC(reinterpret_cast<std::uint8_t*>(&s.entityCount604C),2,f);
 if(s.entityCount604C!=0){
   s.entityData6048.resize(static_cast<std::size_t>(s.entityCount604C)*0x16u);
   o.call312EC(s.entityData6048.data(),static_cast<std::int32_t>(s.entityData6048.size()),f);
 }

 o.call3131C(0x19,0,f);
 o.call312EC(reinterpret_cast<std::uint8_t*>(&block),4,f);
 o.call3131C(block,0,f);
 o.call312EC(reinterpret_cast<std::uint8_t*>(&s.height12756),2,f);
 o.call312EC(reinterpret_cast<std::uint8_t*>(&s.width12754),2,f);

 const std::int32_t cells=static_cast<std::int32_t>(s.width12754)*s.height12756;
 o.call312EC(s.map1760.data(),cells,f);
 o.call312EC(reinterpret_cast<std::uint8_t*>(&block),4,f);
 if(block<0x7FF) o.call312EC(s.records5760.data(),block,f);
 o.call310B0(f);
 return 1;
}

} // namespace ravenloft::game
