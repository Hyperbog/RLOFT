// Batch 74: 318B4..31C56 packed shading lookup generators.
static s32 app74_wrap_add(s32 a,s32 b) noexcept{return static_cast<s32>(static_cast<u32>(a)+static_cast<u32>(b));}
static s32 app74_wrap_mul(s32 a,s32 b) noexcept{return static_cast<s32>(static_cast<u32>(a)*static_cast<u32>(b));}
static s32 app74_sar1(s32 v) noexcept{return v>=0?v/2:-static_cast<s32>((static_cast<u32>(-static_cast<s64>(v))+1u)/2u);}
static s8 app74_nonneg(s8 v) noexcept{return v<0?0:v;}
static s8 app74_cap9(s8 v) noexcept{return v>9?9:v;}
static u16 app74_pack(s8 hi,s8 lo) noexcept {
    const u32 h=static_cast<u32>(static_cast<s32>(hi));
    const u32 l=static_cast<u32>(static_cast<s32>(lo));
    return static_cast<u16>((h<<8)+l);
}

void function_318b4(Machine& m,u32 arg) noexcept {
    const s32 span=static_cast<s32>(load_le32(dg(m,0xC738u)));
    const s32 stride=static_cast<s32>(arg+9u);
    *dg(m,0xF918u)=static_cast<u8>(arg);
    const s32 limit=static_cast<s32>(load_le32(dg(m,0xC730u))+1u);
    const u32 table=load_le32(dg(m,0x258C4u));
    for(s32 i=0;i<limit;++i){
        s8 lo=8,hi=8;
        if(i<span){
            const s32 num=app74_wrap_add(app74_wrap_mul(i,stride),app74_sar1(span));
            lo=app74_nonneg(static_cast<s8>(num/span));
            hi=lo;
            lo=app74_cap9(lo); hi=app74_cap9(hi);
            const s32 half_period=span/app74_wrap_mul(stride,2);
            const s32 phase=i/half_period;
            if((static_cast<u8>(phase)&1u)!=0u)--hi;
            if(static_cast<s32>(lo)<=2)hi=lo;
            if(lo==8&&hi==9)hi=8;
        }
        store_le16(dg(m,table+static_cast<u32>(i)*2u),app74_pack(hi,lo));
        if(i<0x80){
            if(lo>8)lo=8;
            *dg(m,0xFE14u+static_cast<u32>(0x7F-i))=static_cast<u8>(lo);
            *dg(m,0xFE94u+static_cast<u32>(i))=static_cast<u8>(lo);
        }
    }
    store_le16(dg(m,0xFCFAu),0x0808u);
}
static_assert(0x31A12u-0x318B4u==350u);
// 31A12..31A13 is MOV EAX,EAX alignment.

void function_31a14(Machine& m,s32 arg,s32 fade) noexcept {
    if(fade>8)fade=8;
    const s32 step=app74_wrap_add(arg,1);
    const s32 limit=static_cast<s32>(load_le32(dg(m,0xC730u))+1u);
    const s32 fade1=app74_wrap_add(fade,1);
    const s32 denom=app74_wrap_add(10,-fade1);
    const s32 span=static_cast<s32>(load_le32(dg(m,0xC738u)));
    const s32 used=app74_wrap_mul(fade,span/denom);
    const s32 remaining=app74_wrap_add(span,-used);
    const s32 threshold=app74_wrap_add(static_cast<s32>(load_le32(dg(m,0xC730u))),-remaining);
    *dg(m,0xF918u)=static_cast<u8>(arg);
    const u32 table=load_le32(dg(m,0x258C4u));
    u16 last=0u;
    for(s32 i=0;i<limit;++i){
        const s32 term=app74_wrap_mul(app74_wrap_add(app74_wrap_add(limit,-i),1),step);
        const s32 num=app74_wrap_add(term,app74_sar1(limit));
        s8 lo=app74_nonneg(static_cast<s8>(num/limit));
        const s32 scale=static_cast<s16>(load_le16(dg(m,0xF954u+static_cast<u32>(arg)*2u)));
        const s32 scaled_num=app74_wrap_add(app74_sar1(scale),static_cast<s32>(lo)*5);
        lo=static_cast<s8>(scaled_num/scale);
        s8 hi=lo;
        lo=app74_cap9(lo); hi=app74_cap9(hi);
        if(fade!=0&&i!=0){
            s32 adjust=0;
            if(i>remaining){
                const s32 dist=app74_wrap_add(i,-remaining);
                const s32 n=app74_wrap_add(app74_wrap_mul(dist,8),app74_sar1(threshold));
                adjust=n/threshold;
            }
            lo=static_cast<s8>(static_cast<u8>(lo)-static_cast<u8>(adjust));
            hi=static_cast<s8>(static_cast<u8>(hi)-static_cast<u8>(adjust));
            lo=app74_nonneg(lo); hi=app74_nonneg(hi);
        }
        const s32 half_period=limit/app74_wrap_mul(step,2);
        const s32 phase=i/half_period;
        if((static_cast<u8>(phase)&1u)!=0u)--hi;
        if(static_cast<s32>(lo)<=2)hi=lo;
        if(lo==8&&hi==9)hi=8;
        last=app74_pack(hi,lo);
        store_le16(dg(m,table+static_cast<u32>(i)*2u),last);
        if(lo>8)lo=8;
        if(i>=0x46&&i<0xC6)*dg(m,0xFE4Eu+static_cast<u32>(i))=static_cast<u8>(lo);
        if(i>=0x32&&i<0xB2)*dg(m,0xFE14u+static_cast<u32>(0xB1-i))=static_cast<u8>(lo);
    }
    store_le16(dg(m,0xFCFAu),last);
}
static_assert(0x31C57u-0x31A14u==579u);
// 31C57 is NOP alignment.
