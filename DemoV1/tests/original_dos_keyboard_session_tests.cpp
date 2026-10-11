#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_dos_keyboard_session.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    using rloft::OriginalDosKeyboardSession;
    // Test the original CODE.2 storage seam independently of private media.
    // The original-file verifier authenticates these 89 DWORD offsets.
    std::vector<std::uint8_t> code2(558689U,0U);
    constexpr std::size_t table=0x75F24U+0xF590U;
    for(std::size_t scan=0U;scan<89U;++scan) {
        const auto mapped=static_cast<std::uint32_t>(scan+0x100U);
        for(std::size_t i=0U;i<4U;++i)
            code2[table+4U*scan+i]=
                static_cast<std::uint8_t>(mapped>>(i*8U));
    }
    auto session=OriginalDosKeyboardSession::from_installed_code2(code2);
    assert(session.state().input.mapping_f590.size()==89U);
    assert(session.state().input.pending_f572==0U);
    assert(session.state().count_f588==0U);
    assert(!session.held(0x48U));

    // Make, peek and consume share the SAME live F572 state: a
    // consuming read clears only F572, not F571 or the held-key array.
    assert(!session.receive_port60(0x48U));
    assert(session.held(0x48U) && session.state().count_f588==1U);
    int frames=0;
    assert(session.peek_key([&](){++frames;})==0x148U);
    assert(frames==1 && session.state().input.pending_f572==0x48U);
    assert(session.consume_key([&](){assert(false);})==0x148U);
    assert(frames==1 && session.held(0x48U));
    assert(session.state().last_f571==0x48U);
    assert(session.state().input.pending_f572==0U);

    // 30AD8 runs one callback BEFORE checking F572; callback can
    // publish raw DOS scan bytes through the separate ISR model.
    assert(session.input_ready([&](){
        ++frames;
        assert(!session.receive_port60(0x1EU));
    }));
    assert(frames==2 && session.state().input.pending_f572==0x1EU);
    assert(session.consume_key([&](){assert(false);})==0x11EU);
    assert(session.state().input.pending_f572==0U);

    // A non-navigation key break restores the last HELD navigation
    // scan; it does not dequeue a Windows character.
    assert(!session.receive_port60(0x9EU));
    assert(session.state().input.pending_f572==0x48U);
    assert(session.consume_key([&](){assert(false);})==0x148U);
    assert(!session.receive_port60(0xC8U));
    assert(!session.held(0x48U) && session.state().count_f588==0U);
    assert(session.state().input.pending_f572==0U);

    // 30AEC re-enters 2F868 until a callback publishes input.
    frames=0;
    assert(session.consume_key([&](){
        ++frames;
        if(frames==3) assert(!session.receive_port60(0x20U));
    })==0x120U);
    assert(frames==3 && session.state().input.pending_f572==0U);

    // Modifier and Space latches are separate from key translation.
    assert(!session.receive_port60(0x2AU));
    assert(session.modifiers(0x01U)==1U);
    assert(!session.receive_port60(0x39U));
    assert(session.consume_space()==1U);
    assert(session.consume_space()==0U);
    assert(!session.receive_port60(0xAAU));
    assert(session.modifiers(0x01U)==0U);

    // A layout or patch with an unrecognized size is not decoded
    // at hardcoded DGROUP offsets.
    bool rejected=false;
    code2.pop_back();
    try {(void)OriginalDosKeyboardSession::from_installed_code2(code2);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
}
