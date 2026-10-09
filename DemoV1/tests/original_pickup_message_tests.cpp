#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_pickup_message.hpp"
#include <array>
#include <cassert>
#include <cstdint>
#include <optional>
#include <utility>

int main() {
    using M=rloft::OriginalPickupMessageModel;
    // Original object-ID comparison order in CODE.2 21767..2187E.
    const std::array<std::pair<std::uint16_t,std::uint32_t>,9> cases{{
        {0x49U,0x25U}, {0x9EU,0x36U}, {0x5FU,0x37U},
        {0x8BU,0x38U}, {0x10U,0x39U}, {0xA7U,0x3AU},
        {0xC9U,0x3AU}, {0x77U,0x3CU}, {0x75U,0x29U}
    }};
    for(const auto& item:cases) {
        assert(M::assignment_21704(item.first,false,false)
               ==std::optional<std::uint32_t>(item.second));
    }
    for(std::uint16_t id=0x61U;id<=0x64U;++id) {
        assert(!M::assignment_21704(id,false,false));
        assert(M::assignment_21704(id,true,false)==0x0FU);
    }
    assert(!M::assignment_21704(0x74U,true,false));
    assert(M::assignment_21704(0x74U,false,true)==0x2FU);
    assert(!M::assignment_21704(0x87U,true,true));
    assert(!M::assignment_21704(0x88U,true,true));
    assert(!M::assignment_21704(0xE9U,true,true));
    assert(!M::assignment_21704(0x5EU,true,true));
    assert(!M::assignment_21704(0xCAU,true,true));
    assert(!M::assignment_21704(0U,true,true));
    assert(!M::assignment_21704(0xFFFFU,true,true));
}
