#include <cstdint>
#include <cstring>
#include <functional>
namespace ravenloft::game {
struct Stream1330COps{
 std::uint8_t* stream{};std::uint32_t& pos;
 std::function<void(void*,const void*,std::uint32_t)> copy;
 std::function<void(int,int,int,int,int)> blit;
};
void decode1330C(const Stream1330COps&o,std::uint8_t*scan){
 for(int y=0;y<0x190;++y){int out=0;auto*p=scan;
  while(out<0x140){std::uint8_t n=o.stream[o.pos++];if(n<0x80){int len=n+1;o.copy(p,o.stream+o.pos,len);o.pos+=len;p+=len;out+=len;}
   else if(n>0x80){int len=0x101-n;std::uint8_t v=o.stream[o.pos++];std::memset(p,v,len);p+=len;out+=len;}}
  o.blit(0x759c,y,0,0x50,0);
 }
}
struct Script133CCOps{
 std::uint8_t* stream{};std::uint32_t&pos;
 std::function<void(std::uint8_t)> page;
 std::function<std::uint8_t()> nextByte;
 std::function<void(int,int,int)> put;
};
void script133CC(const Script133CCOps&o){
 o.page(0);std::uint16_t outer;std::memcpy(&outer,o.stream+o.pos,2);o.pos+=2;
 int base=0;while(outer--){std::uint16_t add,n;std::memcpy(&add,o.stream+o.pos,2);std::memcpy(&n,o.stream+o.pos+2,2);o.pos+=4;base+=add;
  while(n--){std::uint16_t x,count;std::memcpy(&x,o.stream+o.pos,2);std::memcpy(&count,o.stream+o.pos+2,2);o.pos+=4;int px=0;
   while(count--){for(std::uint16_t k=0;k<x;++k)o.put(px++,base,o.nextByte());}}}}
}
struct TimedStream13488Ops{
 std::uint32_t&active,&remaining,&pos,&frame,&nextFrame;
 std::function<void()> decode;
 std::function<void(std::uint32_t,int,std::uint32_t)> seek;
};
void timedStream13488(const TimedStream13488Ops&o){
 if(o.active){if(--o.remaining==0){o.seek(o.nextFrame,0,o.frame);o.pos=0x1f400;o.active=0;return;}o.pos+=6;o.decode();o.seek(o.frame,0,o.frame);o.pos=0x1f400;return;}
 std::uint16_t n;std::memcpy(&n,&o.remaining,2);o.remaining=n;o.pos+=2;o.nextFrame+=o.pos;o.pos+=4;o.decode();o.seek(0,1,o.frame);o.seek(o.nextFrame,0,o.frame);o.pos=0x1f400;o.active=1;
}
}
