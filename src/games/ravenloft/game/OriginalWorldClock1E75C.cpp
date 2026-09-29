#include "games/ravenloft/game/OriginalWorldClock1E75C.h"

namespace ravenloft::game {

std::uint32_t OriginalWorldClock1E75C::normalize31F94(std::uint32_t value){
    auto hour=static_cast<std::uint16_t>(value>>16u);
    if(hour==0xFFFFu)hour=23u;
    hour=static_cast<std::uint16_t>(hour%24u);
    return (static_cast<std::uint32_t>(hour)<<16u)|(value&0xFFFFu);
}

void OriginalWorldClock1E75C::incrementHourAndNormalize31F94(){
    // 0x1E7AB / 0x222CD increments only the high WORD at CF4E, wrapping at
    // 16 bits before 0x31F94 normalizes the resulting hour.
    auto hour=static_cast<std::uint16_t>(worldClockCf4c_>>16u);
    hour=static_cast<std::uint16_t>(hour+1u);
    worldClockCf4c_=(static_cast<std::uint32_t>(hour)<<16u)|(worldClockCf4c_&0xFFFFu);
    worldClockCf4c_=normalize31F94(worldClockCf4c_);
}

std::uint32_t OriginalWorldClock1E75C::service1E75C(std::uint32_t gameplayA7E4){
    // 0x1E781..0x1E7A5 uses 32-bit subtraction followed by signed JL. Once
    // the threshold is reached BF50 is replaced with the current A7E4 value;
    // residual ticks are intentionally not carried into the next interval.
    const auto delta=static_cast<std::int32_t>(gameplayA7E4-baselineBf50_);
    if(delta<static_cast<std::int32_t>(kGameplayTicksPerHour))return 0;

    baselineBf50_=gameplayA7E4;
    auto remaining=delta;
    std::uint32_t advanced=0;
    while(remaining>=static_cast<std::int32_t>(kGameplayTicksPerHour)){
        incrementHourAndNormalize31F94();
        remaining-=static_cast<std::int32_t>(kGameplayTicksPerHour);
        ++advanced;
    }
    return advanced;
}

bool OriginalWorldClock1E75C::mistWaitStep222C0(){
    if(hourHighWord()==2u)return true;
    incrementHourAndNormalize31F94();
    return false;
}

} // namespace ravenloft::game
