#include "rloft_native/native_type7_scene_shape.hpp"
#include "rloft_native/native_lzss.hpp"
#include "rloft_native/native_sha256.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft_native {
namespace {
void require(bool good, const char* why) {
    if(!good) throw std::runtime_error(why);
}
std::uint16_t word(const std::vector<std::uint8_t>& bytes,
                   std::size_t pos) {
    require(pos<=bytes.size() && bytes.size()-pos>=2,
            "EH7/INIT original 16-bit word outside source");
    return static_cast<std::uint16_t>(
        std::uint16_t(bytes[pos]) | (std::uint16_t(bytes[pos+1])<<8U));
}
std::int16_t signed_word(const std::vector<std::uint8_t>& bytes,
                         std::size_t pos) {
    const auto raw=word(bytes,pos);
    return raw<0x8000U?static_cast<std::int16_t>(raw):
        static_cast<std::int16_t>(static_cast<std::int32_t>(raw)-65536);
}
NativeSceneSourceDescriptor descriptor(
    const std::vector<std::uint8_t>& bytes,std::size_t pos) {
    require(pos<=bytes.size() && bytes.size()-pos>=7,
            "original INIT scene descriptor truncated");
    return {word(bytes,pos),word(bytes,pos+2U),bytes[pos+4U],
            bytes[pos+5U],bytes[pos+6U]};
}
void audit_class_table(const std::vector<NativeSceneSourceDescriptor>& table,
                       std::size_t bank_size,unsigned source_class) {
    std::size_t maximum=0;
    for(const auto& desc:table) {
        require(desc.variant_count!=0,
                "original INIT scene class variant count zero");
        const auto end=std::size_t(desc.shape_base_local)+
                       std::size_t(desc.shape_stride)*desc.variant_count;
        require(end<=bank_size,
                "original INIT class shape allocation exceeds source bank");
        maximum=std::max(maximum,end);
    }
    // Every archived source-class descriptor table exactly reaches its
    // following bank boundary; this is evidenced for original INIT.
    require(maximum==bank_size && (source_class==1 || source_class==3),
            "original INIT scene source allocation does not close bank");
}
}
NativeType7SceneShape decode_native_type7_scene_shape(
    const std::vector<std::uint8_t>& entry) {
    require(entry.size()>=8U && entry[0]=='E' && entry[1]=='H' &&
            entry[2]==7 && entry[3]==0 && entry[4]==0,
            "RES3 scene record is not EH type-7 LZSS source");
    const auto decoded_bytes=word(entry,5U);
    require(decoded_bytes>=12U,
            "original EH7 compressed scene body too small");
    const auto decoded=native_lzss4k_decode(
        entry.data()+7U,entry.size()-7U,decoded_bytes);
    require(decoded.consumed==entry.size()-7U,
            "original EH7 compressed stream has trailing input bytes");
    const auto& body=decoded.bytes;
    NativeType7SceneShape scene;
    scene.original_entry_sha256=native_sha256_hex(entry);
    scene.decoded_body_sha256=native_sha256_hex(body);
    scene.source_width=word(body,0U);
    scene.x_min=signed_word(body,2U);
    scene.x_max=signed_word(body,4U);
    scene.y_min=signed_word(body,6U);
    scene.y_max=signed_word(body,8U);
    scene.decoded_body_bytes=body.size();
    require(scene.source_width>0 && scene.source_width<=128U &&
            (scene.source_width%2U)==0 &&
            scene.x_min>=0 && scene.x_min<=scene.x_max &&
            scene.x_max<scene.source_width &&
            scene.y_min>=0 && scene.y_min<=scene.y_max &&
            scene.y_max<=128,
            "original EH7 signed geometric bounds invalid");
    const auto cols=std::size_t(scene.x_max-scene.x_min)+1U;
    const auto table_end=10U+2U*cols;
    require(table_end<=body.size(),
            "original EH7 scene column-pointer table truncated");
    scene.columns.reserve(cols);
    std::set<std::size_t> unique_offsets;
    for(std::size_t c=0;c<cols;++c) {
        const auto offset=word(body,10U+2U*c);
        require(std::size_t(offset)>=table_end &&
                std::size_t(offset)<body.size(),
                "EH7 scene list pointer outside decoded body");
        NativeType7Column column;
        column.original_list_offset=offset;
        std::size_t cursor=offset;
        while(true) {
            require(cursor<=body.size() && body.size()-cursor>=2,
                    "EH7 scene list missing zero terminator");
            const auto stop=word(body,cursor);
            if(stop==0) break;
            require(cursor<=body.size() && body.size()-cursor>=6,
                    "EH7 six-byte scene raster descriptor truncated");
            const auto adjustment=signed_word(body,cursor+2U);
            const auto entry_selector=word(body,cursor+4U);
            require(stop<=0x100 && entry_selector<=0x100 &&
                    (stop&1U)==0 && (entry_selector&1U)==0,
                    "EH7 renderer microcode selector out of bounds/odd");
            column.descriptors.push_back(
                {stop,adjustment,entry_selector});
            cursor+=6U;
        }
        unique_offsets.insert(offset);
        scene.total_descriptors+=column.descriptors.size();
        scene.columns.push_back(std::move(column));
    }
    scene.unique_lists=unique_offsets.size();
    scene.structure_verified=true;
    return scene;
}

