#include "rloft_native/native_session_profile.hpp"
#include "rloft_native/native_sha256.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft_native {
namespace {
constexpr char kMagic[]="RNATSV01";
constexpr std::size_t kShaSize=64U;
constexpr unsigned kVersion=1U;
constexpr double kPi=3.1415926535897932384626433832795;
void require(bool yes,const char* message) {
    if(!yes)throw std::runtime_error(message);
}
bool valid_sha(const std::string& sha) {
    return sha.size()==kShaSize &&
        std::all_of(sha.begin(),sha.end(),[](char c) {
            return (c>='0'&&c<='9')||(c>='a'&&c<='f');
        });
}
void add32(std::vector<std::uint8_t>& out,std::uint32_t n) {
    for(unsigned i=0;i<4;++i)out.push_back(
        static_cast<std::uint8_t>(n>>(8U*i)));
}
void add64(std::vector<std::uint8_t>& out,std::uint64_t n) {
    for(unsigned i=0;i<8;++i)out.push_back(
        static_cast<std::uint8_t>(n>>(8U*i)));
}
void add_double(std::vector<std::uint8_t>& out,double n) {
    static_assert(sizeof(double)==8,"native session requires binary64");
    static_assert(std::numeric_limits<double>::is_iec559,
                  "native session requires IEEE754 doubles");
    std::uint64_t bits{};
    std::memcpy(&bits,&n,sizeof(bits));
    add64(out,bits);
}
std::uint64_t read_uint(const std::vector<std::uint8_t>& data,
                        std::size_t& at,unsigned count) {
    require(count==4U||count==8U,"native profile integer length invalid");
    require(at<=data.size()&&data.size()-at>=count,
            "native session profile header truncated");
    std::uint64_t n=0;
    for(unsigned i=0;i<count;++i)
        n|=std::uint64_t(data[at++])<<(8U*i);
    return n;
}
double read_double(const std::vector<std::uint8_t>& data,std::size_t& at) {
    const auto bits=read_uint(data,at,8);
    double n{};
    std::memcpy(&n,&bits,sizeof(n));
    return n;
}
void validate(const NativeSessionProfile& p,
              const NativeWorldRuntime& world) {
    // Validation binds a native profile to the *complete*, immutable
    // original source world, never to an unrelated game installation.
    require(valid_sha(p.original_world_sha256) &&
            p.original_world_sha256==
                world.snapshot().source_fingerprint_sha256,
            "native profile original LEVELS source fingerprint mismatch");
    require(p.level==NativePartyStart::kFirstLevel,
            "native profile is not a supported first-level party session");
    const auto& map=world.original().level(p.level);
    require(map.width>0U&&map.height>0U&&
            std::size_t(map.width)*map.height<=32768U &&
            p.discovered_cells.size()==std::size_t(map.width)*map.height,
            "native profile revealed map size does not match source");
    require(std::all_of(p.discovered_cells.begin(),
                        p.discovered_cells.end(),
                        [](std::uint8_t b){return b<=1U;}),
            "native profile discovered-cell byte is not Boolean");
    require(std::isfinite(p.column)&&std::isfinite(p.row)&&
            std::isfinite(p.heading_radians)&&
            std::isfinite(p.pitch_radians)&&
            p.column>=0.0&&p.row>=0.0&&
            p.column<double(map.width)&&p.row<double(map.height)&&
            p.heading_radians>=-kPi-0.000001&&
            p.heading_radians<=kPi+0.000001&&
            p.pitch_radians>=-1.42&&p.pitch_radians<=1.42,
            "native profile saved camera is outside supported bounds");
    require(world.normal_collision(
                p.level,static_cast<unsigned>(p.column),
                static_cast<unsigned>(p.row))==NativeCollision::Passable,
            "native profile camera is inside original blocked terrain");
    // CHARS is not a saved DOS profile: only validate and preserve
    // CODE.1 transfer, never rewrite or synthesize actor identity.
    static_cast<void>(decode_native_party_start(p.chars));
}
} // namespace

std::vector<std::uint8_t> encode_native_session_profile(
    const NativeSessionProfile& p,const NativeWorldRuntime& world) {
    validate(p,world);
    std::vector<std::uint8_t> bytes;
    bytes.reserve(8U+4U+64U+p.chars.size()+4U+32U+4U+
                  p.discovered_cells.size()+64U);
    bytes.insert(bytes.end(),kMagic,kMagic+8U);
    add32(bytes,kVersion);
    bytes.insert(bytes.end(),p.original_world_sha256.begin(),
                 p.original_world_sha256.end());
    bytes.insert(bytes.end(),p.chars.begin(),p.chars.end());
    add32(bytes,p.level);
    add_double(bytes,p.column);
    add_double(bytes,p.row);
    add_double(bytes,p.heading_radians);
    add_double(bytes,p.pitch_radians);
    add32(bytes,static_cast<std::uint32_t>(p.discovered_cells.size()));
    bytes.insert(bytes.end(),p.discovered_cells.begin(),
                 p.discovered_cells.end());
    const auto digest=native_sha256_hex(bytes);
    bytes.insert(bytes.end(),digest.begin(),digest.end());
    return bytes;
}

NativeSessionProfile decode_native_session_profile(
    const std::vector<std::uint8_t>& data,const NativeWorldRuntime& world) {
    constexpr std::size_t kPrefix=8U+4U+kShaSize+0x920U+4U+32U+4U;
    require(data.size()>=kPrefix+kShaSize &&
            data.size()<=kPrefix+32768U+kShaSize,
            "native session profile byte count invalid");
    require(std::equal(kMagic,kMagic+8U,data.begin()),
            "native profile does not have a native-only format marker");
    const std::string digest(data.end()-static_cast<std::ptrdiff_t>(kShaSize),
                             data.end());
    const std::vector<std::uint8_t> content(
        data.begin(),data.end()-static_cast<std::ptrdiff_t>(kShaSize));
    require(valid_sha(digest)&&native_sha256_hex(content)==digest,
            "native session profile SHA-256 integrity check failed");
    std::size_t at=8U;
    require(read_uint(content,at,4)==kVersion,
            "native profile format version unsupported");
    NativeSessionProfile p;
    require(content.size()-at>=kShaSize,
            "native profile original fingerprint truncated");
    p.original_world_sha256.assign(content.begin()+static_cast<std::ptrdiff_t>(at),
                                   content.begin()+static_cast<std::ptrdiff_t>(at+kShaSize));
    at+=kShaSize;
    require(content.size()-at>=p.chars.size(),
            "native profile original CHARS image truncated");
    std::copy_n(content.begin()+static_cast<std::ptrdiff_t>(at),
                p.chars.size(),p.chars.begin());
    at+=p.chars.size();
    p.level=static_cast<unsigned>(read_uint(content,at,4));
    p.column=read_double(content,at);
    p.row=read_double(content,at);
    p.heading_radians=read_double(content,at);
    p.pitch_radians=read_double(content,at);
    const auto count=read_uint(content,at,4);
    require(count<=32768U&&count==content.size()-at,
            "native profile discovered-cell section malformed");
    p.discovered_cells.assign(
        content.begin()+static_cast<std::ptrdiff_t>(at),content.end());
    validate(p,world);
    return p;
}
} // namespace rloft_native
