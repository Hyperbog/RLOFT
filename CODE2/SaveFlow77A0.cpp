#include "SaveFlow77A0.h"
namespace ravenloft::game {
void SaveFlow77A0::run(SaveFlow77A0State&s,const SaveFlow77A0Ops&o,const char*cur,const char*aux,int msgA,int msgB,int msgC){
 s.a79d=0;o.call171FC(s.c6c8);s.a79d=1;if(o.save66E8(0)!=0){o.call803C(msgC);return;}
 s.path6690[0]='0';s.path6690[1]='0';s.path6690[2]=0;o.call33A38(s.path6690);
 if(o.exists33A5A(s.path6690)){o.call1C21C(msgA);return;}
 o.disk7378();if(o.exists33A5A(aux))o.call2F580(msgB);
 if(o.copy73A0(cur,s.path6690)==1){o.call1C21C(msgA);return;}s.path6690[2]='\\';
}}
