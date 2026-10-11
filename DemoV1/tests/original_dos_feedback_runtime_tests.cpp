#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_dos_feedback_runtime.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    // Native runtime loads the keyboard map from CODE.2, not an
    // invented Win32 virtual-key table or hardcoded English layout.
    // The source-only test uses synthetic bytes at the authenticated
    // original offsets; original media is only consumed externally.
    std::vector<std::uint8_t> synthetic_code2(558689U,0U);
    constexpr std::size_t kMapAt=0x75F24U+0xF590U;
    constexpr std::size_t kKeyAt=0x75F24U+0xF572U;
    for(std::size_t i=0U;i<89U;++i) {
        const auto word=static_cast<std::uint32_t>(
            0x01020000U | static_cast<std::uint32_t>(i));
        for(std::size_t b=0U;b<4U;++b)
            synthetic_code2[kMapAt+4U*i+b]=
                static_cast<std::uint8_t>(word>>(8U*b));
    }
    synthetic_code2[kKeyAt]=0x48U;
    auto observed=
        rloft::OriginalDosFeedbackRuntimeModel::capture_initial_f590_from_code2(
            synthetic_code2);
    assert(observed.pending_f572==0x48U);
    assert(observed.mapping_f590.size()==89U);
    assert(observed.mapping_f590.front()==0x01020000U);
    assert(observed.mapping_f590[0x48U]==0x01020048U);
    assert(observed.mapping_f590.back()==0x01020058U);
    synthetic_code2.resize(558688U);
    bool rejected_version=false;
    try {
        (void)rloft::OriginalDosFeedbackRuntimeModel::
            capture_initial_f590_from_code2(synthetic_code2);
    }catch(const std::invalid_argument&) {rejected_version=true;}
    assert(rejected_version);

    using rloft::OriginalDosFeedbackRuntimeModel;
    using rloft::OriginalDosInputState;
    using Bytes=std::vector<std::uint8_t>;

    // 311E8 replaces internal dollars permanently, but the final
    // temporary DOS '$' is restored to NUL after the service returns.
    Bytes text{'A','$','B','$',0U,'Z'};
    int interrupts=0;
    OriginalDosFeedbackRuntimeModel::print_311e8(
        text,[&](const Bytes& live,std::size_t n) {
            ++interrupts;
            assert(n==4U);
            assert((live==Bytes{'A','*','B','*','$','Z'}));
            assert(&live==&text);
        });
    assert(interrupts==1);
    assert((text==Bytes{'A','*','B','*',0U,'Z'}));

    text={0U,'Q'};
    OriginalDosFeedbackRuntimeModel::print_311e8(
        text,[&](const Bytes& live,std::size_t n) {
            ++interrupts;
            assert(n==0U && live[0]=='$' && live[1]=='Q');
        });
    assert((text==Bytes{0U,'Q'}));
    assert(interrupts==2);

    bool rejected=false;
    text={'$','X'};
    try {
        OriginalDosFeedbackRuntimeModel::print_311e8(
            text,[](const Bytes&,std::size_t){assert(false);});
    } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected && (text==Bytes{'$','X'}));

    // Original 30AD8 is one 2F868 frame/input pass and a boolean
    // live F572 probe; it does not consume the scan byte.
    OriginalDosInputState polled{};
    polled.mapping_f590={0xAAU,0xABU,0xACU};
    int frame_calls=0;
    assert(!OriginalDosFeedbackRuntimeModel::input_ready_30ad8(
        polled,[&]() {++frame_calls;}));
    assert(frame_calls==1 && polled.pending_f572==0U);
    assert(OriginalDosFeedbackRuntimeModel::input_ready_30ad8(
        polled,[&]() {++frame_calls;polled.pending_f572=2U;}));
    assert(frame_calls==2 && polled.pending_f572==2U);
    // Original 30B40 also calls 2F868 exactly once, then returns
    // F590[BYTE F572] WITHOUT clearing F572, even for index zero.
    assert(OriginalDosFeedbackRuntimeModel::peek_key_30b40(
        polled,[&]() {++frame_calls;})==0xACU);
    assert(frame_calls==3 && polled.pending_f572==2U);
    assert(OriginalDosFeedbackRuntimeModel::peek_key_30b40(
        polled,[&]() {++frame_calls;polled.pending_f572=0U;})==0xAAU);
    assert(frame_calls==4 && polled.pending_f572==0U);
    bool missing_peek=false;
    polled.pending_f572=42U;
    try {
        (void)OriginalDosFeedbackRuntimeModel::peek_key_30b40(
            polled,[&]() {++frame_calls;});
    }catch(const std::invalid_argument&) {missing_peek=true;}
    assert(missing_peek && polled.pending_f572==42U);
    assert(frame_calls==5);
    bool missing_service=false;
    try {
        (void)OriginalDosFeedbackRuntimeModel::input_ready_30ad8(polled,{});
    }catch(const std::invalid_argument&) {missing_service=true;}
    assert(missing_service && polled.pending_f572==42U);

    // 30AEC starts with a pending byte: it must return the original
    // DWORD table result without invoking the optional graphics path.
    OriginalDosInputState input{};
    input.mapping_f590.assign(256U,0xFFFFFFFFU);
    input.mapping_f590[2U]=0x12345678U;
    input.pending_f572=2U;
    assert(OriginalDosFeedbackRuntimeModel::poll_30aec(
        input,[]{assert(false);})==0x12345678U);
    assert(input.pending_f572==0U);

    // While input is zero, the original calls 2F868 repeatedly. The
    // opaque callback can mutate both the live key and F590 mapping.
    int draws=0;
    input.mapping_f590[5U]=123U;
    const auto result=OriginalDosFeedbackRuntimeModel::poll_30aec(
        input,[&]() {
            ++draws;
            assert(input.pending_f572==0U);
            if(draws==3) {
                input.pending_f572=5U;
                input.mapping_f590[5U]=0x80000001U;
            }
        });
    assert(draws==3 && result==0x80000001U);
    assert(input.pending_f572==0U);

    // F590 does not have a source index guard. Incomplete captured
    // memory must fail closed BEFORE falsely clearing the live byte.
    input.mapping_f590.resize(2U);
    input.pending_f572=9U;
    rejected=false;
    try {
        (void)OriginalDosFeedbackRuntimeModel::poll_30aec(input,{});
    } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected && input.pending_f572==9U);

    input.pending_f572=0U;
    rejected=false;
    try {
        (void)OriginalDosFeedbackRuntimeModel::poll_30aec(input,{});
    } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected && input.pending_f572==0U);
}
