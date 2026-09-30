#include <cstdint>
#include <cstring>
namespace ravenloft::game {
struct PackedReader13268{const std::uint8_t*data{};std::uint32_t pos{};
 std::uint8_t u8(){return data[pos++];}
 std::uint16_t u16(){std::uint16_t v;std::memcpy(&v,data+pos,2);pos+=2;return v;}
 std::uint32_t u32(){std::uint32_t v;std::memcpy(&v,data+pos,4);pos+=4;return v;}
 void bytes(void*dst,std::uint32_t n){std::memcpy(dst,data+pos,n);pos+=n;}
 void block300(void*dst){bytes(dst,0x300);}
};
void removeRecord131DC(std::uint8_t*base,std::int16_t&count){
 for(int i=0;i<count;++i){auto*p=base+i*0x16;std::int16_t a,b;std::uint16_t c;std::memcpy(&a,p+2,2);std::memcpy(&b,p+6,2);std::memcpy(&c,p+14,2);
  if(a==0x1f&&b==0x0d&&c==0x3c){std::memmove(p,p+0x16,(count-i-1)*0x16);--count;return;}}
}
std::int32_t adjustedTime13248(std::int32_t base,std::int32_t delta){return base+delta-0x1f400;}
}
