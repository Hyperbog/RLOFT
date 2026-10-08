#include "rloft/combat_target_priority.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    using rloft::CombatTargetPriorityModel;
    std::vector<std::uint8_t> order;

    // DB94 default selects group0 before trying group1.
    auto get=[&](std::uint8_t group)->std::int32_t {
        order.push_back(group);
        return group==0U ? 9 : 12;
    };
    assert(CombatTargetPriorityModel::choose(0U,get)==9);
    assert((order==std::vector<std::uint8_t>{0U}));
    order.clear();
    assert(CombatTargetPriorityModel::choose(1U,get)==12);
    assert((order==std::vector<std::uint8_t>{1U}));

    order.clear();
    auto first_missing=[&](std::uint8_t group)->std::int32_t {
        order.push_back(group);
        return group==0U ? -1 : 42;
    };
    assert(CombatTargetPriorityModel::choose(0U,first_missing)==42);
    assert((order==std::vector<std::uint8_t>{0U,1U}));

    order.clear();
    auto both_missing=[&](std::uint8_t group)->std::int32_t {
        order.push_back(group);
        return -1;
    };
    assert(CombatTargetPriorityModel::choose(1U,both_missing)==-1);
    assert((order==std::vector<std::uint8_t>{1U,0U}));

    // Only low bit controls selection: DOS does not treat a
    // larger "preference flags" value as an index or new group.
    order.clear();
    assert(CombatTargetPriorityModel::choose(0x1234U,get)==9);
    assert((order==std::vector<std::uint8_t>{0U}));
    order.clear();
    assert(CombatTargetPriorityModel::choose(0x1235U,get)==12);
    assert((order==std::vector<std::uint8_t>{1U}));

    bool failed=false;
    try {
        (void)CombatTargetPriorityModel::choose(0U,{});
    } catch (const std::invalid_argument&) {
        failed=true;
    }
    assert(failed);
}
