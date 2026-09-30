#include "MapExportFile2A34.h"

namespace ravenloft::game {

void MapExportFile2A34::run(MapExportFile2A34State& s,const MapExportFile2A34Ops& o,
                            std::int32_t number) {
    const std::int32_t hundreds=number/100;
    s.nameTemplate5FDA[0x0D]=static_cast<std::uint8_t>(hundreds+'0');
    s.nameTemplate5FDA[0x0E]=static_cast<std::uint8_t>((number%10)+'0');
    const std::int32_t remainder=number-hundreds*100;
    s.nameTemplate5FDA[0x0F]=static_cast<std::uint8_t>((remainder/10)+'0');

    o.call31380(s.nameTemplate5FDA);
    const std::int32_t oldHandle=o.call312C8(s.nameTemplate5FDA);
    o.call312DC(oldHandle);

    const std::int32_t handle=o.call312B4(s.nameTemplate5FDA);
    if(handle==-1){o.call2E6D0(); return;}

    o.call26E8(0,handle,1);
    o.call312DC(handle);
}

} // namespace ravenloft::game
