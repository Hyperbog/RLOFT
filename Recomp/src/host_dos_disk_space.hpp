#pragma once
// DOS INT 21h AH=36h "Get Disk Free Space", 16-bit register adaptation
// from an actual host path. DOS registers AX/BX/CX/DX cannot express an
// arbitrary modern volume, so this conservative FAT16-compatible view
// caps the representable size at 65535 clusters of 32 KiB each.
// Host FREE and TOTAL bytes are observed, never fabricated constants.
// This is a native DOS service adapter, NOT an original DOS drive trace.
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <stdexcept>
#include <system_error>

namespace rloft { namespace host_dos_disk {
struct Reply {
    std::uint16_t sectors_per_cluster=64u; // DOS AX
    std::uint16_t free_clusters=0u;         // DOS BX
    std::uint16_t bytes_per_sector=512u;    // DOS CX
    std::uint16_t total_clusters=0u;        // DOS DX
};
constexpr std::uint64_t kBytesPerCluster=64u*512u;
inline Reply encode(std::uint64_t capacity,std::uint64_t available) {
    if(capacity==0u || available>capacity ||
       capacity==std::numeric_limits<std::uint64_t>::max() ||
       available==std::numeric_limits<std::uint64_t>::max())
        throw std::runtime_error("Host filesystem capacity/available bytes invalid");
    Reply result{};
    const auto total=std::min<std::uint64_t>(capacity/kBytesPerCluster,65535u);
    const auto free=std::min<std::uint64_t>(available/kBytesPerCluster,total);
    if(total==0u)
        throw std::runtime_error("Host volume too small for DOS AH36 32KiB geometry");
    result.total_clusters=static_cast<std::uint16_t>(total);
    result.free_clusters=static_cast<std::uint16_t>(free);
    return result;
}
inline Reply from_host_path(const std::filesystem::path& game_root) {
    std::error_code error{};
    const auto stats=std::filesystem::space(game_root,error);
    if(error)throw std::runtime_error(
        "Cannot query the game volume for DOS INT21 AH36: "+error.message());
    return encode(stats.capacity,stats.available);
}
}} // namespace rloft::host_dos_disk
