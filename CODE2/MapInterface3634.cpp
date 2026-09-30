#include "MapInterface3634.h"
#include <algorithm>

namespace ravenloft::game {
void MapInterface3634::run(MapInterface3634State&s,const MapInterface3634Ops&o){
 o.call19CCC(0);
 std::fill(s.palette759C.begin(),s.palette759C.end(),0);
 s.value6058=0; o.call1A9F8(); o.call291B0(s.valueC050);
 s.viewX12744=std::max(-0x28,static_cast<int>(s.playerX255D2)-0x18);
 s.viewY12740=std::max(-0x28,static_cast<int>(s.playerY255D6)-0x18);
 s.mapHeight12756=s.sourceHeight25619; s.mapWidth12754=s.sourceWidth2561B;
 s.mapIndex1274C=s.currentMapC6C8; s.currentMap5FCB=1;
 const int cells=static_cast<int>(s.mapWidth12754)*s.mapHeight12756;
 std::fill_n(s.map1760.begin(),std::min<int>(cells,s.map1760.size()),0xFF);
 o.call1558(); o.call1834();
 o.call139C(s.viewX12744,s.viewY12740,0x30,0x2F,1);
 o.call1A5B0(s.valueC050,s.value8600,s.value8602); s.valueC050^=1u;
 o.call22D70(); s.stateD680=4;
 while(s.stateD680==4){
   if(s.action5FCA)o.call34B0();
   if(s.value9E35==0)s.action5FCA=0;
   o.call22D8C();
 }
 if(!s.savedRecords.empty()){
   std::fill(s.records5760.begin(),s.records5760.end(),0);
   std::copy_n(s.savedRecords.begin(),std::min<std::size_t>(s.savedRecordBytes6054,s.savedRecords.size()),s.records5760.begin());
   s.savedRecords.clear(); s.savedRecordBytes6054=0;
 }
 if(!s.entityData6048.empty()){s.entityData6048.clear();s.entityCount604C=0;}
 o.tail19CF4();
}
} // namespace ravenloft::game