bool NativeRes3SceneBanks::contains(unsigned cls,
                                    std::size_t local) const noexcept {
    return cls<4 && local<lengths[cls];
}
std::size_t NativeRes3SceneBanks::absolute_id(
    unsigned cls,std::size_t local) const {
    if(!contains(cls,local))
        throw std::out_of_range("RES3 original source class/local shape");
    return std::size_t(starts[cls])+local;
}

NativeRes3SceneBanks decode_native_res3_scene_banks(
    const std::vector<std::uint8_t>& init,std::size_t entry_count) {
    require(init.size()>=0x24U,
            "original INIT scene mapping header too short");
    require(entry_count>0 && entry_count<=65536U,
            "original RES3 scene entry count outside bound");
    NativeRes3SceneBanks banks;
    banks.res3_entries=entry_count;
    for(unsigned i=0;i<4;++i)
        banks.starts[i]=word(init,0x06U+2U*i);
    require(banks.starts[0]==0 &&
            banks.starts[0]<banks.starts[1] &&
            banks.starts[1]<banks.starts[2] &&
            banks.starts[2]<banks.starts[3] &&
            banks.starts[3]<entry_count,
            "original INIT source-class bank starts invalid");
    for(unsigned i=0;i<4;++i) {
        const auto end=i==3?entry_count:std::size_t(banks.starts[i+1]);
        banks.lengths[i]=end-banks.starts[i];
    }
    std::size_t pos=0x24U;
    for(unsigned which=0;which<2;++which) {
        const auto count=word(init,pos);
        pos+=2U;
        require(count==(which==0?175U:12U),
                "original INIT source class descriptor count unexpected");
        require(pos<=init.size() &&
                count <= (init.size()-pos)/7U,
                "original INIT class descriptor table truncated");
        auto& records=banks.init_descriptors[which];
        records.reserve(count);
        for(unsigned i=0;i<count;++i) {
            records.push_back(descriptor(init,pos));
            pos+=7U;
        }
        audit_class_table(records,banks.lengths[which==0?1:3],
                          which==0?1:3);
    }
    return banks;
}

std::size_t NativeSceneDefinitionDirectory::npc_shape_base(
    unsigned type_id) const {
    if(type_id>=npc_definitions.size())
        throw std::out_of_range("original NPC definition type id out of range");
    return npc_definitions[type_id].absolute_shape_base;
}
std::size_t NativeSceneDefinitionDirectory::object_shape_base(
    unsigned type_id) const {
    if(type_id>=object_definitions.size())
        throw std::out_of_range("original OBJECTS definition type id out of range");
    return object_definitions[type_id].absolute_shape_base;
}
NativeSceneSourceDescriptor NativeSceneDefinitionDirectory::init_scene_descriptor(
    unsigned source_class,unsigned type_id) const {
    if(source_class!=1 && source_class!=3)
        throw std::out_of_range("original INIT scene descriptor class must be 1 or 3");
    const auto& table=banks.init_descriptors[source_class==1?0:1];
    if(type_id>=table.size())
        throw std::out_of_range("original INIT scene descriptor type id out of range");
    return table[type_id];
}
std::size_t NativeSceneDefinitionDirectory::init_base_variant(
    unsigned source_class,unsigned type_id,unsigned variant_index) const {
    const auto desc=init_scene_descriptor(source_class,type_id);
    if(variant_index>=desc.variant_count)
        throw std::out_of_range("original INIT scene variant index out of range");
    const auto local=std::size_t(desc.shape_base_local)+
                     std::size_t(desc.shape_stride)*variant_index;
    return banks.absolute_id(source_class,local);
}

NativeSceneDefinitionDirectory decode_native_scene_definitions(
    const NativeRes3SceneBanks& banks,
    const std::vector<std::uint8_t>& npcs,
    const std::vector<std::uint8_t>& objects) {
    auto full_count=[](const std::vector<std::uint8_t>& bytes,
                       unsigned stride,unsigned expected,
                       const char* label) {
        require(bytes.size()>=4,"native source definition table truncated");
        const auto count=std::size_t(bytes[0])|
                         (std::size_t(bytes[1])<<8U)|
                         (std::size_t(bytes[2])<<16U)|
                         (std::size_t(bytes[3])<<24U);
        require(count==expected && bytes.size()==4U+count*stride,label);
        return count;
    };
    const auto npc_count=full_count(
        npcs,0x45,83,"original NPCS definition count/stride invalid");
    const auto object_count=full_count(
        objects,0x41,336,"original OBJECTS definition count/stride invalid");
    NativeSceneDefinitionDirectory result;
    result.banks=banks;
    result.npc_definitions.reserve(npc_count);
    result.object_definitions.reserve(object_count);
    for(std::size_t id=0;id<npc_count;++id) {
        const auto local=word(npcs,4U+id*0x45U+0x14U);
        require(banks.contains(0,local),
                "NPC definition local base exceeds original RES3 class zero");
        result.npc_definitions.push_back({
            local,banks.absolute_id(0,local)});
        result.max_npc_local_base=std::max(
            result.max_npc_local_base,std::size_t(local));
    }
    for(std::size_t id=0;id<object_count;++id) {
        const auto at=4U+id*0x41U;
        const auto local=word(objects,at+0x28U);
        const auto stride=objects[at+0x2AU];
        // The original source permits an unused zero+zero placeholder.
        require((banks.contains(2,local) ||
                 (local==0 && stride==0)) &&
                std::size_t(local)+stride<=banks.lengths[2],
                "OBJECTS shape base/stride exceeds original RES3 class two");
        result.object_definitions.push_back({
            local,stride,banks.absolute_id(2,local)});
        result.max_object_allocation_end=std::max(
            result.max_object_allocation_end,std::size_t(local)+stride);
    }
    require(result.max_object_allocation_end==banks.lengths[2],
            "original OBJECTS shapes do not close class-two RES3 bank");
    return result;
}

