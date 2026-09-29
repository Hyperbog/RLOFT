#include "games/ravenloft/game/PlayerTargetSelectorDB94.h"
#include <cstddef>

namespace ravenloft::game {

int PlayerTargetSelectorDACC::select(int mode,
                                     std::span<const PlayerTargetRecordDACC> records,
                                     std::span<const data::RuntimePlacement22> actors) {
    if(records.empty()) return -1;

    // DS:F038 contains the number of records in the previous render buffer;
    // ESI starts at count-1 and walks the sort ordinal downward.
    for(int rank=static_cast<int>(records.size())-1;rank>=0;--rank) {
        const PlayerTargetRecordDACC* record=nullptr;
        for(const auto& candidate:records) {
            if(candidate.sortRank==static_cast<std::uint16_t>(rank)) {
                record=&candidate;
                break;
            }
        }
        // A valid original render buffer has exactly one record per rank. Keep
        // malformed reconstructed input deterministic instead of reading past
        // the array as the DOS executable would.
        if(!record) continue;

        // +0x08 must be category 0 (NPC).
        if(record->category!=0) continue;
        const auto actorIndex=static_cast<std::size_t>(record->actorIndex);
        if(actorIndex>=actors.size()) continue;
        const auto stateByte3=static_cast<std::uint8_t>(actors[actorIndex].state>>24u);

        // Runtime actor +0x0B bit 1 marks the suppressed/dead state.
        if((stateByte3&0x02u)!=0u) continue;

        if(mode==0) {
            if(record->screenX>0x80u) continue;
        } else if(mode==1) {
            if(record->screenX<=0x80u) continue;
        }

        // 0xDB5D subtracts the 0x7D00 render-list bias, SARs by three,
        // and requires >= 0x3B. Avoid a signed-shift portability dependency:
        // for values that can pass, this is exactly diff >= 0x3B*8 = 0x1D8.
        const int scaleDifference=static_cast<int>(record->biasedScale)-0x7D00;
        if(scaleDifference<0x1D8) continue;

        // Runtime actor +0x0B bit 4 is the targetable/hostile gate.
        if((stateByte3&0x10u)==0u) continue;
        return static_cast<int>(actorIndex);
    }
    return -1;
}

int PlayerAutoTargetDB94::select(std::size_t characterIndex,
                                 std::span<const PlayerTargetRecordDACC> records,
                                 std::span<const data::RuntimePlacement22> actors) {
    const int preferred=(characterIndex&1u)?1:0;
    int result=PlayerTargetSelectorDACC::select(preferred,records,actors);
    if(result!=-1) return result;
    return PlayerTargetSelectorDACC::select(preferred^1,records,actors);
}

} // namespace ravenloft::game
