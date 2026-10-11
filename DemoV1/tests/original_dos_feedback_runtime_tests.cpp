#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_dos_feedback_runtime.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
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
