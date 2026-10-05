// Batch 89: 333BC..334C1 offset-biased companion record materialization.
void function_333bc(Machine& m,Application86Boundary& b) noexcept {
    const u32 source=load_le32(dg(m,0x255F0u));
    const u32 bias=static_cast<u32>(load_le16(dg(m,0xFCE4u)))-0x8000u;
    u32 adjusted=source+bias;
    if(load_le16(dg(m,0x255F0u))>=0x8000u)adjusted=source-bias;
    const u32 record_offset=(adjusted>>4)&0x0FC0u;

    const u32 index2=load_le32(dg(m,0xFCDCu))*2u;
    const u32 flags_addr=0x198E0u+index2;
    const u16 flags=static_cast<u16>(load_le16(dg(m,flags_addr))|0x0440u);
    store_le16(dg(m,flags_addr),flags);
    const u32 high_class=(static_cast<u32>(flags)&0xC000u)>>14;
    const u32 depth=b.call_35730?b.call_35730():0u;

    const u32 rec=0x15A18u+load_le32(dg(m,0xC6FCu))*0x1Cu;
    store_le32(dg(m,rec+0x00u),depth);
    store_le32(dg(m,rec+0x18u),high_class);
    store_le32(dg(m,rec+0x10u),record_offset);
    store_le32(dg(m,rec+0x0Cu),load_le32(dg(m,0xFCD4u)));
    store_le32(dg(m,rec+0x14u),load_le32(dg(m,0xFCDCu)));
    const u32 table_index=load_le32(dg(m,0xC700u))*2u;
    store_le32(dg(m,rec+0x04u),static_cast<u32>(load_le16(dg(m,0x256A2u+table_index))));
    store_le32(dg(m,rec+0x08u),static_cast<u32>(load_le16(dg(m,0x25622u+table_index))));

    if(load_le32(dg(m,rec+0x08u))==0x93u)store_le32(dg(m,0x22220u),0u);
    const s32 signed_depth=static_cast<s32>(depth);
    if(signed_depth<static_cast<s32>(load_le32(dg(m,0x22220u))))store_le32(dg(m,0x22220u),depth);
    if(signed_depth>static_cast<s32>(load_le32(dg(m,0xBF34u))))store_le32(dg(m,0xBF34u),depth);
}
static_assert(0x334C2u-0x333BCu==262u);
// 334C2..334C3 is XCHG EBX,EBX alignment with no incoming direct control-flow target.
