// Synthetic only: original CODE.2 373BF -> 2E6F0 -> 30508/303C8
// with an in-memory font cache and plane callback sentinels.
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../../Code2/Code2.cpp"
#include <cassert>
#include <cstdint>
#include <vector>
#include <algorithm>
namespace {
using code2::u8;
using code2::u32;
std::vector<u8>* guest=nullptr;
u32 calls=0,seen_char[8]{},seen_row[8]{},seen_col[8]{};
u32 trans_calls=0;
void write_decimal(u32 address,u8 value) noexcept {
    assert(guest && address>=0x2B64Eu && address<0x2B658u);
    (*guest)[address]=value;
}
u32 direct(u32 ch,u32 row,u32 col,u32 image) noexcept {
    assert(image==0x20400u && calls<8u);
    seen_char[calls]=ch;seen_row[calls]=row;seen_col[calls]=col;
    ++calls;return 1u;
}
u32 trans(u32 ch,u32 row,u32 col,u32 image,u32 table) noexcept {
    assert(table==0xD070u);
    ++trans_calls;
    return direct(ch,row,col,image);
}
}
int main(){
    using namespace code2;
    std::vector<u8> mbytes(0x50000u,0);
    guest=&mbytes;
    Machine m{};
    m.data=mbytes.data();
    // Source 2F29C cache contract for font resource 0, archive slot 0.
    store_le32(mbytes.data()+0xF0B8u,0x20000u);
    store_le32(mbytes.data()+0x2000Cu,0x20100u);
    mbytes[0x20102u]=1u;
    store_le32(mbytes.data()+0x20105u,0x20300u);
    store_le32(mbytes.data()+0x20300u,0x20400u);
    for(u32 c=0x2Du;c<=0x39u;++c)
        mbytes[0x20304u+c]=3u;
    // Original 373BF formats negative decimal to original 2B64E.
    Runtime124Boundary runtime{};
    runtime.write8=&write_decimal;
    std::fill_n(mbytes.begin()+0x2B64Eu,10u,0u);
    (void)function_373bf(m,runtime,-12,0x2B64Eu,10u);
    assert(mbytes[0x2B64Eu]=='-'&&mbytes[0x2B64Fu]=='1'&&
           mbytes[0x2B650u]=='2'&&mbytes[0x2B651u]==0u);
    Application69Boundary resources{};
    Application56Boundary glyph{};
    glyph.call_2a380=&direct;
    glyph.call_2a584=&trans;
    Application70Boundary fonts{};
    fonts.app69=&resources;
    fonts.app56=&glyph;
    // Width 32, text width 9, centered start x=0xD5+(32-9)/2=224.
    function_30508(m,fonts,0x120u,0xD5u,0x20u,0x2B64Eu,0u);
    assert(calls==3u && trans_calls==0u);
    assert(seen_char[0]=='-'&&seen_char[1]=='1'&&seen_char[2]=='2');
    assert(seen_col[0]==224u&&seen_col[1]==227u&&seen_col[2]==230u);
    assert(seen_row[0]==0x120u&&seen_row[1]==0x120u&&seen_row[2]==0x120u);
    assert(load_le32(mbytes.data()+0x2B680u)==233u);
    assert(load_le32(mbytes.data()+0x2B684u)==0x120u);
    // Original F56C != 0 selects translated 293EC -> 2A584 branch.
    store_le32(mbytes.data()+0xF56Cu,0xD070u);
    function_30508(m,fonts,0x124u,0xD5u,0x20u,0x2B64Eu,0u);
    assert(calls==6u&&trans_calls==3u);
    assert(seen_row[3]==0x124u && seen_col[5]==230u);
    return 0;
}
