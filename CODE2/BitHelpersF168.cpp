#include <cstdint>
namespace ravenloft::game {
bool specialIdF168(int v){switch(v){case 6:case 0x109:case 0x10e:case 0x120:case 0x127:case 0x131:case 0x134:case 0x135:case 0x13b:case 0x13d:case 0x141:case 0x146:case 0x147:return true;default:return false;}}
std::uint32_t bitF218(std::uint32_t field,std::uint8_t bit){return (field&(1u<<(bit&31)))>>(bit&31);}
void setBitF238(std::uint32_t&field,std::int16_t slot,std::uint8_t bit,std::uint32_t value){if(slot==-1)return;const unsigned b=bit&31;field=(field&~(1u<<b))|(value<<b);}
std::uint8_t bitF274(const std::uint8_t*base,int row,std::uint8_t bit){return static_cast<std::uint8_t>((base[row*10]&(1u<<(bit&7)))>>(bit&7));}
void setBitF290(std::uint8_t*base,int row,std::uint8_t bit,std::uint8_t value){const unsigned b=bit&7;base[row*10]=static_cast<std::uint8_t>((base[row*10]&~(1u<<b))|(value<<b));}
bool specialF2B4(int v){return v==0x10||v==0x5f;}
bool pairF2D4(std::int16_t a,std::int16_t b,int v){return a==v||b==v;}
}
