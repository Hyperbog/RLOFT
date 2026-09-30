#include "SharedLoad7938.h"
namespace ravenloft::game {
int SharedLoad7938::run(SharedLoad7938State&s,const SharedLoad7938Ops&o){
 s.d681=0;s.d682=0;std::string p=s.currentPath+s.suffix398+s.suffix39C;int h=o.open312A0(p.c_str());if(h==-1)return 1;return o.sharedLoad6CAD(h);
}}
