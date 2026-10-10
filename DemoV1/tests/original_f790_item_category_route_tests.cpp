#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_f790_item_category_route.hpp"

#include <cassert>
#include <cstdint>

int main() {
    using Route=rloft::OriginalF790ItemCategoryRoute;
    constexpr std::uint32_t table[10]={
        0xFA85,0xFA85,0xFA85,0xFB3D,0xFB3D,
        0xFB3D,0xFB3D,0xF8D4,0xF8D4,0xF8D4};
    for (int i=0;i<10;++i)
        assert(Route::destination(static_cast<std::uint8_t>(i+2))==table[i]);
    // Original AL byte SUB and unsigned JA default, including wraparound
    // for categories 0,1 and all categories above eleven.
    for(int i=0;i<256;++i) {
        const auto actual=Route::destination(static_cast<std::uint8_t>(i));
        if(i<2||i>11)assert(actual==0xFB3DU);
        else if(i<=4)assert(actual==0xFA85U);
        else if(i<=8)assert(actual==0xFB3DU);
        else assert(actual==0xF8D4U);
    }
    static_assert(Route::destination(0U)==0xFB3DU,"unsigned AL underflow");
    static_assert(Route::destination(11U)==0xF8D4U,"last CS dispatch entry");
    static_assert(Route::destination(255U)==0xFB3DU,"large category fallback");
}
