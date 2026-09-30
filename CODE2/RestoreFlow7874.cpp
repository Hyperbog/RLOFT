#include "RestoreFlow7874.h"
namespace ravenloft::game {
void RestoreFlow7874::run(RestoreFlow7874State&s,const RestoreFlow7874Ops&o,const char*cur,const char*aux,int missing,int auxMsg,int copyMsg){
 s.path6690[0]='0';s.path6690[1]='0';s.path6690[2]=0;if(o.exists33A5A(s.path6690)){o.call1C21C(missing);return;}
 if(o.exists33A5A(cur)){o.call1C21C(missing);return;}o.disk7378();if(o.exists33A5A(aux))o.call2F580(auxMsg);
 if(o.copy73A0(s.path6690,cur)==1)o.call2F580(copyMsg);s.path6690[2]='\\';
}}
