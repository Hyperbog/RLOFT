#include "Code1ResourceIndex8978.h"
#include <algorithm>
namespace ravenloft::game {
namespace {
std::uint16_t r16(const std::uint8_t* p) noexcept {return std::uint16_t(p[0])|(std::uint16_t(p[1])<<8);}
std::uint32_t r32(const std::uint8_t* p) noexcept {return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|(std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);}
void w16(std::uint8_t* p,std::uint16_t v) noexcept {p[0]=std::uint8_t(v);p[1]=std::uint8_t(v>>8);}
void w32(std::uint8_t* p,std::uint32_t v) noexcept {p[0]=std::uint8_t(v);p[1]=std::uint8_t(v>>8);p[2]=std::uint8_t(v>>16);p[3]=std::uint8_t(v>>24);}
std::string basePath(const Code1ResourceIndexState8978& s){auto e=std::find(s.basePath2AB1.begin(),s.basePath2AB1.end(),'\0');return {s.basePath2AB1.begin(),e};}
}
std::uint16_t Code1ResourceRecord8978::word3()const noexcept{return r16(raw.data()+3);}
void Code1ResourceRecord8978::setWord3(std::uint16_t v)noexcept{w16(raw.data()+3,v);}
std::uint32_t Code1ResourceRecord8978::dword5()const noexcept{return r32(raw.data()+5);}
void Code1ResourceRecord8978::setDword5(std::uint32_t v)noexcept{w32(raw.data()+5,v);}

void Code1ResourceIndex8978::loadInfo(Code1ResourceIndexState8978& s,const Code1ResourceIndexOps8978& o){
    auto h=o.openRead62A8("INFO.DAT");if(h==-1)o.fatalId9814(0x240);
    (void)o.read6304(s.infoDigits2AA4.data(),13,h);s.basePath2AB1.fill('\0');
    for(auto& v:s.infoDigits2AA4)v=std::uint8_t(v-0x30u);
    std::int16_t n=0;(void)o.read6304(&n,2,h);(void)o.read6304(s.basePath2AB1.data(),n,h);(void)o.close62F0(h);
}
void Code1ResourceIndex8978::openGroups(const std::string& prefix,std::int32_t count,Code1ResourceIndexState8978& s,const Code1ResourceIndexOps8978& o){
    loadInfo(s,o);s.groupTableAllocation2B08=o.heapAllocate71C8(std::size_t(count)*16u);s.groupCount2B04=count;s.groups.resize(std::size_t(count));
    for(std::int32_t g=0;g<count;++g){
        std::string name;if(s.infoDigits2AA4[std::size_t(g)]!=0){name=basePath(s);name+="\\";name+=prefix;}else name=prefix;name+=std::to_string(g);
        auto& e=s.groups[std::size_t(g)];e.handle=o.openRead62A8(name);if(e.handle==-1)o.fatalId9814(0x26C);
        (void)o.read6304(&e.count,4,e.handle);e.dword4=0;e.recordAllocation=o.heapAllocate71C8(std::size_t(e.count)*9u);
        e.records=o.allocateRecordStorage?o.allocateRecordStorage(e.count):std::vector<Code1ResourceRecord8978>(e.count);
        for(auto& rec:e.records){rec.flags0()&=0xF0u;rec.flags0()|=0x0Fu;rec.flags0()&=0xEFu;rec.loaded2()=0;rec.setWord3(0);rec.setDword5(0);}
    }
}
void Code1ResourceIndex8978::releaseAllItems(Code1ResourceIndexState8978& s,const Code1ResourceIndexOps8978& o){
    for(std::int32_t g=0;g<s.groupCount2B04;++g){auto n=s.groups[std::size_t(g)].count;for(std::uint32_t i=0;i<n;++i){o.call9488(std::int32_t(i),g);o.call91D8(std::int32_t(i),g);}}
}
void Code1ResourceIndex8978::setLoadedWord3(std::uint32_t v,Code1ResourceIndexState8978& s)noexcept{
    v>>=5;for(std::int32_t g=0;g<s.groupCount2B04;++g)for(auto& rec:s.groups[std::size_t(g)].records)if(rec.loaded2()!=0)rec.setWord3(std::uint16_t(v));
}
void Code1ResourceIndex8978::shutdown(Code1ResourceIndexState8978& s,const Code1ResourceIndexOps8978& o){
    releaseAllItems(s,o);for(std::int32_t g=0;g<s.groupCount2B04;++g){auto& e=s.groups[std::size_t(g)];if(e.handle!=0){(void)o.close62F0(e.handle);e.handle=0;e.count=0;}if(e.recordAllocation!=0){o.heapFree7240(e.recordAllocation);e.recordAllocation=0;e.records.clear();}}
    o.heapFree7240(s.groupTableAllocation2B08);s.groupTableAllocation2B08=0;s.groups.clear();
}
std::uint32_t Code1ResourceIndex8978::sumDword4(const Code1ResourceIndexState8978& s)noexcept{std::uint32_t t=0;for(std::int32_t g=0;g<s.groupCount2B04;++g)t+=s.groups[std::size_t(g)].dword4;return t;}
void Code1ResourceIndex8978::readHeader(Code1ResourceHeader8D0C& out,std::int32_t index,std::int32_t handle,std::uint32_t context,const Code1ResourceIndexOps8978& o){
    o.seekIndexed6358(index,handle);std::array<std::uint8_t,5> h{};(void)o.read6304(h.data(),5,handle);out.signature=r16(h.data());out.byte2=h[2];out.payloadLength=std::int16_t(r16(h.data()+3));
    if(out.payloadLength!=0){out.payloadAllocation=o.heapAllocate71C8(std::size_t(out.payloadLength));out.payload.resize(std::size_t(out.payloadLength));(void)o.read6304(out.payload.data(),out.payloadLength,handle);}else{out.payloadAllocation=0;out.payload.clear();}
    if(out.signature!=0x4845u){std::string m="Attempt to load non-resource file: ";m+=std::to_string(index);m+="/";m+=std::to_string(context);o.fatalText9814(m);}
}
std::int32_t Code1ResourceIndex8978::groupHandle(std::int32_t g,const Code1ResourceIndexState8978& s)noexcept{return s.groups[std::size_t(g)].handle;}
std::uint32_t Code1ResourceIndex8978::groupItemCount(std::int32_t g,const Code1ResourceIndexState8978& s)noexcept{return s.groups[std::size_t(g)].count;}
} // namespace ravenloft::game
