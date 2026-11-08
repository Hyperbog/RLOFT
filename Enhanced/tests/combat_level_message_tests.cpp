#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_level_message.hpp"
#include "rloft/combat_irq_message.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <vector>

namespace {
std::vector<std::uint8_t> synthetic_code2() {
    std::vector<std::uint8_t> result(0x75F24U+0x67E8U,0U);
    result[0x75F24U+0x67E4U]=0x48U;
    result[0x75F24U+0x67E5U]=0x05U;
    constexpr char format[]="%s gains a level.";
    for(std::size_t i=0U;i<sizeof(format);++i)
        result[0x75F24U+0x548U+i]=
            static_cast<std::uint8_t>(format[i]);
    return result;
}
} // namespace

int main() {
    using rloft::CombatLevelMessageModel;
    using rloft::CombatIrqMessageModel;
    auto code2=synthetic_code2();
    rloft::PartyImage party{};
    party.bytes[0]='A';
    party.bytes[1]='L';
    party.bytes[2]='I';
    party.bytes[3]='N';
    party.bytes[4]='A';
    party.bytes[5]=0U;
    std::vector<std::uint8_t> dest(50U,0xD3U);
    const auto check=[&](std::size_t actor) {
        return CombatLevelMessageModel::inspect(party,actor,code2,dest);
    };
    auto result=check(0U);
    assert(result);
    constexpr char expected[]="ALINA gains a level.";
    assert(result->formatted_byte_count==sizeof(expected)-1U);
    for(std::size_t i=0;i<sizeof(expected);++i)
        assert(result->formatted_c_string[i]==
            static_cast<std::uint8_t>(expected[i]));
    assert(result->projected_destination[sizeof(expected)]==0xD3U);
    assert(dest[0]==0xD3U && party.bytes[0]=='A');

    // The level-up formatter writes DGROUP 759C, then the real
    // DDA4 path queues that exact C-string through 1C21C.
    rloft::CombatIrqMessageSnapshot queue{};
    queue.original_irq_tick=0x10000U;
    queue.source_c_string=result->formatted_c_string;
    queue.destination_buffer.assign(80U,0xAEU);
    const auto delivered=CombatIrqMessageModel::queue(queue);
    assert(delivered);
    assert(delivered->deadline_cf54==0x1005AU);
    assert(delivered->bytes_copied==sizeof(expected));
    for(std::size_t i=0;i<sizeof(expected);++i)
        assert(delivered->destination_buffer[i]==
            static_cast<std::uint8_t>(expected[i]));
    assert(delivered->destination_buffer[sizeof(expected)]==0xAEU);

    // Exact host bounds. Original sprintf was unbounded; no
    // invented truncation should masquerade as a DOS result.
    dest.resize(sizeof(expected)-1U);
    assert(!check(0U));
    dest.resize(sizeof(expected));
    assert(check(0U));

    // Any single-byte corruption in format or serialized pointer
    // means the game version is unsupported rather than defaulting.
    code2[0x75F24U+0x548U+3U]^=0x01U;
    assert(!check(0U));
    code2=synthetic_code2();
    code2[0x75F24U+0x67E5U]=0U;
    assert(!check(0U));
    code2=synthetic_code2();
    assert(!check(4U));

    // Original 16-byte name must terminate before another field.
    for(std::size_t i=0U;i<16U;++i)
        party.bytes[i]='X';
    assert(!check(0U));

    // DOS text bytes pass through with no locale or UTF-8 coercion.
    party.bytes.fill(0U);
    party.bytes[0]=0xE9U;
    result=check(0U);
    assert(result && result->formatted_c_string[0]==0xE9U);

    // DDA4 actor-index argument is a character record pointer
    // determined from actor stride 248h, not the party's first slot.
    const auto actor3=3U*rloft::PartyImage::kActorSize;
    party.bytes[actor3]='T';
    party.bytes[actor3+1U]=0U;
    result=check(3U);
    assert(result && result->formatted_c_string[0]=='T');
    assert(result->formatted_c_string[1]==' ');

    // Original game resource verification, never checked-in assets.
    const auto* owned=std::getenv("RLOFT_TEST_CODE2");
    if(owned!=nullptr && *owned!='\0') {
        std::ifstream file(owned,std::ios::binary);
        assert(file.good());
        const std::vector<std::uint8_t> original(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        const auto old=code2;
        code2=original;
        result=check(3U);
        assert(result && result->formatted_c_string[0]=='T');
        code2=old;
    }
}
