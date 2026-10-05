// Batch 93: 33808..338C4 record decomposition and weighted bucket selection.
void function_33808(Machine& m,u32 table_index,u32 threshold,u32 record_index,
                    u32 out_value,u32 out_shifted,u32 out_bucket,
                    u32 out_remainder,u32 out_quotient) noexcept {
    const u32 record_offset=record_index*0x1Cu;
    store_le32(dg(m,out_value),load_le32(dg(m,0x15A20u+record_offset)));
    store_le32(dg(m,out_shifted),load_le32(dg(m,0x15A28u+record_offset))>>6);

    const u32 numerator=load_le32(dg(m,0x15A2Cu+record_offset));
    const u32 divisor=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x25619u)))));
    const u32 quotient=numerator/divisor;
    const u32 remainder=numerator%divisor;
    store_le32(dg(m,out_remainder),remainder);
    store_le32(dg(m,out_quotient),quotient);
    store_le16(dg(m,out_bucket),0xFFFFu);

    if(table_index>=load_le32(dg(m,0xC738u)))return;
    const u32 shape=load_le32(dg(m,0x15618u+table_index*4u));
    if(shape==0u)return;

    const u32 weights=load_le32(dg(m,0x18FE0u+table_index*4u))-0x102u;
    const u32 bias=(shape>>3)+0x84u-(shape>>1);
    u32 sum=0u;
    for(u32 bucket=0u;bucket<0x80u;++bucket){
        sum+=static_cast<u32>(load_le16(dg(m,weights+bucket*2u)));
        const u32 probe=sum+bias;
        if(static_cast<s32>(probe)>=static_cast<s32>(threshold)){
            store_le32(dg(m,out_bucket),bucket);
            return;
        }
    }
}
static_assert(0x338C5u-0x33808u==189u);
// 338C5..338C7 is XCHG EBX,EBX plus NOP alignment with no incoming direct target.
