// Retail CODE.2 298F0: six-argument source wrapper into 2F29C and 2A584.
// Synthetic sentinel coordinates; no original graphics are bundled.
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../../Code2/Code2.cpp"
#include <cassert>
#include <cstdint>
int main() {
    using namespace code2;
    u32 seen_id=0,seen_slot=0,seen_f=0,seen_row=0,seen_col=0,seen_image=0,seen_lut=0;
    // Function pointer callbacks cannot capture local state.
    // The frame/image boundaries are distinguishable through exact
    // constant arguments even without a private game RES archive.
    static u32 resource=0,slot=0,frame=0,row=0,column=0,img=0,lut=0;
    Application57Boundary b{};
    b.call_2f29c=[](u32 id,u32 archive)->u32{
        resource=id;slot=archive;return 0x40300u;
    };
    b.call_2a584=[](u32 f,u32 y,u32 x,u32 ptr,u32 tab)->u32{
        frame=f;row=y;column=x;img=ptr;lut=tab;return 1u;
    };
    b.call_2e758=[]()->u32{return 0x2Au;};
    const u32 result=function_298f0(
        b,2u,0x138u,0xD7u,0x16Cu,1u,0xD070u);
    assert(result==0x2Au && resource==0x16Cu && slot==1u);
    assert(frame==2u && row==0x138u && column==0xD7u);
    assert(img==0x40300u && lut==0xD070u);
    b.call_2f29c=[](u32,u32)->u32{return 0u;};
    frame=0xFFFFu;
    (void)function_298f0(b,7u,8u,9u,0x12u,0u,0xCF70u);
    assert(frame==0xFFFFu); // no invalid missing-resource blit
    (void)seen_id;(void)seen_slot;(void)seen_f;(void)seen_row;
    (void)seen_col;(void)seen_image;(void)seen_lut;
}
