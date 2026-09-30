#include "MapExport26E8.h"
#include <algorithm>

namespace ravenloft::game {

void MapExport26E8::run(const MapExport26E8State& s,const MapExport26E8Ops& o,
                        std::int32_t arg0,std::int32_t output,std::int32_t alt) {
 if(alt==0) {
   if(o.call2354()==0){o.call2E6D0(); return;}
   if(s.value2561B>0x50) o.call31107(0x0F,arg0);
 }
 const int w=s.width12754, h=s.height12756;
 std::vector<std::uint8_t> terrain(w),entities(w),labels(w);

 for(int y=0;y<h;++y) {
   std::fill(terrain.begin(),terrain.end(),0x20);
   std::fill(entities.begin(),entities.end(),0x20);
   std::fill(labels.begin(),labels.end(),0x20);

   for(int x=0;x<w;++x) {
     const std::size_t pos=static_cast<std::size_t>(x+y*s.height12756);
     const std::uint8_t cell=s.map1760[pos];
     if(cell!=0xFF) {
       int code=cell;
       if(code<0x20) code=o.call09B0(static_cast<int>(pos),y,x);
       terrain[x]=(code>=0x20&&code<=0x33)?s.tileChars6011[code]:s.fallback6030;
     }

     int n=o.call0700(x,y,0);
     while(n!=-1) {
       const auto t=o.entityType22246(n);
       entities[x]=(t>=0x3C&&t<=0x41)?0x2A:0xFE;
       n=o.call0700(x,y,n+1);
     }

     std::uint8_t lab[2]{};
     if(o.call23B8(y,x,lab)) {
       labels[x]=lab[0];
       // Original stream reserves a second byte for successful labels; the
       // exact interleaving is retained below by direct output handling.
     }
   }

   if(alt==0) {
     for(auto c:terrain)o.call31107(c,output); o.call31107(0x0D,output);
     for(auto c:entities)o.call31107(c,output); o.call31107(0x0D,output);
     for(auto c:labels)o.call31107(c,output);
     o.call31107(s.value5F60,output); o.call31107(s.value5F61,output);
   } else {
     for(int x=0;x<w;++x) {
       if(entities[x]!=0x20) terrain[x]=entities[x];
       if(labels[x]!=0x20) terrain[x]=labels[x];
     }
     o.call31304(terrain.data(),w,alt);
     const std::uint8_t nl[2]={s.value5F60,s.value5F61};
     o.call31304(nl,2,alt);
   }
 }
 o.call2474(output,output,alt);
 if(alt==0){o.call31107(0x0C,output); o.call31491(output);}
}

} // namespace ravenloft::game
