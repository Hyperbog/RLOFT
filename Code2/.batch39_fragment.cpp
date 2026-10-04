// -----------------------------------------------------------------------------
// Batch 39: 1C0C0..1C408 angle/state normalization, event bitset primitives,
// timed text overlay helpers, dice rolling, and decoded-buffer reads.
// Compiler separators through 1C40B are ledgered separately; next routine 1C40C.
// -----------------------------------------------------------------------------

struct Application39Boundary {
    u32 (*call_30700)()=nullptr;
    void (*call_291b0)(u32)=nullptr;
    void (*call_2939c)(u32,u32,u32,u32)=nullptr;
    void (*call_3060c)(u32,u32,u32,u32)=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    void (*call_3131c)(u32,u32,u32)=nullptr;
    void (*call_312ec_local)(void*,u32,u32)=nullptr;
    void (*call_312ec_dgroup)(u32,u32,u32)=nullptr;
};

static u32 app39_sar32(u32 value,u32 count) noexcept {
    count&=31u;
    if(count==0u)return value;
    u32 out=value>>count;
    if((value&0x80000000u)!=0u)out|=(~0u<<(32u-count));
    return out;
}
static s32 app39_bit_word(s32 bit) noexcept { return bit/32; }
static u32 app39_bit_count(s32 bit) noexcept {
    return static_cast<u32>(static_cast<u8>(bit%32))&31u;
}
static u32 app39_bit_addr(s32 bit) noexcept {
    return 0x22200u+static_cast<u32>(app39_bit_word(bit)*4);
}

void function_1c0c0(Machine& m,u32 raw_value) noexcept {
    const s32 value=static_cast<s32>(raw_value);
    if(*dg(m,0xC02Fu)==0u) {
        store_le32(dg(m,0xC658u),raw_value);
        return;
    }
    const s32 first=value/45;
    const s32 second=(first+1)/8;
    const s32 rem=(first+1)-second*8;
    const s32 half=rem/2;
    store_le32(dg(m,0xC658u),static_cast<u32>(half*90));
}
static_assert(0x1C121u-0x1C0C0u==97u);

void function_1c124(Machine& m,u32 raw_bit) noexcept {
    const s32 bit=static_cast<s32>(raw_bit);
    const u32 count=app39_bit_count(bit);
    const u32 mask=1u<<count;
    const u32 addr=app39_bit_addr(bit);
    store_le32(dg(m,addr),load_le32(dg(m,addr))|mask);
}
static_assert(0x1C160u-0x1C124u==60u);

void function_1c160(Machine& m,u32 raw_bit) noexcept {
    const s32 bit=static_cast<s32>(raw_bit);
    const u32 count=app39_bit_count(bit);
    const u32 mask=1u<<count;
    const u32 addr=app39_bit_addr(bit);
    store_le32(dg(m,addr),load_le32(dg(m,addr))&~mask);
}
static_assert(0x1C19Eu-0x1C160u==62u);

void function_1c1a0(Machine& m,u32 raw_bit) noexcept {
    const s32 bit=static_cast<s32>(raw_bit);
    const u32 count=app39_bit_count(bit);
    const u32 mask=1u<<count;
    const u32 addr=app39_bit_addr(bit);
    store_le32(dg(m,addr),load_le32(dg(m,addr))^mask);
}
static_assert(0x1C1DCu-0x1C1A0u==60u);

[[nodiscard]] u32 function_1c1dc(Machine& m,u32 raw_bit) noexcept {
    const s32 bit=static_cast<s32>(raw_bit);
    const u32 count=app39_bit_count(bit);
    const u32 mask=1u<<count;
    const u32 selected=load_le32(dg(m,app39_bit_addr(bit)))&mask;
    return app39_sar32(selected,count); // SAR returns FFFFFFFFh for selected bit 31.
}
static_assert(0x1C21Au-0x1C1DCu==62u);

