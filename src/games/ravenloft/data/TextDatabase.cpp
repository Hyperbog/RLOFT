#include "games/ravenloft/data/TextDatabase.h"
#include "games/ravenloft/data/Binary.h"
#include "engine/core/FileSystem.h"
#include "games/ravenloft/game/Code1TextRecord004C.h"
#include <stdexcept>
namespace ravenloft::data {
TextDatabase::TextDatabase(const std::filesystem::path& path){auto b=ssi::core::readFile(path);const auto n=u32(b,0);if(n!=1073)throw std::runtime_error("Unexpected T record count");entries_.reserve(n);for(std::uint32_t i=0;i<n;++i)entries_.push_back(ravenloft::game::Code1TextRecord004C::decode(b,static_cast<std::int32_t>(i)));}
}
