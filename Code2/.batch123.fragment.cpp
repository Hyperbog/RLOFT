// Batch 123: PC speaker PIT channel-2 programming, 372F6..37347.
struct Runtime123Boundary {
    u8 (*in8)(u16)=nullptr;
    void (*out8)(u16,u8)=nullptr;
};

void function_372f6(Runtime123Boundary& b,u32 divisor) noexcept {
    if(divisor<=0x12u) return;
    const u32 counter=0x1234DCu/divisor;
    u8 gate=b.in8?b.in8(0x61u):0u;
    if((gate&3u)!=3u) {
        gate=static_cast<u8>(gate|3u);
        if(b.out8)b.out8(0x61u,gate);
        if(b.out8)b.out8(0x43u,0xB6u);
    }
    if(b.out8)b.out8(0x42u,static_cast<u8>(counter));
    if(b.out8)b.out8(0x42u,static_cast<u8>(counter>>8));
}
static_assert(0x37339u-0x372F6u==67u);

void function_37339(Runtime123Boundary& b) noexcept {
    const u8 gate=static_cast<u8>((b.in8?b.in8(0x61u):0u)&0xFCu);
    if(b.out8)b.out8(0x61u,gate);
}
static_assert(0x37348u-0x37339u==15u);
