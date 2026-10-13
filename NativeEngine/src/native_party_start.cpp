#include "rloft_native/native_party_start.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft_native {
namespace {
std::uint16_t word(const NativeCharsImage& image,std::size_t at) {
    return std::uint16_t(image.at(at)) |
           (std::uint16_t(image.at(at+1U))<<8U);
}
bool allowed_name_byte(std::uint8_t c) noexcept {
    return (c>='A'&&c<='Z')||(c>='0'&&c<='9')||c==' ';
}
} // namespace

NativePartyStart decode_native_party_start(const NativeCharsImage& image) {
    NativePartyStart result;
    result.chars=image;
    constexpr std::size_t kRecord=0x248U;
    for(std::size_t i=0;i<result.actors.size();++i) {
        auto& actor=result.actors[i];
        const auto base=i*kRecord;
        // Original CODE.1 function_3138 sets accepted status to zero.
        if(word(image,base+0x10U)!=0U)
            throw std::runtime_error("native CHARS actor is not accepted");
        bool terminated=false;
        for(std::size_t n=0;n<16U;++n) {
            const auto ch=image.at(base+n);
            if(ch==0U){terminated=true;break;}
            if(!allowed_name_byte(ch))
                throw std::runtime_error("native CHARS name is not original CODE.1 ASCII");
            actor.name.push_back(char(ch));
        }
        if(!terminated)
            throw std::runtime_error("native CHARS name is not terminated in 16 bytes");
        actor.portrait=image.at(base+0x12U);
        actor.sex=image.at(base+0x13U);
        actor.race=image.at(base+0x14U);
        actor.alignment=image.at(base+0x1AU);
        if(actor.portrait>16U || actor.sex>1U || actor.race>5U ||
           actor.alignment>5U)
            throw std::runtime_error("native CHARS actor choice is outside original rules");
        for(std::size_t k=0;k<3U;++k) {
            const auto encoded=image.at(base+0x15U+k);
            if(encoded!=0xFFU&&encoded>5U)
                throw std::runtime_error("native CHARS class is invalid");
            actor.classes[k]=encoded==0xFFU?-1:int(encoded);
        }
        if(actor.classes[0]<0)
            throw std::runtime_error("native CHARS actor lacks primary class");
        // CODE.1 4EF0 stores current and max hit points as signed words.
        actor.current_hp=std::int16_t(word(image,base+0x1CU));
        actor.max_hp=std::int16_t(word(image,base+0x1EU));
        if(actor.current_hp!=actor.max_hp)
            throw std::runtime_error("native new-party CHARS hit points diverge");
    }
    return result;
}
} // namespace rloft_native
