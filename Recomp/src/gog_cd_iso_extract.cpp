// Read-only GOG game.gog ISO9660 MODE1/MODE2-FORM1 extractor.
// Exports only the owner's RES9 and RES10 to a user-private scratch folder.
// No original game/CD bytes are embedded or redistributed.
#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
namespace fs=std::filesystem;
using u8=std::uint8_t;
using u32=std::uint32_t;
struct Disc {
    std::ifstream f;
    std::uint64_t length=0;
    unsigned stride=0,header=0;
    explicit Disc(const fs::path& path):f(path,std::ios::binary|std::ios::ate) {
        if(!f)throw std::runtime_error("Cannot open original game.gog image");
        const auto n=f.tellg();
        if(n<=0)throw std::runtime_error("CD image is empty");
        length=static_cast<std::uint64_t>(n);
        for(const auto x: {std::array<unsigned,2>{2048,0},
                           std::array<unsigned,2>{2352,16},
                           std::array<unsigned,2>{2352,24}}){
            stride=x[0];header=x[1];
            const auto sec=sector(16,true);
            if(sec[0]==1 && std::string(sec.begin()+1,sec.begin()+6)=="CD001" &&
               sec[6]==1)return;
        }
        throw std::runtime_error(
            "game.gog is not a supported ISO9660 2048-byte or 2352-byte disc image");
    }
    std::array<char,2048> sector(u32 lba,bool probe=false){
        std::array<char,2048> b{};
        const auto off=std::uint64_t(lba)*stride+header;
        if(off>length||length-off<2048) {
            if(probe)return b;
            throw std::runtime_error("Disc extent outside game.gog");
        }
        f.clear();
        f.seekg(static_cast<std::streamoff>(off),std::ios::beg);
        f.read(b.data(),b.size());
        if(!f)throw std::runtime_error("Truncated disc sector");
        return b;
    }
};
u32 le32(const char* p) noexcept{
    const auto* q=reinterpret_cast<const u8*>(p);
    return u32(q[0])|(u32(q[1])<<8)|(u32(q[2])<<16)|(u32(q[3])<<24);
}
struct Item{u32 lba=0,size=0;bool directory=false;std::string name;};
Item record(const char* p,unsigned limit){
    if(limit<34)throw std::runtime_error("Truncated ISO directory record");
    const unsigned n=static_cast<u8>(p[32]);
    if(33u+n>limit)throw std::runtime_error("Invalid ISO file name length");
    Item v{le32(p+2),le32(p+10),(static_cast<u8>(p[25])&2u)!=0,{}};
    for(unsigned i=0;i<n;++i){
        const auto ch=static_cast<u8>(p[33+i]);
        if((n==1 && ch<=1u))return {};
        if(ch==';')break;
        if(ch<32u||ch>126u)throw std::runtime_error("Unsupported ISO filename");
        v.name+=static_cast<char>((ch>='a'&&ch<='z')?ch-32:ch);
    }
    return v;
}
std::vector<Item> entries(Disc& d,const Item& dir) {
    if(dir.size==0 || dir.size>8u*1024u*1024u ||
       !dir.directory)throw std::runtime_error("Invalid ISO directory extent");
    std::vector<Item> children;
    const auto sectors=(std::uint64_t(dir.size)+2047u)/2048u;
    for(std::uint64_t i=0;i<sectors;++i){
        if(std::uint64_t(dir.lba)+i>UINT32_MAX)
            throw std::runtime_error("ISO directory LBA overflow");
        const auto sec=d.sector(dir.lba+static_cast<u32>(i));
        unsigned off=0;
        while(off<2048){
            const unsigned n=static_cast<u8>(sec[off]);
            if(!n)break;  // sector padding
            if(n>2048u-off)throw std::runtime_error("ISO directory record crosses sector");
            const auto ent=record(sec.data()+off,n);
            if(!ent.name.empty())children.push_back(ent);
            if(children.size()>4096)throw std::runtime_error("ISO directory entry bound exceeded");
            off+=n;
        }
    }
    return children;
}
void export_entry(Disc& d,const Item& item,const fs::path& out){
    if(item.directory||item.size==0u||item.size>256u*1024u*1024u)
        throw std::runtime_error("Invalid original RES CD archive size");
    if(fs::exists(out))throw std::runtime_error("Refusing to overwrite CD extraction");
    std::ofstream dst(out,std::ios::binary|std::ios::trunc);
    if(!dst)throw std::runtime_error("Cannot create private CD output");
    std::uint64_t remaining=item.size;
    for(std::uint64_t i=0;remaining;i++){
        if(std::uint64_t(item.lba)+i>UINT32_MAX)
            throw std::runtime_error("ISO file sector overflow");
        const auto sec=d.sector(item.lba+static_cast<u32>(i));
        const auto take=static_cast<std::streamsize>(std::min<std::uint64_t>(remaining,2048u));
        dst.write(sec.data(),take);
        if(!dst)throw std::runtime_error("Writing private archive failed");
        remaining-=static_cast<std::uint64_t>(take);
    }
    dst.close();
    if(fs::file_size(out)!=item.size)throw std::runtime_error("CD output size mismatch");
}
int main(int argc,char** argv) {
    try{
        if(argc!=5 || std::string(argv[1])!="--image" ||
           std::string(argv[3])!="--out")
            throw std::invalid_argument("usage: rloft_gog_cd_extract --image game.gog --out PRIVATE_DIR");
        Disc disc(argv[2]);
        const auto pvd=disc.sector(16);
        const Item root=record(pvd.data()+156u,static_cast<u8>(pvd[156]));
        if(!root.directory || root.lba==0)
            throw std::runtime_error("Invalid original ISO9660 root directory");
        std::vector<std::pair<Item,unsigned>> queue{{root,0}};
        std::set<u32> walked;
        std::array<Item,2> wanted{};
        std::array<bool,2> present{false,false};
        for(std::size_t cursor=0;cursor<queue.size();++cursor){
            const auto node=queue[cursor];
            if(!walked.insert(node.first.lba).second)continue;
            if(walked.size()>128)throw std::runtime_error("Excessive ISO directory nesting");
            for(const auto& child:entries(disc,node.first)){
                if(child.directory && node.second<8){
                    queue.push_back({child,node.second+1});
                }else if(!child.directory){
                    const int slot=child.name=="RES9"?0:child.name=="RES10"?1:-1;
                    if(slot>=0){
                        if(present[slot])
                            throw std::runtime_error("Multiple RES9/RES10 candidates in CD");
                        present[slot]=true;wanted[slot]=child;
                    }
                }
            }
        }
        if(!present[0]||!present[1])
            throw std::runtime_error("Could not find both RES9 and RES10 in game.gog");
        const fs::path out(argv[4]);
        fs::create_directories(out);
        if(fs::exists(out/"RES9")||fs::exists(out/"RES10"))
            throw std::runtime_error("Private RES9/RES10 destination already exists");
        // Fail closed: if second extraction fails, remove first scratch file.
        try{
            export_entry(disc,wanted[0],out/"RES9");
            export_entry(disc,wanted[1],out/"RES10");
        }catch(...){
            std::error_code ignored;
            fs::remove(out/"RES9",ignored);
            fs::remove(out/"RES10",ignored);
            throw;
        }
        std::cout<<"RLOFT_GOG_CD_RES9_RES10_PRIVATE_EXTRACTION_PASS"
                 <<" res9_bytes="<<wanted[0].size
                 <<" res10_bytes="<<wanted[1].size
                 <<" source=LOCAL_USER_CD_IMAGE\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"RLOFT_GOG_CD_EXTRACTION_FAIL: "<<e.what()<<"\n";
        return 1;
    }
}
