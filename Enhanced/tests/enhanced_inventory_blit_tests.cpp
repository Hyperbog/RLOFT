#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/enhanced_inventory_blit.hpp"
#include <cassert>
#include <stdexcept>

int main() {
    using namespace rloft;
    IndexedFramebuffer screen{};
    screen.clear(91);
    Type1Raster item{};
    item.count=1;
    item.width=3;
    item.height=2;
    item.transparent_index=254;
    item.pixels={1,254,3,4,5,6};
    const auto unchanged=item.pixels;
    const auto drawn=EnhancedInventoryBlit::reflected_horizontal(
        screen,item,0,22,42);
    assert(drawn==5);
    const auto at=[&](int row,int col){
        return screen.pixels()[static_cast<std::size_t>(row)*
            IndexedFramebuffer::kWidth+static_cast<std::size_t>(col)];
    };
    assert(at(22,42)==3 && at(22,43)==91 && at(22,44)==1);
    assert(at(23,42)==6 && at(23,43)==5 && at(23,44)==4);
    assert(at(21,42)==91 && at(24,42)==91);
    assert(item.pixels==unchanged);
    screen.clear(9);
    assert(EnhancedInventoryBlit::reflected_horizontal(
        screen,item,0,399,319)==1);
    assert(at(399,319)==3);
    bool bad=false;
    try {
        auto invalid=item;
        invalid.pixels.pop_back();
        (void)EnhancedInventoryBlit::reflected_horizontal(
            screen,invalid,0,22,42);
    } catch(const std::invalid_argument&) {bad=true;}
    assert(bad && at(399,319)==3);
    bad=false;
    try {
        (void)EnhancedInventoryBlit::reflected_horizontal(
            screen,item,1,22,42);
    } catch(const std::invalid_argument&) {bad=true;}
    assert(bad);
}
