#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct PartyGateCCF4Ops{std::function<std::uint16_t(int)> wordF0;std::function<void()> sequenceCBC8;};
void partyGateCCF4(const PartyGateCCF4Ops&o){for(int i=0;i<4;++i)if((o.wordF0(i)&0xd040u)==0)return;o.sequenceCBC8();}
}
