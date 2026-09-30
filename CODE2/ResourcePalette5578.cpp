#include "ResourcePalette5578.h"
namespace ravenloft::game {
void ResourcePalette5578::run(ResourcePalette5578State&s,const ResourcePalette5578Ops&o){
 o.call32220(0xC054,0,0xFF,0,0,0);o.call30D30(0xC054,0,0x300);
 for(int i=0;i<256;++i)s.remap12A30[i]=std::uint8_t(i);
 const std::uint8_t src[]={0x08,0x1B,0x16,0x1A,0x12,0x09,0x10,0x0B,0x0A,0x5C,0x58,0x55,0x53,0x57,0x54,0x56};
 for(int i=0;i<16;++i)s.remap12A30[0xF0+i]=src[i];
 s.remap12A30[0xED]=0x59;s.remap12A30[0xEE]=0x0D;s.remap12A30[0xEF]=0x0C;
 o.call2AA9C(0,s.remap12A30,0x880D);
 for(int i=0;i<256;++i)s.remap12A30[i]=std::uint8_t(i);
 o.call3059C(s.remap12A30);
}
}
