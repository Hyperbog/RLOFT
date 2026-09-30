#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct AverageCF88Ops{std::function<std::int8_t(int)> classCode;std::function<std::int16_t(int)> level;std::function<std::uint8_t(int)> type17;std::function<std::int8_t(int)> base6821;std::function<std::int8_t(int,int)> table67FA;};
int averageCF88(const AverageCF88Ops&o,int ch){
 int total=0,count=0;int rec=0;
 for(int pass=0;pass<3;++pass){
  const int cls=o.classCode(ch+pass);
  const int lv=o.level(ch+pass*2);
  if(cls==-1)break;
  const int q=cls/3,base=o.base6821(cls),adj=o.table67FA(o.type17(ch),q);
  for(int i=1;i<=lv;++i){int v=0;switch(pass){case 0:v=i<10?base+adj:(adj+3);break;case 1:v=i<11?base+adj:(adj+i);break;default:v=i<10?base+adj:adj+2;break;}total+=v;}
  ++count;rec+=4;
 }
 return count?total/count:0;
}
}
