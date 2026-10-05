// Batch 114: 35680..35868 fixed-point multiply/projection helpers.
// 35797 is alignment; 35869..35877 is runtime constant/alignment data.
static u32 app114_mul_fraction(u32 value,u32 factor,bool use_factor_sign) noexcept {
    const u16 bx=static_cast<u16>(factor);
    u16 si=use_factor_sign?static_cast<u16>(factor>>16):0u;
    if(static_cast<s16>(value>>16)<0) {
        value=0u-value;                                  // NEG dword [EBP+8]
        si=static_cast<u16>(si^0x8000u);
    }

    u32 product=static_cast<u32>(static_cast<u16>(value))*static_cast<u32>(bx);
    const u16 di=static_cast<u16>(product>>16);
    product=static_cast<u32>(static_cast<u16>(value>>16))*static_cast<u32>(bx);
    u16 ax=static_cast<u16>(product);
    u16 dx=static_cast<u16>(product>>16);
    const u32 sum=static_cast<u32>(ax)+static_cast<u32>(di);
    ax=static_cast<u16>(sum);
    dx=static_cast<u16>(static_cast<u32>(dx)+(sum>>16));

    if(static_cast<s16>(si)<0) {
        dx=static_cast<u16>(0u-static_cast<u32>(dx));
        const bool borrow=(ax!=0u);                      // CF from NEG AX
        ax=static_cast<u16>(0u-static_cast<u32>(ax));
        dx=static_cast<u16>(static_cast<u32>(dx)-(borrow?1u:0u));
    }
    return (static_cast<u32>(dx)<<16)|static_cast<u32>(ax);
}

// 35680..356D7: factor sign comes from the high word of the second dword.
[[nodiscard]] u32 function_35680(u32 value,u32 factor) noexcept {
    return app114_mul_fraction(value,factor,true);
}
static_assert(0x356D8u-0x35680u==88u);

// 356D8..3572F: same 16-bit multiply core, but the factor begins with SI=0.
[[nodiscard]] u32 function_356d8(u32 value,u32 factor) noexcept {
    return app114_mul_fraction(value,factor,false);
}
static_assert(0x35730u-0x356D8u==88u);

// 35730..35796: derive a bounded unsigned divisor from two fixed-point products,
// then return only the low 16 bits of the encoded unsigned quotient.
[[nodiscard]] u32 function_35730(Machine& m) noexcept {
    const u32 a=load_le32(dg(m,0x255F0u))-load_le32(dg(m,0x255ECu));
    const u32 b=load_le32(dg(m,0x255E8u))-load_le32(dg(m,0x255F4u));
    const u32 first=function_35680(a,load_le32(dg(m,0x255F8u)));
    const u32 second=function_35680(b,load_le32(dg(m,0x255FCu)));
    u32 delta=first-second;
    if(static_cast<s32>(delta)<static_cast<s32>(0x1680u))delta=0x1680u;
    const u32 divisor=delta>>8;
    const u32 q=load_le32(dg(m,0x255E0u))/divisor;
    return q&0xFFFFu;
}
static_assert(0x35797u-0x35730u==103u);

// 35798..35868: PUSHA/POPA makes all GPR changes temporary; the only observable
// outputs are the two word stores addressed by the third and fourth arguments.
void function_35798(Machine& m,u32 x,u32 y,u32 out_y,u32 out_scale) noexcept {
    const u32 ax=x-load_le32(dg(m,0x255ECu));
    const u32 bx=y-load_le32(dg(m,0x255F4u));
    const u32 p0=function_35680(ax,load_le32(dg(m,0x255F8u)));
    const u32 p1=function_35680(bx,load_le32(dg(m,0x255FCu)));
    const u32 depth=p0-p1-0x2000u;
    const u32 p2=function_35680(ax,load_le32(dg(m,0x255FCu)));
    const u32 p3=function_35680(bx,load_le32(dg(m,0x255F8u)));
    const u32 lateral=p2+p3;

    if(static_cast<s32>(depth)<static_cast<s32>(0x5800u)) {
        store_le16(dg(m,out_scale),0u);
        return;
    }

    const std::int64_t product=static_cast<std::int64_t>(static_cast<s32>(load_le32(dg(m,0xC6CCu)))) *
                               static_cast<std::int64_t>(static_cast<s32>(lateral));
    const std::int64_t depth_signed=static_cast<std::int64_t>(static_cast<s32>(depth));
    const std::int64_t q0=product/depth_signed;          // signed IDIV truncates toward zero
    const u16 screen=static_cast<u16>(load_le16(dg(m,0xC6D0u))+static_cast<u16>(q0));
    store_le16(dg(m,out_y),screen);

    const s32 scaled_depth=static_cast<s32>(depth)>>8;
    const std::int64_t q1=static_cast<std::int64_t>(static_cast<s32>(load_le32(dg(m,0x255E0u)))) /
                          static_cast<std::int64_t>(scaled_depth);
    store_le16(dg(m,out_scale),static_cast<u16>(q1));
}
static_assert(0x35869u-0x35798u==209u);

// 35797 is NOP alignment. 35869..3586F is zero alignment; 35870 and 35874
// are separately referenced dwords containing 00002020h.
static_assert(0x35878u-0x35869u==15u);
