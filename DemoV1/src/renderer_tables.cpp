#include "rloft/renderer_tables.hpp"
#include <cstddef>
#include <stdexcept>
namespace rloft { namespace {
std::uint16_t u16(const std::uint8_t* p){return static_cast<std::uint16_t>(p[0])|(static_cast<std::uint16_t>(p[1])<<8U);}
std::uint32_t u32(const std::uint8_t* p){return static_cast<std::uint32_t>(p[0])|(static_cast<std::uint32_t>(p[1])<<8U)|(static_cast<std::uint32_t>(p[2])<<16U)|(static_cast<std::uint32_t>(p[3])<<24U);}
}
RendererTables RendererTables::parse_prefix(const std::vector<std::uint8_t>& b){
 constexpr std::size_t kLookupAt=0x1B44U;
 if(b.size()<0x1B44U)throw std::runtime_error("SINTAB fixed prefix is truncated");
 RendererTables out{};
 std::size_t p=0;
 for(auto& v:out.sine_q16_signmag){v=u32(b.data()+p);p+=4;}
 for(auto& v:out.tangent_q16){v=u32(b.data()+p);p+=4;}
 for(auto& v:out.projection_a){v=u16(b.data()+p);p+=2;}
 for(auto& v:out.projection_b){v=u16(b.data()+p);p+=2;}
 for(auto& v:out.constants){v=u32(b.data()+p);p+=4;}
 for(auto& v:out.view_angle_tenths){v=static_cast<std::int32_t>(u32(b.data()+p));p+=4;}
 out.scale_block_count=u32(b.data()+p);p+=4;
 out.runtime_lookup_max=u32(b.data()+p);p+=4;
 out.renderer_aux_value=u32(b.data()+p);p+=4;
 if(p!=kLookupAt)throw std::runtime_error("SINTAB fixed-prefix layout mismatch");
 const auto count=static_cast<std::size_t>(out.runtime_lookup_max)+1U;
 if(count> (b.size()-p)/2U)throw std::runtime_error("SINTAB runtime lookup is truncated");
 out.runtime_lookup.resize(count);
 for(auto& v:out.runtime_lookup){v=u16(b.data()+p);p+=2;}
 return out;
}
RendererTables RendererTables::load(const GameSource& source){return parse_prefix(source.read("SINTAB"));}
}  // namespace rloft
