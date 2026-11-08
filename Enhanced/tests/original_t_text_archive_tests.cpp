#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_t_text_archive.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    std::vector<std::uint8_t> bytes={
        2,0,0,0, 12,0,0,0, 17,0,0,0,
        3,0,static_cast<std::uint8_t>('A'^0xDC),
        static_cast<std::uint8_t>('B'^0xDC),static_cast<std::uint8_t>('C'^0xDC),
        5,0,static_cast<std::uint8_t>('H'^0xDC),
        static_cast<std::uint8_t>('e'^0xDC),static_cast<std::uint8_t>('l'^0xDC),
        static_cast<std::uint8_t>('l'^0xDC),static_cast<std::uint8_t>('o'^0xDC)};
    const rloft::OriginalTTextArchive archive(bytes);
    assert(archive.count()==2);
    assert(archive.decoded(0)=="ABC");
    assert(archive.decoded(1)=="Hello");
    bool caught=false;
    try { (void)archive.decoded(2); }
    catch(const std::out_of_range&) {caught=true;}
    assert(caught);
    bytes[4]=0xff;bytes[5]=0xff;bytes[6]=0xff;bytes[7]=0xff;
    caught=false;
    try { (void)rloft::OriginalTTextArchive(bytes).decoded(0); }
    catch(const std::out_of_range&) {caught=true;}
    assert(caught);
    caught=false;
    try { (void)rloft::OriginalTTextArchive({}); }
    catch(const std::invalid_argument&) {caught=true;}
    assert(caught);
}
