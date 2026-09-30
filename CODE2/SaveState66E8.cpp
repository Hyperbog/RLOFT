#include "SaveState66E8.h"
#include <cstring>
namespace ravenloft::game {
namespace {
bool put(const SaveState66E8Ops&o,int f,const void*p,int n){if(o.write31304(p,n,f)==n)return true;o.close312DC(f);return false;}
}
int SaveState66E8::save(SaveState66E8&s,const SaveState66E8Ops&o,int slot){
 auto cp=[&](std::size_t d,std::size_t q,std::size_t n){std::memcpy(s.name12990.data()+d,s.ds.data()+q,n);};
 std::size_t n=0;while(n<s.name12990.size()-1&&s.ds[0xc020+n]){s.name12990[n]=s.ds[0xc020+n];++n;}s.name12990[n]=0;
 auto cat=[&](std::size_t q){while(n<s.name12990.size()-1&&s.ds[q])s.name12990[n++]=s.ds[q++];s.name12990[n]=0;};cat(0x398);cat(0x39c);
 o.rewind31380(reinterpret_cast<const char*>(s.name12990.data()));o.close312DC(o.tell312C8(reinterpret_cast<const char*>(s.name12990.data())));
 int f=o.create312B4(reinterpret_cast<const char*>(s.name12990.data()));if(f==-1)return 1;
 if(slot==-1)cp(0,0x3a8,5);else cp(0,0x6461+std::size_t(slot)*0x32,0x32);
 struct B{std::size_t p;int n;}; const B blocks[]={{0x12990,0x32},{0x22200,0x20},{0xc6c8,4},{0x255d0,4},{0x255d4,4},{0xc658,4},{0xf03c,2},{0xf03e,2},{0xfd08,4},{0x6dae,2}};
 for(auto b:blocks)if(!put(o,f,s.ds.data()+b.p,b.n))return 1;
 auto t=o.time30700();if(!put(o,f,&t,4))return 1;
 const B blocks2[]={{0xcf4c,4},{0x8600,2},{0x8602,1},{0x218e0,0x920},{0xc038,4}};
 for(auto b:blocks2)if(!put(o,f,s.ds.data()+b.p,b.n))return 1;
 std::uint32_t count;std::memcpy(&count,s.ds.data()+0xc038,4);if(!put(o,f,s.ds.data()+0x248ec,int(count*0x16)))return 1;
 if(!put(o,f,s.ds.data()+0xd8a8,8)||!put(o,f,s.ds.data()+0xa7e4,4))return 1;
 for(int i=0;i<0x3e;++i){auto p=0x7330+i*10;int bytes=s.ds[p]*2;std::uint32_t ptr;std::memcpy(&ptr,s.ds.data()+p+5,4);if(!put(o,f,s.ds.data()+ptr,bytes)||!put(o,f,s.ds.data()+p+9,1))return 1;}
 const B tail[]={{0xf920,4},{0xf91c,4},{0x67ec,0x0f},{0xa808,0x16d0},{0x6fa0,0x10}};for(auto b:tail)if(!put(o,f,s.ds.data()+b.p,b.n))return 1;
 o.close312DC(f);return 0;
}}
