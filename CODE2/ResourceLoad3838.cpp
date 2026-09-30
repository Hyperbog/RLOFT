#include "ResourceLoad3838.h"
namespace ravenloft::game {
void ResourceLoad3838::cleanup380D(ResourceLoad3838State&s){s.heap608C.clear();}
std::int32_t ResourceLoad3838::bump3820(ResourceLoad3838State&s,std::int32_t n){
 const auto old=s.value6094; s.value6094+=n; return old;
}
void ResourceLoad3838::run3838(ResourceLoad3838State&s,const ResourceLoad3838Ops&o){
 const int f=o.call312A0(0x24);
 s.blockC65C.resize(0x708); o.call312EC(s.blockC65C.data(),0x708,f);
 s.blockC660.resize(0xE10); o.call312EC(s.blockC660.data(),0xE10,f);
 o.call312EC(s.table19660.data(),0x100,f);
 o.call312EC(s.table193E0.data(),0x100,f);
 for(auto*p:{&s.c6D8,&s.c6D4,&s.c6D0,&s.c6DC,&s.c6E0,&s.value2560C,&s.c6CC,&s.value255E0})
   o.call312EC(reinterpret_cast<std::uint8_t*>(p),4,f);
 o.call312EC(s.blockC73C.data(),0x400,f);
 o.call312EC(reinterpret_cast<std::uint8_t*>(&s.c738),4,f);
 o.call312EC(reinterpret_cast<std::uint8_t*>(&s.c730),4,f);
 o.call312EC(reinterpret_cast<std::uint8_t*>(&s.c734),4,f);
 s.table258C4.resize(static_cast<std::size_t>(s.c730+1));
 o.call312EC(reinterpret_cast<std::uint8_t*>(s.table258C4.data()),(s.c730+1)*2,f);
 o.call312EC(reinterpret_cast<std::uint8_t*>(&s.value6090),4,f);
 s.heap608C.resize(s.value6090); s.value6094=0;
 s.ptr18FE0.resize(static_cast<std::size_t>(s.c738)+1);
 s.ptr15618.resize(static_cast<std::size_t>(s.c738)+1);
 for(int i=0;i<s.c738;++i){
   std::uint16_t n=0; o.call312EC(reinterpret_cast<std::uint8_t*>(&n),2,f);
   const int off=bump3820(s,n); s.ptr18FE0[i+1]=off; s.ptr15618[i+1]=(i+1)*4;
   o.call312EC(s.heap608C.data()+off,n,f);
 }
 if(s.ptr18FE0.size()>1)s.ptr18FE0[0]=s.ptr18FE0[1];
 o.call312DC(f);
}
}
