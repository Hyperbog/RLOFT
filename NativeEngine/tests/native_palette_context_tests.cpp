#include "rloft_native/native_palette_context.hpp"
#include <cassert>
#include <stdexcept>
using namespace rloft_native;
namespace {
template<class F> void reject(F f) {
    bool threw=false;
    try{f();}catch(const std::exception&){threw=true;}
    assert(threw);
}
void test() {
    const auto check=[](const char* archive,std::size_t index,
                         unsigned bank,NativePaletteEvidence evidence) {
        const auto value=resolve_native_palette(archive,index);
        assert(value.bank && *value.bank==bank && value.evidence==evidence);
        assert(!value.reason.empty());
    };
    // Verified CODE.2 uses unrelated banks WITHIN RES0.
    check("RES0",16,0,NativePaletteEvidence::ExactCallSite);
    check("RES0",18,0,NativePaletteEvidence::ExactCallSite);
    check("RES0",19,1,NativePaletteEvidence::ExactCallSite);
    check("RES0",30,4,NativePaletteEvidence::ExactCallSite);
    check("RES0",31,0,NativePaletteEvidence::ExactCallSite);
    check("RES0",35,4,NativePaletteEvidence::ExactCallSite);
    check("RES0",46,0,NativePaletteEvidence::ExactCallSite);
    check("RES0",55,4,NativePaletteEvidence::ExactCallSite);
    check("RES0",63,10,NativePaletteEvidence::ExactCallSite);
    check("res0",64,10,NativePaletteEvidence::ExactCallSite);
    check("RES0",89,10,NativePaletteEvidence::ExactCallSite);
    check("RES0",65,11,NativePaletteEvidence::ExactCallSite);
    check("RES0",66,11,NativePaletteEvidence::ExactCallSite);
    check("RES0",90,11,NativePaletteEvidence::ExactCallSite);
    check("RES0",62,12,NativePaletteEvidence::ExactCallSite);
    check("RES0",97,12,NativePaletteEvidence::ExactCallSite);
    check("RES0",105,12,NativePaletteEvidence::ExactCallSite);
    check("RES0",77,13,NativePaletteEvidence::ExactCallSite);
    check("RES0",116,13,NativePaletteEvidence::ExactCallSite);
    check("RES0",117,13,NativePaletteEvidence::ExactCallSite);
    check("RES8",45,4,NativePaletteEvidence::ExactCallSite);
    check("RES8",71,4,NativePaletteEvidence::ExactCallSite);

    const auto missing_level=resolve_native_palette("RES3",300);
    assert(missing_level.bank && *missing_level.bank==4);
    assert(missing_level.evidence==NativePaletteEvidence::ContextualPreview);
    assert(missing_level.alternatives.size()==3);
    for(std::size_t level=0;level<32;++level) {
        const auto result=resolve_native_palette("RES3",300,level);
        assert(result.bank && *result.bank==native_world_palette_bank(level));
        assert(result.evidence==NativePaletteEvidence::LevelContext);
    }
    assert(native_world_palette_bank(29)==0);
    assert(native_world_palette_bank(3)==2);
    assert(native_world_palette_bank(5)==3);
    assert(native_world_palette_bank(0)==4);
    assert(resolve_native_palette("RES0",3).evidence==
           NativePaletteEvidence::ContextualPreview);
    const auto profile=resolve_native_palette("RES8",8);
    assert(profile.bank && *profile.bank==4 &&
           profile.alternatives.size()==1 &&
           profile.alternatives[0]==3);
    const auto profile_level=resolve_native_palette("RES8",8,5);
    assert(profile_level.bank && *profile_level.bank==3);
    // An unrelated level is not evidence to override a profile's known
    // bank set: fall back to explicitly uncertain canonical bank 4.
    const auto mismatch=resolve_native_palette("RES8",8,3);
    assert(mismatch.evidence==NativePaletteEvidence::ContextualPreview);
    for(const char* name:{"RES5","RES6","RES10"}) {
        const auto u=resolve_native_palette(name,2);
        assert(!u.bank && u.evidence==NativePaletteEvidence::Unresolved);
    }
    reject([] {static_cast<void>(native_world_palette_bank(32));});
    reject([] {static_cast<void>(resolve_native_palette("RES3",2,32));});
    reject([] {static_cast<void>(resolve_native_palette("../RES0",1));});
}
}
int main(){test();}
