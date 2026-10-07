#include "rloft/renderer_tables.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>
namespace {
void p16(std::vector<std::uint8_t>& b,std::size_t o,std::uint16_t v){b[o]=static_cast<std::uint8_t>(v);b[o+1]=static_cast<std::uint8_t>(v>>8);}
void p32(std::vector<std::uint8_t>& b,std::size_t o,std::uint32_t v){for(int i=0;i<4;++i)b[o+i]=static_cast<std::uint8_t>(v>>(i*8));}
}
int main(){
 std::vector<std::uint8_t> b(0x1B44+6,0);std::size_t p=0;
 for(int i=0;i<450;++i){p32(b,p,static_cast<std::uint32_t>(i));p+=4;}
 for(int i=0;i<900;++i){p32(b,p,static_cast<std::uint32_t>(1000+i));p+=4;}
 for(int i=0;i<128;++i){p16(b,p,static_cast<std::uint16_t>(i));p+=2;}
 for(int i=0;i<128;++i){p16(b,p,static_cast<std::uint16_t>(200+i));p+=2;}
 const std::uint32_t constants[8]={256,256,127,328,328,22272,175,179200};
 for(auto v:constants){p32(b,p,v);p+=4;}
 for(int i=0;i<256;++i){p32(b,p,static_cast<std::uint32_t>(359-i*2));p+=4;}
 p32(b,p,127);p+=4;p32(b,p,2);p+=4;p32(b,p,128);p+=4;
 assert(p==0x1B44);p16(b,p,11);p+=2;p16(b,p,22);p+=2;p16(b,p,33);
 const auto t=rloft::RendererTables::parse_prefix(b);
 assert(t.sine_q16_signmag[449]==449);assert(t.tangent_q16[0]==1000);
 assert(t.c6d8()==256&&t.c6d0()==127&&t.grid_projection_constant()==22272&&t.depth_numerator()==179200);
 assert(t.scale_block_count==127&&t.runtime_lookup_max==2&&t.renderer_aux_value==128);
 assert(t.runtime_lookup.size()==3&&t.runtime_lookup[2]==33);
}
