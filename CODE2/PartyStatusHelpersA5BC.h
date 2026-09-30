#pragma once
#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct PartyStatusHelpersA5BCState {
 std::uint8_t* characters=nullptr; std::int32_t resourceBase=0;
 std::uint8_t d681=0,d682=0,savedD681=0,savedD682=0; std::array<std::uint8_t,4> refresh{};
};
struct PartyStatusHelpersA5BCOps {
 std::function<void(int)> ui1F494;
 std::function<void(int,int,int,int)> playResource;
 std::function<void(int,int,int)> render9B4C;
 std::function<int(int,int)> value22E40;
 std::function<std::int32_t()> clock;
};
class PartyStatusHelpersA5BC {
public:
 static void saveUi(PartyStatusHelpersA5BCState&,const PartyStatusHelpersA5BCOps&);
 static void restoreUi(PartyStatusHelpersA5BCState&,const PartyStatusHelpersA5BCOps&);
 static void installParty20(PartyStatusHelpersA5BCState&,const PartyStatusHelpersA5BCOps&,int character);
 static void installAll20(PartyStatusHelpersA5BCState&,const PartyStatusHelpersA5BCOps&,int character);
};
}
