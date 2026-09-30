#include "ResourcePanel518C.h"
namespace ravenloft::game {
void ResourcePanel518C::draw(ResourcePanel518CState&s,const ResourcePanel518COps&o,int mode,int selected,int surface,int packedBase,int labelBase,const std::uint8_t*labels,const std::uint8_t*values){
 o.call291B0(std::uint8_t(s.valueC050));for(int i=0;i<256;++i)s.remap12A30[i]=std::uint8_t(i);s.remap12A30[0]=0xF0;s.remap12A30[0x1B]=0xF1;o.call3059C(s.remap12A30);
 o.call2E510(0,0,mode,0);o.call2E510(0,0,mode+1,0);
 int y=5,x=6;std::size_t obj=0x218E0;
 for(int ch=0;ch<5;++ch,obj+=0x248,y+=0x42,x+=0x42){
  if(ch<4&&o.call50DC(mode,ch)<0)continue;
  o.call2975C(ch==selected,0,y,int(s.value258DC)+0x4A+mode,0);
  o.call30508(ch==selected?1:2,x,0x30,ch==4?int(s.value60E0):int(obj),1);
 }
 int rowData=0,countOff=0,labelOff=0;
 for(int row=0;row<6;++row,rowData+=4,countOff+=8,++labelOff){
  o.call30D30(int(reinterpret_cast<std::intptr_t>(s.rowBuffers12B58[row].data())),-1,0x14);
  o.call3210F(0x759C,int(s.value60A4),row+1);
  int sx=s.rowX63F0[row],sy=s.rowY63F4[row];
  o.call30508(sx+2,sy,0x64,0x759C,1);o.call3210F(0x759C,int(s.value60A8),values[labelOff]);o.call30508(sx+0x0E,sy,0x64,0x759C,1);
  int out=0;const int n=s.counts6420[row+mode*4];
  for(int col=0;col<n;++col){
   const auto v=std::uint8_t(*(reinterpret_cast<const std::uint8_t*>(packedBase+row*10+col)));
   if((v&15)==15)continue;
   o.call3060C(sx+0x2D+out*12,sy+2,0x380,1);o.call3060C(sx+0x2D+out*12,sy+0x19,o.call2E6F0(v&15),1);
   o.call1C368(0x759C,labelBase+col);o.call30630(sx+0x2D+out*12,sy+0x20,0x759C,1,0x44);
   auto b=s.rowBuffers12B58[row];if(out*2+1<int(b.size())){b[out*2]=std::int8_t(col);b[out*2+1]=std::int8_t(v&15);}++out;
  }
 }
 o.call32220(0xC054,0,0xFF,0,0,0);o.call325B0(0xC054,0,surface,0x388,0x100);int saved=0;o.call32468(0xC054,0,0xEC,6,3,int(reinterpret_cast<std::intptr_t>(&saved)));
 for(int i=0;i<256;++i)s.remap12A30[i]=std::uint8_t(i);
 const std::uint8_t at[]={0x08,0x1B,0x16,0x1A,0x12,0x09,0x10,0x0B,0x0A,0x5C,0x58,0x55,0x53,0x57,0x54,0x56};
 for(int i=0;i<16;++i)s.remap12A30[at[i]]=std::uint8_t(0xF0+i);
 s.remap12A30[0x59]=0xED;s.remap12A30[0x0D]=0xEE;s.remap12A30[0x0C]=0xEF;
 o.call2AA9C(0,s.remap12A30,0x880D);o.call1A5B0(s.valueC050,s.value8600,s.value8602);reinterpret_cast<std::uint8_t*>(&s.valueC050)[0]^=1;o.call322EC(0xC054,0,0xFF,0,0,0);
}
}
