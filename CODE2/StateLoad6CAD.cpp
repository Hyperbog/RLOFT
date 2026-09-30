#include "StateLoad6CAD.h"
namespace ravenloft::game {
static std::uint16_t w(std::span<std::uint8_t>d,std::size_t p){return std::uint16_t(d[p])|(std::uint16_t(d[p+1])<<8);}
int StateLoad6CAD::run(StateLoad6CADState&s,const StateLoad6CADOps&o,int handle,int ebx,int a0,int a1,int a2){
 std::uint32_t l0=0,l1=0,l2=0,l3=0,l4=0,l5=0,l6=0,l7=0;
 o.seek3131C(0x32,0,handle);o.read312EC(s.ds.data()+0x22200,0x20,handle);o.read312EC(&l0,4,handle);
 o.read312EC(s.ds.data()+0x255d0,4,handle);o.read312EC(s.ds.data()+0x255d4,4,handle);o.read312EC(&l1,4,handle);o.call1C0C0(int(l1));
 if(s.ds[0xc02f]){s.ds[0x255d0]=0;s.ds[0x255d1]=0x80;s.ds[0x255d4]=0;s.ds[0x255d5]=0x80;}
 o.read312EC(s.ds.data()+0xf03c,2,ebx);o.read312EC(s.ds.data()+0xf03e,2,ebx);o.read312EC(s.ds.data()+0xfd08,4,ebx);o.read312EC(s.ds.data()+0x6dae,2,ebx);
 o.read312EC(&l2,4,ebx);o.call30738(int(l2));o.read312EC(s.ds.data()+0xcf4c,4,ebx);o.read312EC(&l3,2,ebx);o.read312EC(&l4,1,ebx);o.call1A344(std::int16_t(l3),std::uint8_t(l4));
 o.read312EC(s.ds.data()+0x218e0,0x920,ebx);o.read312EC(&l5,4,ebx);o.read312EC(s.ds.data()+0x248ec,int(l5)*0x16,ebx);
 for(std::size_t p=0;p<0x920;p+=0x248){if((w(s.ds,0x218f0+p)&0x8000)!=0)continue;if(std::int8_t(s.ds[0x218f5+p])==1)s.ds[0x21a11+p]|=4;for(int i=0;i<3;++i){auto t=std::int8_t(s.ds[0x218f5+p+i]);if(t==4)s.ds[0x21a11+p]|=2;if(t==3)s.ds[0x21a11+p]|=1;}}
 o.read312EC(s.ds.data()+0xd8a8,8,ebx);o.read312EC(&l6,4,ebx);o.call1E74C(int(l6));
 for(int i=0;i<62;++i){auto p=0x7330+i*10;int n=int(s.ds[p])*2;o.read312EC(s.ds.data()+p+5,n,ebx);o.read312EC(s.ds.data()+p+9,1,ebx);}
 o.read312EC(&l6,4,ebx);o.read312EC(&l7,4,ebx);int n=o.read312EC?0:0; (void)n;
 o.read312EC(s.ds.data()+0x12990,0x0f,ebx);
 auto*tmp=static_cast<std::uint8_t*>(o.alloc3104C(0x248));for(int i=0;i<10;++i){o.read312EC(tmp,0x248,ebx);o.copy30CD4(s.ds.data()+0xa808+i*0x248,tmp,0x248);}o.read312EC(tmp,0x10,ebx);o.copy30CD4(s.ds.data()+0x6fa0,tmp,0x10);o.free310B0(tmp);
 o.close312DC(ebx);s.cf5a5d={1,1,1,1};o.call31EDC(a0);o.call31F38(a1);s.ds[0x6f90]=s.ds[0x6f94]=s.ds[0x6f98]=s.ds[0x6f9c]=0;s.c6c8=-1;o.call181B4(a2);s.bf44=0x270f;s.c038=int(l5);return 0;
}}
