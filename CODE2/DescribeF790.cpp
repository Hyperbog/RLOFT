#include <cstdint>
#include <functional>
#include <string>
namespace ravenloft::game {
struct DescribeF790Ops{
 std::function<std::string(int)> itemName,numberText;std::function<std::uint8_t(int,int)> itemByte;std::function<std::int8_t(int,int)> itemSByte;
 std::function<int(int)> weightF5F8;std::function<void(const std::string&)> layoutF66C;std::function<void(int)> seed30CC4;std::function<std::uint16_t()> seed30700;
};
void describeF790(const DescribeF790Ops&o,int item,bool detailed,bool alternate){
 if(alternate){o.layoutF66C("");return;}if(item==-1)return;
 std::string s=o.itemName(item);
 if(o.itemByte(item,0x37)&&detailed)s+=" "+o.numberText(o.itemByte(item,0x37));
 const int type=o.itemByte(item,0x2f);
 // 0xF768 is a ten-entry dispatch table for item types 2..11.  The branches
 // append the original numeric/name fragments before the common weight tail.
 switch(type){
  case 2:case 3:case 4:{int a=o.itemByte(item,0x38),b=o.itemByte(item,0x39),c=o.itemByte(item,0x3a);s+=" "+o.numberText(a+c)+" "+o.numberText(a*b+c);break;}
  case 5:case 6:case 7:case 8:case 9:case 10:case 11:s+=" "+o.numberText(-o.itemSByte(item,0x34));break;
  default:break;
 }
 int weight=o.itemByte(item,0x30);if(type==1)weight+=o.weightF5F8(static_cast<std::int8_t>(o.itemByte(item,0x31)));
 s+=" "+o.numberText(weight/2);if(weight&1)s+=".5";
 if(type==1&&o.itemByte(item,0x31)!=0x7f)s+=" "+o.numberText(o.itemByte(item,0x31));
 o.seed30CC4(item);o.layoutF66C(s);o.seed30CC4(o.seed30700());
}
}
