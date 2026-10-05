// Batch 75: 31C58..31E71 two-phase packed shading/lookup generator.
void function_31c58(Machine& m,s32 arg) noexcept {
    const s32 shade=3;
    const s32 multiplier=4;
    const s32 period=shade+1;
    const s32 limit=static_cast<s32>(load_le32(dg(m,0xC730u))+1u);
    const s32 left=app74_wrap_add(app74_wrap_mul(multiplier,0x1E),app74_wrap_mul(arg,5));
    const s32 right=app74_wrap_add(app74_wrap_mul(left,3),-app74_wrap_mul(arg,5));
    const s32 middle=app74_wrap_add(app74_wrap_add(limit,-left),-right);
    *dg(m,0xF918u)=static_cast<u8>(shade);

    const u32 table=load_le32(dg(m,0x258C4u));
    u16 last=0u;
    for(s32 i=0;i<limit;++i){
        s8 lo;
        if(i<left)lo=0;
        else if(i>=app74_wrap_add(limit,-right))lo=9;
        else {
            const s32 n=app74_wrap_add(app74_wrap_add(app74_wrap_mul(app74_wrap_add(i,-left),8),8),app74_sar1(middle));
            lo=static_cast<s8>(n/middle);
        }
        lo=app74_nonneg(lo);
        s8 hi=lo;
        lo=app74_cap9(lo); hi=app74_cap9(hi);
        const s32 half_period=limit/app74_wrap_mul(period,2);
        const s32 phase=i/half_period;
        if((static_cast<u8>(phase)&1u)!=0u)--hi;
        if(static_cast<s32>(lo)<=2)hi=lo;
        if(lo==8&&hi==9)hi=8;
        last=static_cast<u16>((static_cast<u32>(static_cast<s32>(hi))<<8)|static_cast<u32>(static_cast<s32>(lo)));
        store_le16(dg(m,table+static_cast<u32>(i)*2u),last);
    }
    store_le16(dg(m,0xFCFAu),last);

    const s32 left2=shade*5;
    const s32 right2=left2*3;
    const s32 middle2=0x81-left2-right2;
    for(s32 i=0;i<0x80;++i){
        s8 value;
        if(i<left2)value=0;
        else if(i>=0x81-right2)value=8;
        else {
            const s32 n=app74_wrap_add(app74_wrap_mul(app74_wrap_add(app74_wrap_add(i,-left2),1),7),app74_sar1(middle2));
            value=static_cast<s8>(n/middle2);
        }
        if(static_cast<s32>(value)>8)value=8;
        *dg(m,0xFE94u+static_cast<u32>(i))=static_cast<u8>(value);
        *dg(m,0xFE14u+static_cast<u32>(0x7F-i))=static_cast<u8>(value);
    }
}
static_assert(0x31E72u-0x31C58u==538u);
// 31E72..31E73 is MOV EAX,EAX alignment with no incoming direct control-flow target.
