#include "games/ravenloft/data/TextDatabase.h"
#include "games/ravenloft/data/Binary.h"
#include "engine/core/FileSystem.h"
#include <stdexcept>
namespace ravenloft::data {
TextDatabase::TextDatabase(const std::filesystem::path& path){auto b=ssi::core::readFile(path);const auto n=u32(b,0);if(n!=1073)throw std::runtime_error("Unexpected T record count");entries_.reserve(n);for(std::uint32_t i=0;i<n;++i){const auto o=u32(b,4+4*i);const auto len=u16(b,o);requireRange(b,o+2,len,"T record");std::string s; s.reserve(len);for(std::size_t j=0;j<len;++j)s.push_back(static_cast<char>(b[o+2+j]^0xDC));entries_.push_back(std::move(s));}}
}
