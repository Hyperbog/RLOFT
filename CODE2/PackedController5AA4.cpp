#include "PackedController5AA4.h"
namespace ravenloft::game {
int PackedController5AA4::run(const PackedController5AA4Ops&o,int object,int selected,int a,int b,int c,int d,int e){
 std::vector<void*> p(6);for(auto&v:p)v=o.alloc3104C(0x14);
 o.draw518C(object,a,b,selected,c,d,e);
 int choice=0;
 for(;;){
  while(o.input1A1F4()==0){
   const auto x=o.mouseX9E31(),y=o.mouseY9E33();
   if(y==0x13F||x==0x18F){o.show5984(b,a);continue;}
   o.hide5A14(b);
  }
  o.hide5A14(b);
  const int x=o.mouseX9E31(),y=o.mouseY9E33();
  if(x<0x0F){
   const int q=(y-5)/0x42,rem=(y-5)%0x42;
   if(rem>=0&&rem<0x30&&q!=selected){
    if(q==4){choice=q;break;}
    if(o.select50DC(object,q)>=0){choice=q;break;}
   }
  }
  o.hit5698(object,x,y,b,c,d,e);
  while(o.input1A1F4()!=0){}
 }
 o.cleanup5578();for(auto v:p)o.free310B0(v);return choice;
}
}
