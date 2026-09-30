#include "FileReplace73A0.h"
namespace ravenloft::game {
int FileReplace73A0::run(const FileReplace73A0Ops&o,const char*src,const char*dst,const char*srcSuffix,const char*dstSuffix){
 std::string probe=std::string(src)+srcSuffix; unsigned char meta[44]{};
 if(o.stat339A2(probe.c_str())!=0)return 0;
 for(;;){
  std::string from=std::string(src)+dstSuffix+reinterpret_cast<char*>(meta+30);
  int n=o.size3127C(from.c_str()),in=o.openRead312A0(from.c_str());if(in==-1)return 1;
  std::vector<std::uint8_t>b(std::size_t(n));o.read312EC(b.data(),n,in);o.close312DC(in);
  std::string to=std::string(dst)+dstSuffix+reinterpret_cast<char*>(meta+30);
  int t=o.create31380(to.c_str());o.close312DC(t);int out=o.openWrite312B4(to.c_str());if(out==-1)return 1;
  int wrote=o.write31304(b.data(),n,out);o.close312DC(out);if(wrote!=n)return 1;
  if(o.statBuffer339CF(meta)!=0)return 0;
 }
}}