NativeRes3SceneCorpus audit_native_res3_scene_corpus(
    const std::vector<std::uint8_t>& init,
    const NativeResArchive& archive) {
    NativeRes3SceneCorpus corpus;
    corpus.banks=decode_native_res3_scene_banks(init,archive.entry_count());
    corpus.shapes.reserve(archive.entry_count());
    for(std::size_t id=0;id<archive.entry_count();++id) {
        const auto shape=decode_native_type7_scene_shape(archive.entry(id));
        require(shape.structure_verified,"original RES3 shape was not validated");
        ++corpus.type7_shapes;
        corpus.raster_descriptors+=shape.total_descriptors;
        corpus.unique_lists+=shape.unique_lists;
        corpus.decoded_body_total_bytes+=shape.decoded_body_bytes;
        std::size_t unique_descriptors=0;
        std::set<std::uint16_t> counted_offsets;
        for(const auto& col:shape.columns)
            if(counted_offsets.insert(col.original_list_offset).second)
                unique_descriptors+=col.descriptors.size();
        corpus.shapes.push_back({id,
            shape.original_entry_sha256,shape.decoded_body_sha256,
            shape.decoded_body_bytes,shape.source_width,shape.columns.size(),
            shape.unique_lists,shape.total_descriptors,unique_descriptors});
        switch(shape.source_width) {
            case 64:++corpus.width_histogram[0];break;
            case 96:++corpus.width_histogram[1];break;
            case 128:++corpus.width_histogram[2];break;
            default:throw std::runtime_error(
                "original RES3 EH7 shape has unknown source width");
        }
        for(unsigned cls=0;cls<4;++cls) {
            if(id>=corpus.banks.starts[cls] &&
               id<corpus.banks.starts[cls]+corpus.banks.lengths[cls]) {
                ++corpus.source_class_shapes[cls];
                break;
            }
        }
    }
    require(corpus.type7_shapes==archive.entry_count(),
            "original RES3 scene shapes incomplete");
    return corpus;
}

std::string native_res3_scene_corpus_json(
    const NativeRes3SceneCorpus& v) {
    std::ostringstream out;
    out<<"{\"schema\":\"rloft-native-res3-type7-shapes/v1\","
       <<"\"res3_entries\":"<<v.banks.res3_entries
       <<",\"decoded_eh7_shapes\":"<<v.type7_shapes
       <<",\"unique_column_lists\":"<<v.unique_lists
       <<",\"raster_descriptors\":"<<v.raster_descriptors
       <<",\"total_decoded_body_bytes\":"<<v.decoded_body_total_bytes
       <<",\"width_64\":"<<v.width_histogram[0]
       <<",\"width_96\":"<<v.width_histogram[1]
       <<",\"width_128\":"<<v.width_histogram[2]
       <<",\"source_class_lengths\":[";
    for(std::size_t i=0;i<4;++i) {
        if(i)out<<",";
        out<<v.source_class_shapes[i];
    }
    out<<"],\"sintab_x86_code_executed\":false,"
       <<"\"pixel_color_parity_verified\":false,"
       <<"\"original_scene_frame_parity_verified\":false,"
       <<"\"event_vm_executed\":false,\"shapes\":[";
    for(std::size_t i=0;i<v.shapes.size();++i) {
        const auto& shape=v.shapes[i];
        if(i)out<<",";
        out<<"{\"id\":"<<shape.id
           <<",\"eh7_entry_sha256\":\""<<shape.eh7_entry_sha256
           <<"\",\"eh7_body_sha256\":\""<<shape.eh7_body_sha256
           <<"\",\"decoded_body_bytes\":"<<shape.decoded_body_bytes
           <<",\"source_width\":"<<shape.source_width
           <<",\"source_columns\":"<<shape.source_columns
           <<",\"unique_column_lists\":"<<shape.unique_column_lists
           <<",\"raster_descriptors_with_column_references\":"
           <<shape.raster_descriptors_with_column_references
           <<",\"raster_descriptors_unique_lists\":"
           <<shape.raster_descriptors_unique_lists<<"}";
    }
    out<<"]}\n";
    return out.str();
}
} // namespace rloft_native
