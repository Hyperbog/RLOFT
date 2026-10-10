#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_f790_caller_catalog.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

int main() {
    using Catalog=rloft::OriginalF790CallerCatalog;
    using Origin=rloft::OriginalF790ArgumentOrigin;
    static_assert(Catalog::kDirectCalls.size()==17U,
                  "all direct original CODE.2 F790 E8 callsites");
    static_assert(Catalog::arguments_at(0x1134cU).has_value(),
                  "initial 112EC call");
    static_assert(*Catalog::arguments_at(0x1184aU)==Origin::ExplicitEmpty,
                  "original immediate empty-hand action");
    static_assert(*Catalog::arguments_at(0x11f47U)==Origin::StackLocal,
                  "original container transaction argument");
    static_assert(*Catalog::arguments_at(0x130deU)==
                      Origin::SelectedKnownZero,
                  "original selected/known-zero argument");
    static_assert(*Catalog::arguments_at(0x131bbU)==
                      Origin::GlobalHeld8600_8602,
                  "original feedback-expiry restore");
    static_assert(!Catalog::arguments_at(0x1134dU).has_value(),
                  "unknown address cannot map to an input source");

    const std::array<std::uint32_t,17> verified_addresses{{
        0x1134cU,0x1173bU,0x1178eU,0x1184aU,0x11882U,
        0x118ffU,0x119fcU,0x11b63U,0x11f47U,0x127ecU,
        0x128a3U,0x12afaU,0x12ba4U,0x12d50U,0x12dfdU,
        0x130deU,0x131bbU
    }};
    std::array<unsigned,4> totals{};
    for(std::size_t i=0;i<verified_addresses.size();++i) {
        const auto expected=verified_addresses[i];
        assert(Catalog::kDirectCalls[i].e8_offset==expected);
        assert(i==0U || verified_addresses[i-1U]<expected);
        const auto o=Catalog::arguments_at(expected);
        assert(o && *o==Catalog::kDirectCalls[i].argument_origin);
        switch(*o) {
        case Origin::GlobalHeld8600_8602: ++totals[0];break;
        case Origin::ExplicitEmpty:++totals[1];break;
        case Origin::StackLocal:++totals[2];break;
        case Origin::SelectedKnownZero:++totals[3];break;
        }
    }
    assert((totals==std::array<unsigned,4>{{11U,3U,2U,1U}}));
}
