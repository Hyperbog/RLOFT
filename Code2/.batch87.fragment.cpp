// Batch 87: 33124..332B2 right-sweep record materialization.
void function_33124(Machine& m,Application86Boundary& b) noexcept {
    const u32 base=(load_le32(dg(m,0x255F0u))>>4)&0x0FC0u;
    u32 record_offset=base;
    if(load_le32(dg(m,0x25604u))==0xFFFF0000u){
        store_le16(dg(m,0x255EAu),static_cast<u16>(load_le16(dg(m,0x255EAu))+1u));
    }else{
        record_offset=0x0FC0u-base;
    }

    const u32 depth=b.call_35730?b.call_35730():0u;
    u32 cell_type=load_le32(dg(m,0xC700u));
    u32 extra=0u;
    u32 index2=load_le32(dg(m,0xFCDCu))*2u;
    u32 flags_addr=0x198E0u+index2;
    u16 flags=static_cast<u16>(load_le16(dg(m,flags_addr))|0x0440u);
    store_le16(dg(m,flags_addr),flags);
    const u32 high_class=(static_cast<u32>(flags)&0xC000u)>>14;

    if((flags&0x0100u)!=0u){
        const u32 delta=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x25606u)))))<<1;
        const u32 alternate_index2=index2-delta;
        store_le16(dg(m,0x248E6u),load_le16(dg(m,0x255F2u)));
        const u16 alternate=load_le16(dg(m,0x198E0u+alternate_index2));
        if((alternate&0x0080u)!=0u){
            cell_type=static_cast<u32>(alternate&0x003Fu);
            record_offset=base;
            extra=1u;
        }
    }

    index2=load_le32(dg(m,0xFCDCu))*2u;
    flags_addr=0x198E0u+index2;
    flags=static_cast<u16>(load_le16(dg(m,flags_addr))|0x0440u);
    store_le16(dg(m,flags_addr),flags);
    if((flags&0x1000u)!=0u && extra==0u){
        extra=(b.call_1e9a0?b.call_1e9a0(load_le32(dg(m,0xFCDCu))):0u)&0xFFFFu;
    }

    const u32 rec=0x15A18u+load_le32(dg(m,0xC6FCu))*0x1Cu;
    store_le32(dg(m,rec+0x18u),high_class);
    store_le32(dg(m,rec+0x00u),depth);
    store_le32(dg(m,rec+0x10u),record_offset);
    const u32 table_index=cell_type*2u;
    store_le32(dg(m,rec+0x04u),static_cast<u32>(load_le16(dg(m,0x256A2u+table_index)))+extra);
    store_le32(dg(m,rec+0x08u),static_cast<u32>(load_le16(dg(m,0x25622u+table_index)))+extra);
    store_le32(dg(m,rec+0x0Cu),load_le32(dg(m,0xFCD4u)));
    store_le32(dg(m,rec+0x14u),load_le32(dg(m,0xFCDCu)));

    if(load_le32(dg(m,rec+0x08u))==0x93u)store_le32(dg(m,0x22220u),0u);
    const s32 signed_depth=static_cast<s32>(depth);
    if(signed_depth<static_cast<s32>(load_le32(dg(m,0x22220u))))store_le32(dg(m,0x22220u),depth);
    if(signed_depth>static_cast<s32>(load_le32(dg(m,0xBF34u))))store_le32(dg(m,0xBF34u),depth);
}
static_assert(0x332B3u-0x33124u==399u);
// 332B3 is an unreferenced NOP alignment byte.