void function_1c21c(Machine& m,Application39Boundary& b,u32 src) noexcept {
    const u32 now=b.call_30700?b.call_30700():0u;
    store_le32(dg(m,0xCF54u),now+0x5Au);
    u32 si=src,di=0x14580u;
    for(;;) {
        const u8 first=*dg(m,si);
        *dg(m,di)=first;
        if(first==0u)break;
        const u8 second=*dg(m,si+1u);
        *dg(m,di+1u)=second;
        si+=2u; di+=2u;
        if(second==0u)break;
    }
    *dg(m,0xCF59u)=1u;
}
static_assert(0x1C258u-0x1C21Cu==60u);

void function_1c258(Machine& m,Application39Boundary& b) noexcept {
    if(*dg(m,0xCF59u)==0u)return;
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    if(b.call_2939c)b.call_2939c(0u,0x106u,0x20u,load_le32(dg(m,0x2222Cu)));
    if(b.call_3060c)b.call_3060c(0x107u,0x22u,0x14580u,1u);
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0x106u,0x20u,0x0Cu,0x100u);
    *dg(m,0xCF59u)=0u;
}
static_assert(0x1C2C2u-0x1C258u==106u);

void function_1c2c4(Machine& m,Application39Boundary& b) noexcept {
    function_1c258(m,b);
    const u32 now=b.call_30700?b.call_30700():0u;
    if(now<=load_le32(dg(m,0xCF54u)))return; // encoded unsigned JBE.
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    if(b.call_2939c)b.call_2939c(0u,0x106u,0x20u,load_le32(dg(m,0x2222Cu)));
    store_le32(dg(m,0xCF54u),0xFFFFFFFFu);
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0x106u,0x20u,0x0Cu,0x100u);
}
static_assert(0x1C327u-0x1C2C4u==99u);

[[nodiscard]] u32 function_1c328(Machine&,Application39Boundary& b,
                                  u32 raw_count,u32 sides,u32 per_die,u32 flat) noexcept {
    u32 total=0u;
    s32 i=0;
    const s32 count=static_cast<s32>(raw_count);
    if(count>0) {
        do {
            const u32 random=b.call_30ca2?b.call_30ca2():0u;
            const u32 product=random*sides;
            total+= (product>>15u)+1u;
            ++i;
            total+=per_die;
        } while(i<count);
    }
    total+=flat;
    if(static_cast<s32>(total)<1)total=1u;
    return total;
}
static_assert(0x1C366u-0x1C328u==62u);

void function_1c368(Machine& m,Application39Boundary& b,u32 dest,u32 index) noexcept {
    const u32 handle0=load_le32(dg(m,0x22224u));
    if(b.call_3131c)b.call_3131c(index*4u+4u,0u,handle0);

    u8 offset_bytes[4]{};
    if(b.call_312ec_local)b.call_312ec_local(offset_bytes,4u,load_le32(dg(m,0x22224u)));
    const u32 offset=load_le32(offset_bytes);
    if(b.call_3131c)b.call_3131c(offset,0u,load_le32(dg(m,0x22224u)));

    u8 length_bytes[2]{};
    if(b.call_312ec_local)b.call_312ec_local(length_bytes,2u,load_le32(dg(m,0x22224u)));
    u16 length=load_le16(length_bytes);
    if(b.call_312ec_dgroup)b.call_312ec_dgroup(dest,static_cast<u32>(length),load_le32(dg(m,0x22224u)));

    *dg(m,dest+static_cast<u32>(length))=0u;
    while(length!=0u) {
        *dg(m,dest+static_cast<u32>(length)-1u)^=0xDCu;
        length=static_cast<u16>(length-1u);
    }
}
static_assert(0x1C409u-0x1C368u==161u);
static_assert((0x1C121u-0x1C0C0u)+(0x1C160u-0x1C124u)+
              (0x1C19Eu-0x1C160u)+(0x1C1DCu-0x1C1A0u)+
              (0x1C21Au-0x1C1DCu)+(0x1C258u-0x1C21Cu)+
              (0x1C2C2u-0x1C258u)+(0x1C327u-0x1C2C4u)+
              (0x1C366u-0x1C328u)+(0x1C409u-0x1C368u)==829u);
