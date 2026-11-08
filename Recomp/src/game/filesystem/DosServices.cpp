#include "DosServices.hpp"
#include <cstdint>
#include <string>
#include <string_view>

namespace rloft::host_dos_services {
CinematicPath validate_cinematic(
    std::string_view raw,std::string_view prefix) noexcept {
    // Original 139AC builds a drive-prefix, DOS separator and CINEdd.ANM.
    if(raw.size()!=prefix.size()+11u ||
       raw.substr(0u,prefix.size())!=prefix ||
       raw[prefix.size()]!='\\')
        return CinematicPath::PrefixMismatch;
    const auto name=raw.substr(prefix.size()+1u);
    if(name.size()!=10u || name.substr(0u,4u)!="CINE" ||
       name[4u]<'0' || name[4u]>'9' ||
       name[5u]<'0' || name[5u]>'9' ||
       name.substr(6u)!=".ANM")
        return CinematicPath::FilenameMismatch;
    return CinematicPath::Valid;
}
std::string canonical_basename(
    std::string name,std::uint32_t guest_path) {
    if(guest_path==0xCFCu && name=="COLORS.")
        return "COLORS";
    return name;
}
} // namespace rloft::host_dos_services
