// Source-media-free boundary test for the actual CODE.2 31334 host adapter.
#include "game/resources/ResourceCache.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace {
using B=std::uint8_t;
void check(bool yes,const char* reason) {if(!yes)throw std::runtime_error(reason);}
void le16(std::vector<B>& b,std::size_t i,unsigned x) {
    b[i]=static_cast<B>(x);b[i+1u]=static_cast<B>(x>>8u);
}
void le32(std::vector<B>& b,std::size_t i,unsigned x) {
    le16(b,i,x);le16(b,i+2u,x>>16u);
}
}
int main() {
    try {
        constexpr unsigned start1=12u,start2=28u;
        std::vector<B> file(start2+5u+136u+1u,0u);
        le32(file,0u,2u);le32(file,4u,start1);le32(file,8u,start2);
        file[start1]='E';file[start1+1u]='H';file[start1+2u]=1u;
        le16(file,start1+3u,8u);le16(file,start1+5u,1u);
        le16(file,start1+7u,1u);file[start1+9u]=1u;
        file[start2]='E';file[start2+1u]='H';file[start2+2u]=2u;
        le16(file,start2+3u,136u);
        const auto descriptor=start2+5u+128u;
        le16(file,descriptor,128u);le16(file,descriptor+2u,11u);
        file[descriptor+4u]=1u;file[descriptor+6u]=3u;
        rloft::resource_cache::Record r{};
        using namespace rloft::resource_cache;
        check(locate(file,0u,r) && r.start==start1 && r.end==start2 &&
              r.type==1u && authenticate(file,r,0u),"Original type-1 RES record");
        check(locate(file,1u,r) && r.start==start2 && r.type==2u &&
              authenticate(file,r,0u) && !authenticate(file,r,1u),
              "Original RES0 type-2 font and archive owner");
        check(!locate(file,2u,r),"Out-of-range resource index admitted");
        file[descriptor+6u]=0u;
        check(locate(file,1u,r) && !authenticate(file,r,0u),
              "Invalid original font compression accepted");
        file[descriptor+6u]=3u;
        file[4u]=0u;
        check(!locate(file,0u,r),"Corrupt directory offset admitted");
        std::cout<<"RLOFT_RESOURCE_CACHE_31334_INDEX_BOUNDARY_PASS\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"RLOFT_RESOURCE_CACHE_31334_INDEX_BOUNDARY_FAIL "<<e.what()<<"\n";
        return 1;
    }
}
