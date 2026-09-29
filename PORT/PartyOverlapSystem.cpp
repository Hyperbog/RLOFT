#include "games/ravenloft/game/PartyOverlapSystem.h"

namespace ravenloft::game {
namespace {
bool inWrappedInclusive(std::int32_t value,std::int32_t center){
    const auto v=static_cast<std::uint32_t>(value);
    const auto c=static_cast<std::uint32_t>(center);
    constexpr std::uint32_t r=0x8FA0u;
    return v>=c-r && v<=c+r;
}
}

bool PartyOverlapSystem::overlaps(std::int32_t candidateYFixed,
                                  std::int32_t candidateXFixed,
                                  std::int32_t partyXFixed,
                                  std::int32_t partyYFixed){
    return inWrappedInclusive(candidateXFixed,partyXFixed) &&
           inWrappedInclusive(candidateYFixed,partyYFixed);
}

} // namespace ravenloft::game
