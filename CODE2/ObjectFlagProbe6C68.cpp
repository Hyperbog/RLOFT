#include "ObjectFlagProbe6C68.h"
namespace ravenloft::game {
void ObjectFlagProbe6C68::run(ObjectFlagProbe6C68State&s,const ObjectFlagProbe6C68Ops&o){
 const auto*p=o.lookup1E8C8(s.x255D2,s.y255D6);s.cf6C=0;
 if(p&&p[0]==6&&(p[1]&4)!=0&&p[6]!=0)s.cf6C=1;
}}
