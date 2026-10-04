// -----------------------------------------------------------------------------
// Batch 37: 1BBD8..1BD98 moving-record step/retry wrapper.
// 1BD99..1BD9B is compiler LEA alignment. The next ownership unit starts with
// the four-entry table at 1BD9C for the routine beginning at 1BDAC.
// -----------------------------------------------------------------------------

void function_1bbd8(Machine& m,Application36Boundary& b,
                     u32 rec,u32 movement_desc,u32 context) noexcept {
    u32 attempt=0u;
    if((*dg(m,rec+0x0Bu)&0x40u)!=0u) {
        if(load_le16(dg(m,rec+0x12u))==0u) {
            function_1b760(m,b,rec,context,1u);
        } else {
            store_le16(dg(m,rec+0x12u),static_cast<u16>(load_le16(dg(m,rec+0x12u))-1u));
            *dg(m,rec+0x14u)=static_cast<u8>(*dg(m,rec+0x14u)|1u);
        }
    } else {
        function_1b760(m,b,rec,context,attempt);
    }

    while((*dg(m,rec+0x14u)&1u)!=0u) {
        const u32 type=load_le16(dg(m,rec+0x0Eu));
        if(type==9u) {
            if(static_cast<s16>(load_le16(dg(m,rec+6u)))==0x18&&
               static_cast<s16>(load_le16(dg(m,rec+2u)))==7) {
                *dg(m,rec+0x14u)=static_cast<u8>(*dg(m,rec+0x14u)&0xFEu);
                continue;
            }
            const u16 y=load_le16(dg(m,rec+2u));
            const u16 width=load_le16(dg(m,0x25619u));
            store_le16(dg(m,rec+6u),static_cast<u16>(load_le16(dg(m,rec+6u))+1u));
            const u16 tile=static_cast<u16>(static_cast<u16>(y*width)+load_le16(dg(m,rec+6u)));
            store_le16(dg(m,rec+0x0Cu),tile);
            *dg(m,rec+0x14u)=static_cast<u8>(*dg(m,rec+0x14u)&0xFEu);
            continue;
        }

        const u16 flags=load_le16(dg(m,rec+0x14u));
        const bool displaced=(*dg(m,0xC02Fu)==0u)||((app36_orient3(flags)&1u)==0u);
        u32 next4=load_le32(dg(m,rec+4u));
        u32 next0=load_le32(dg(m,rec));
        if(displaced) {
            const u32 orient=app36_orient3(flags);
            const u32 radius=load_le16(dg(m,movement_desc+0x29u+orient*2u));
            const u32 dir=app36_dir3(flags);
            u32 angle=(8u-dir)*45u;
            if(angle>=360u)angle-=360u;
            u32 delta4=0u,delta0=0u;
            if(b.call_2df2c_local)b.call_2df2c_local(&delta4,&delta0,radius,angle);
            next4+=delta4;
            next0+=delta0;
        }

        const u32 ok=b.call_2d5d0?b.call_2d5d0(next0,next4,context):0u;
        if(static_cast<u16>(ok)!=0u) {
            const u16 width=load_le16(dg(m,0x25619u));
            const u16 hi0=static_cast<u16>(next0>>16u);
            const u16 hi4=static_cast<u16>(next4>>16u);
            const u16 tile=static_cast<u16>(static_cast<u16>(hi0*width)+hi4);
            store_le32(dg(m,rec+4u),next4);
            store_le32(dg(m,rec),next0);
            store_le16(dg(m,rec+0x0Cu),tile);

            const u32 divisor=*dg(m,movement_desc+0x19u);
            if(divisor==0u) {
                if(b.divide_error)b.divide_error();
                return;
            }
            const u32 orient=(app36_orient3(load_le16(dg(m,rec+0x14u)))+1u)%divisor;
            store_le16(dg(m,rec+0x14u),app36_set_orient3(load_le16(dg(m,rec+0x14u)),orient&7u));
            *dg(m,rec+0x14u)=static_cast<u8>(*dg(m,rec+0x14u)&0xFEu);
            continue;
        }

        if(type==8u) {
            *dg(m,rec+0x15u)=static_cast<u8>(*dg(m,rec+0x15u)|0xC0u);
            store_le16(dg(m,rec+0x14u),static_cast<u16>(load_le16(dg(m,rec+0x14u))&0xFE3Eu));
            *dg(m,rec+0x14u)=static_cast<u8>(*dg(m,rec+0x14u)&0xFEu);
            continue;
        }

        if(attempt<3u) {
            ++attempt;
            *dg(m,rec+0x15u)=static_cast<u8>(*dg(m,rec+0x15u)&0x3Fu);
            function_1b760(m,b,rec,context,attempt);
            continue;
        }

        *dg(m,rec+8u)=static_cast<u8>(*dg(m,rec+8u)&0xEFu);
        *dg(m,rec+0x14u)=static_cast<u8>(*dg(m,rec+0x14u)&0xFEu);
    }
}
static_assert(0x1BD99u-0x1BBD8u==449u);
