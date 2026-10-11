#include "original_le_image.h"
#include <algorithm>
#include <limits>
#include <string>

namespace rloft_recomp {
namespace {
using u8=std::uint8_t; using u32=std::uint32_t;
constexpr std::size_t kModuleStart=0x25D24;
constexpr std::size_t kLeStart=0x27BDC;
void require(bool ok, const char* detail) { if(!ok) throw std::runtime_error(detail); }
u32 read(const std::vector<u8>& b, std::size_t at, unsigned size) {
    require(size==1 || size==2 || size==4, "invalid integer size");
    require(at<=b.size() && size<=b.size()-at, "LE read outside image");
    u32 n=0; for(unsigned i=0;i<size;++i) n|=u32(b[at+i])<<(i*8u); return n;
}
void write32(std::vector<u8>& b, std::size_t at, u32 value) {
    require(at<=b.size() && 4<=b.size()-at, "LE write outside image");
    for(unsigned i=0;i<4;++i)b[at+i]=u8(value>>(8u*i));
}
std::size_t sum(std::size_t a,std::size_t b) {
    require(b<=std::numeric_limits<std::size_t>::max()-a,"LE offset overflow");return a+b;
}
} // namespace

OriginalLeImage OriginalLeImage::parse(const std::vector<u8>& file) {
    require(file.size()>kLeStart+0xB0,"missing bound LE header");
    require(read(file,kModuleStart,2)==0x5A4D,"missing application MZ");
    require(read(file,kModuleStart+0x3C,4)==kLeStart-kModuleStart,"invalid nested e_lfanew");
    require(read(file,kLeStart,2)==0x454C,"missing LE signature");
    require(read(file,kLeStart+2,2)==0,"unsupported LE byte order");
    auto header=[&](std::size_t offset){return read(file,kLeStart+offset,4);};
    OriginalLeImage result;
    const u32 n_pages=header(0x14), n_objects=header(0x44);
    const u32 obj_table=header(0x40), page_map=header(0x48);
    const u32 fixup_pages=header(0x68), fixup_records=header(0x6C);
    const u32 fixup_size=header(0x30), data_pages=header(0x80);
    const u32 last_page=header(0x2C);
    result.entry_object=header(0x18);result.entry_offset=header(0x1C);
    result.stack_object=header(0x20);result.stack_offset=header(0x24);
    result.page_size=header(0x28);
    require(result.page_size==4096,"unsupported LE page size");
    require(n_objects==2 && n_pages>0 && n_pages<=4096,"unsupported LE object/page count");
    require(last_page>0 && last_page<=result.page_size,"invalid final LE page length");
    require(fixup_pages<=fixup_records && fixup_records<fixup_pages+fixup_size,
            "invalid fixup table layout");
    require(sum(kLeStart,sum(obj_table,std::size_t(n_objects)*24))<=file.size(),"truncated object table");
    require(sum(kLeStart,sum(page_map,std::size_t(n_pages)*4))<=file.size(),"truncated page map");
    require(sum(kLeStart,sum(fixup_pages,std::size_t(n_pages+1)*4))<=file.size(),"truncated fixup page map");
    const std::size_t record_base=sum(kLeStart,fixup_records);
    const std::size_t record_end=sum(kLeStart,sum(fixup_pages,fixup_size));
    require(record_end<=file.size(),"truncated fixup section");
    const std::size_t data_base=sum(kModuleStart,data_pages);
    require(data_base<=file.size(),"invalid LE data page base");
    result.objects.resize(n_objects);
    std::vector<std::size_t> page_owner(n_pages), page_offset(n_pages);
    std::vector<bool> claimed(n_pages,false);
    for(u32 obj_index=0;obj_index<n_objects;++obj_index) {
        auto& obj=result.objects[obj_index];
        const std::size_t p=kLeStart+obj_table+obj_index*24u;
        obj.virtual_size=read(file,p,4);obj.base=read(file,p+4,4);
        obj.flags=read(file,p+8,4);obj.first_page=read(file,p+12,4);
        obj.page_count=read(file,p+16,4);
        require(obj.virtual_size>0 && obj.virtual_size<=0x1000000,"unsupported object virtual size");
        require(obj.page_count>0 && obj.first_page>0 &&
                obj.first_page<=n_pages && obj.page_count<=n_pages-obj.first_page+1,
                "invalid object page interval");
        require(std::uint64_t(obj.page_count-1)*result.page_size<obj.virtual_size+result.page_size,
                "object pages exceed virtual bounds");
        obj.raw.assign(obj.virtual_size,0);obj.relocated.assign(obj.virtual_size,0);
        for(u32 j=0;j<obj.page_count;++j) {
            const u32 page=obj.first_page+j-1;
            require(!claimed[page],"overlapping object page ownership");
            claimed[page]=true; page_owner[page]=obj_index;
            page_offset[page]=std::size_t(j)*result.page_size;
            const std::size_t m=kLeStart+page_map+std::size_t(page)*4;
            const u32 physical_page=(read(file,m,1)<<16)|(read(file,m+1,1)<<8)|read(file,m+2,1);
            require(read(file,m+3,1)==0 && physical_page>=1 && physical_page<=n_pages,
                    "unsupported LE page-map mode");
            const std::size_t available=(page==n_pages-1)?last_page:result.page_size;
            const std::size_t physical=sum(data_base,std::size_t(physical_page-1)*result.page_size);
            require(physical<=file.size() && available<=file.size()-physical,
                    "truncated LE data page");
            const std::size_t count=std::min<std::size_t>(available,obj.raw.size()-page_offset[page]);
            std::copy_n(file.begin()+physical,count,obj.raw.begin()+page_offset[page]);
        }
    }
    for(const bool c:claimed)require(c,"orphan LE page");
    for(auto& obj:result.objects)obj.relocated=obj.raw;
    require(result.entry_object>=1 && result.entry_object<=n_objects &&
            result.entry_offset<result.objects[result.entry_object-1].virtual_size,
            "invalid LE entrypoint");
    require(result.stack_object>=1 && result.stack_object<=n_objects &&
            result.stack_offset<=result.objects[result.stack_object-1].virtual_size,
            "invalid LE stack endpoint");
    for(u32 page=0;page<n_pages;++page) {
        const u32 begin=read(file,kLeStart+fixup_pages+std::size_t(page)*4,4);
        const u32 end=read(file,kLeStart+fixup_pages+std::size_t(page+1)*4,4);
        require(begin<=end && end<=record_end-record_base,"out-of-range LE page fixups");
        std::size_t cursor=sum(record_base,begin),stop=sum(record_base,end);
        while(cursor<stop) {
            require(stop-cursor>=7,"truncated LE fixup record");
            const u8 kind=file[cursor],flags=file[cursor+1];
            require(kind==0x07 && (flags==0 || flags==0x10),
                    "unsupported LE fixup type; refusing partial materialization");
            const std::int16_t displacement=static_cast<std::int16_t>(read(file,cursor+2,2));
            const u8 target_obj=file[cursor+4];
            const unsigned width=flags==0x10?4u:2u;
            require(stop-cursor>=5+width,"truncated target fixup");
            const u32 target=read(file,cursor+5,width);cursor+=5+width;
            require(target_obj>=1 && target_obj<=n_objects,"invalid fixup target object");
            const auto& destination=result.objects[target_obj-1];
            require(target<destination.virtual_size,"fixup target beyond object");
            std::int64_t source_at=std::int64_t(page_offset[page])+displacement;
            // The original files use signed -1/-2/-3 on a new page to patch a
            // 32-bit value beginning on the preceding page. It belongs to that object.
            require(source_at>=0,"fixup crosses before object beginning");
            auto& source=result.objects[page_owner[page]];
            require(std::uint64_t(source_at)+4<=source.raw.size(),"fixup crosses object boundary");
            require(read(source.raw,std::size_t(source_at),4)==target,
                    "stored LE target and fixup record disagree");
            require(destination.base<=std::numeric_limits<u32>::max()-target,
                    "relocated 32-bit target overflows");
            write32(source.relocated,std::size_t(source_at),destination.base+target);
            ++result.applied_fixups;
            if(displacement<0)++result.backward_page_fixups;
        }
        require(cursor==stop,"LE fixup record parse did not exhaust page");
    }
    return result;
}
} // namespace rloft_recomp
