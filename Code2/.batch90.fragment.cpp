// Batch 90: 334C4..335DB view/grid initialization before the record sweep.
struct Application90Boundary {
    u32 (*call_35680)(u32,u32)=nullptr;
    void (*call_32954)()=nullptr;
};

void function_334c4(Machine& m,Application90Boundary& b) noexcept {
    const u32 cell_count=static_cast<u32>(load_le16(dg(m,0x25619u)))*
                         static_cast<u32>(load_le16(dg(m,0x2561Bu)));
    store_le32(dg(m,0xFD0Cu),cell_count);

    u32 index=0u,count=cell_count;
    do{
        const u32 a=0x198E0u+index*2u;
        store_le16(dg(m,a),static_cast<u16>(load_le16(dg(m,a))&0xFFBFu));
        ++index;
        --count;
    }while(count!=0u);

    const u32 view_index=load_le32(dg(m,0xC658u));
    store_le32(dg(m,0xC714u),view_index*10u);
    const u32 table=load_le32(dg(m,0xC65Cu));
    store_le32(dg(m,0x255FCu),load_le32(dg(m,table+view_index*4u)));
    store_le32(dg(m,0x255F8u),load_le32(dg(m,table+(view_index+0x5Au)*4u)));

    const u32 vertical=b.call_35680?b.call_35680(load_le32(dg(m,0x2560Cu)),load_le32(dg(m,0x255F8u))):0u;
    const u32 vertical_delta=load_le32(dg(m,0x255D4u))-vertical;
    store_le32(dg(m,0x255ECu),vertical_delta);
    const u32 vertical_low=vertical_delta&0xFFFFu;
    store_le32(dg(m,0xC70Cu),vertical_low);
    store_le32(dg(m,0xC710u),0u-vertical_low);

    const u32 horizontal=b.call_35680?b.call_35680(load_le32(dg(m,0x2560Cu)),load_le32(dg(m,0x255FCu))):0u;
    const u32 horizontal_sum=load_le32(dg(m,0x255D0u))+horizontal;
    store_le32(dg(m,0x255F4u),horizontal_sum);
    const u32 horizontal_low=horizontal_sum&0xFFFFu;
    store_le32(dg(m,0xC704u),horizontal_low);
    store_le32(dg(m,0xC708u),0u-horizontal_low);

    store_le32(dg(m,0xC724u),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255EEu))))));
    store_le32(dg(m,0xC720u),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255F6u))))));
    store_le32(dg(m,0xC71Cu),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))))));
    store_le32(dg(m,0xC718u),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))))));
    store_le32(dg(m,0xC72Cu),0xFFFFFFFFu);
    store_le32(dg(m,0x22220u),0x00000800u);
    if(b.call_32954)b.call_32954();
}
static_assert(0x335DCu-0x334C4u==280u);
