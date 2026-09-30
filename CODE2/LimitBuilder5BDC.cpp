#include "LimitBuilder5BDC.h"
namespace ravenloft::game {
void LimitBuilder5BDC::build(LimitBuilder5BDCState&s,const LimitBuilder5BDCOps&o,int mode,int object,int level,std::span<const std::uint8_t> table){
 s.limits6208.fill(0);o.copy30CD4(s.limits6208.data(),table.data()+level*8,8);
 const auto b=std::size_t(object)*0x248;
 if(mode==0){
  if(static_cast<std::int8_t>(s.objects[b+0x15])==1)return;
  const auto idx=s.objects[b+0x3B];
  for(int i=0;i<8;++i)if(table[level*8+i])s.limits6208[i]=std::uint8_t(s.limits6208[i]+s.table6160[idx*8+i]);
 }else{
  auto n=s.table613F[s.objects[b+0x39]];if(n&&n<8)for(++n;n<8;++n)s.limits6208[n]=0;
 }
}
}
