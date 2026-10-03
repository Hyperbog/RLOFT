static void application06_gate_nibbles(Machine& m,u32 base,u32 phase,u32 table) noexcept {
    const u32 selector=(phase-1u)*8u;
    for(u32 row=0;row<8u;++row) {
        const bool enabled=*dg(m,table+selector+row)!=0u;
        const s32 count=static_cast<s32>(load_le32(dg(m,0x6420u+row*4u)));
        if(count<=0)continue;
        for(s32 i=0;i<count;++i) {
            u8& cell=*dg(m,base+0x2199Au+row*10u+static_cast<u32>(i));
            const u8 low=cell&0x0Fu;
            if(enabled){if(low==0x0Fu)cell=0u;}
            else if(low!=0x0Fu)cell=0xFFu;
        }
    }
}

void function_03de4(Machine& m,u32 slot,u32 phase) noexcept {
    const u32 base=slot*0x248u;*dg(m,base+0x219A2u)=0x11u;application06_gate_nibbles(m,base,phase,0x62B0u);
}
static_assert(0x03EA4u-0x03DE4u==192u);

void function_03ea4(Machine& m,u32 slot,u32 phase) noexcept {
    const u32 base=slot*0x248u;*dg(m,base+0x219A1u)=0x11u;if(static_cast<s32>(phase-1u)>=3)*dg(m,base+0x219A2u)=0x11u;application06_gate_nibbles(m,base,phase,0x6350u);
}
static_assert(0x03F75u-0x03EA4u==209u);

void function_03f78(Machine& m,Application06Boundary& b,Application04Boundary& a4,u32 text0,u32 text1) noexcept {
    if(a4.call_291b0)a4.call_291b0(*dg(m,0xC050u));
    if(b.call_1c40c)b.call_1c40c(load_le32(dg(m,0xC050u)),0x50u,0x50u,0xF0u,0xA0u);
    if(a4.call_30508)a4.call_30508(0xC4u,0x50u,0xA0u,text0,1u);
    if(a4.call_30508)a4.call_30508(0xCEu,0x50u,0xA0u,text1,1u);
    if(a4.call_1a510)a4.call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(a4.call_1a1f4)while(a4.call_1a1f4()!=0u){}
    for(;;){const u32 pressed=a4.call_1a1f4?a4.call_1a1f4():1u;if(pressed!=0u)break;if(b.call_30b40&&b.call_30b40()!=0u)break;}
    for(;;){if(a4.call_1a1f4)while(a4.call_1a1f4()!=0u){}if(!b.call_30b40||b.call_30b40()==0u)break;}
    if(a4.call_1a5b0)a4.call_1a5b0(load_le32(dg(m,0xC050u)),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x8600u))))),*dg(m,0x8602u));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
}
static_assert(0x04040u-0x03F78u==200u);

[[nodiscard]] u32 function_04040(Machine& m,Application06Boundary& b,Application03Boundary& a3,Application04Boundary& a4) noexcept {
    u32 x=load_le32(dg(m,0x255D0u)),y=load_le32(dg(m,0x255D4u));
    for(u32 angle=0;angle<0x168u;++angle) {
        u32 dy=0u,dx=0u;if(a3.call_2df2c)a3.call_2df2c(&dx,&dy,0x5000u,angle);
        for(;;){x+=dx;y+=dy;const s32 row=static_cast<s16>(static_cast<u16>(y>>16));const s32 col=static_cast<s16>(static_cast<u16>(x>>16));const s32 width=static_cast<s16>(load_le16(dg(m,0x25619u)));const u32 index=static_cast<u32>(row*width+col);const u32 tile=0x198E0u+index*2u;
            bool probe=(load_le16(dg(m,tile))&0x3Fu)==0u;
            if(!probe&&(*dg(m,tile)&0x80u)!=0u)probe=static_cast<u16>(a4.call_1e930?a4.call_1e930(index):0u)!=0x8000u;
            if(!probe)break;
            if(b.call_2c8e4&&b.call_2c8e4(y,x,0xFFFFFFFFu)!=0u)return 0u;
        }
    }
    return 1u;
}
static_assert(0x040FFu-0x04040u==191u);

[[nodiscard]] u32 function_04100(Machine& m,Application06Boundary& b,Application03Boundary& a3,Application04Boundary& a4) noexcept {
    store_le32(dg(m,0x6688u),0u);
    const s32 px=static_cast<s16>(load_le16(dg(m,0x255D2u))),py=static_cast<s16>(load_le16(dg(m,0x255D6u)));
    if(load_le32(dg(m,0xC6C8u))==0x1Du&&px>0x41)return 1u;
    if(load_le32(dg(m,0xC6C8u))==0x0Bu&&px>=4&&px<=7&&py>=7&&py<=0x0B)return 0u;
    if(function_04040(m,b,a3,a4)!=0u){store_le32(dg(m,0x6688u),1u);return 0u;}
    s32 i=0;u32 off=0u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A2u))))) {
        if((*dg(m,0x14D8Bu+off)&2u)==0u) {
            const u32 type=load_le16(dg(m,0x14D8Eu+off));
            if((a3.call_2acdc?a3.call_2acdc(type):0u)!=0u) {
                const u32 tile=load_le16(dg(m,0x14D8Cu+off));
                if((*dg(m,0x198E0u+tile*2u)&0x40u)!=0u)return 1u;
                s32 d=static_cast<s16>(load_le16(dg(m,0x14D82u+off)))-py;if(d<0)d=-d;
                if(d<15){d=static_cast<s16>(load_le16(dg(m,0x14D86u+off)))-px;if(d<0)d=-d;if(d<15&&(*dg(m,0x198E1u+tile*2u)&4u)!=0u)return 1u;}
            }
        }
        off+=0x16u;++i;
    }
    return 0u;
}
static_assert(0x04225u-0x04100u==293u);

[[nodiscard]] u32 function_04228(Machine& m,Application06Boundary& b) noexcept {
    for(u32 type_off=0u;type_off<6u;type_off+=2u) {
        const u32 wanted=load_le16(dg(m,0x258AAu+type_off));
        s32 i=0;u32 off=0u;
        while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A2u))))) {
            bool moved=false;
            if((*dg(m,0x14D8Bu+off)&2u)==0u&&(*dg(m,0x198E0u+static_cast<u32>(load_le16(dg(m,0x14D8Cu+off)))*2u)&0x40u)==0u&&load_le16(dg(m,0x14D8Eu+off))==wanted) {
                s32 dx=static_cast<s16>(load_le16(dg(m,0x14D86u+off)))-static_cast<s16>(load_le16(dg(m,0x255D2u)));if(dx<0)dx=-dx;
                if(dx>10){s32 dy=static_cast<s16>(load_le16(dg(m,0x14D82u+off)))-static_cast<s16>(load_le16(dg(m,0x255D6u)));if(dy<0)dy=-dy;if(dy>10){
                    const u32 old0=load_le32(dg(m,0x14D80u+off)),old4=load_le32(dg(m,0x14D84u+off));
                    store_le32(dg(m,0x14D84u+off),load_le32(dg(m,0x255D0u)));store_le32(dg(m,0x14D80u+off),load_le32(dg(m,0x255D4u)));
                    store_le16(dg(m,0x14D82u+off),static_cast<u16>(load_le16(dg(m,0x14D82u+off))+1u));
                    moved=b.call_2d5d0&&b.call_2d5d0(load_le32(dg(m,0x14D80u+off)),load_le32(dg(m,0x14D84u+off)),static_cast<u32>(i))!=0u;
                    if(!moved){store_le32(dg(m,0x14D80u+off),load_le32(dg(m,0x255D4u)));store_le16(dg(m,0x14D82u+off),static_cast<u16>(load_le16(dg(m,0x14D82u+off))-1u));moved=b.call_2d5d0&&b.call_2d5d0(load_le32(dg(m,0x14D80u+off)),load_le32(dg(m,0x14D84u+off)),static_cast<u32>(i))!=0u;}
                    if(!moved){store_le32(dg(m,0x14D84u+off),load_le32(dg(m,0x255D4u)));store_le16(dg(m,0x14D86u+off),static_cast<u16>(load_le16(dg(m,0x14D86u+off))-1u));moved=b.call_2d5d0&&b.call_2d5d0(load_le32(dg(m,0x14D80u+off)),load_le32(dg(m,0x14D84u+off)),static_cast<u32>(i))!=0u;}
                    if(!moved){store_le32(dg(m,0x14D84u+off),load_le32(dg(m,0x255D4u)));store_le16(dg(m,0x14D86u+off),static_cast<u16>(load_le16(dg(m,0x14D86u+off))+1u));moved=b.call_2d5d0&&b.call_2d5d0(load_le32(dg(m,0x14D80u+off)),load_le32(dg(m,0x14D84u+off)),static_cast<u32>(i))!=0u;}
                    if(!moved){store_le32(dg(m,0x14D84u+off),old4);store_le32(dg(m,0x14D80u+off),old0);}
                }}
            }
            if(moved){const u32 roff=static_cast<u32>(i)*0x16u;*dg(m,0x14D8Bu+roff)=static_cast<u8>((*dg(m,0x14D8Bu+roff)&0xCFu)|0x10u);return 1u;}
            off+=0x16u;++i;
        }
    }
    return 0u;
}
static_assert(0x043F1u-0x04228u==457u);

void function_043f4(Machine& m,Application06Boundary& b,u32 slot,s32* second,s32* first) noexcept {
    const u32 a=b.call_50dc?b.call_50dc(0u,slot):0u;const u32 c=b.call_50dc?b.call_50dc(1u,slot):0u;const u32 base=0x218E0u+slot*0x248u;
    for(u32 row=0;row<8u;++row)for(u32 i=0;i<10u;++i){
        if(c!=0u){const u8 v=*dg(m,base+row*10u+i+0x6Au);const u8 lo=v&0x0Fu,hi=v>>4;if(lo!=0x0Fu&&hi!=lo&&hi<lo&&first)*first-=static_cast<s32>(lo-hi);}
        if(a!=0u){const u8 v=*dg(m,base+row*10u+i+0xBAu);const u8 lo=v&0x0Fu,hi=v>>4;if(lo!=0x0Fu&&hi!=lo&&hi<lo&&second)--*second;}
    }
}
static_assert(0x044CFu-0x043F4u==219u);

[[nodiscard]] u32 function_044d0(Machine& m,Application06Boundary& b,u32 slot,u32 row,u32 col,u32 kind,u32* out,u32* remaining,u32 value) noexcept {
    const u32 cell=slot*0x248u+row*10u+col+0x2199Au;
    if((*dg(m,cell)&0x0Fu)==0x0Fu||(*dg(m,cell)&0xF0u)==0u)return 0u;
    u32 changed=0u,slot_off=0u;
    for(u32 s=0;s<4u;++s,slot_off+=0x248u){
        const u16 flags=load_le16(dg(m,0x218F0u+slot_off));
        if((flags&0xC000u)!=0u)continue;
        if(load_le16(dg(m,0x218FCu+slot_off))==load_le16(dg(m,0x218FEu+slot_off))){if(remaining)--*remaining;continue;}
        while((*dg(m,cell)&0xF0u)!=0u&&
              static_cast<s16>(load_le16(dg(m,0x218FCu+slot_off)))<static_cast<s16>(load_le16(dg(m,0x218FEu+slot_off)))) {
            if(b.call_9d90)b.call_9d90(kind,s,slot);
            *dg(m,0xCF5Au+s)=1u;
            if(b.call_1f494)b.call_1f494(1u);
            const u8 v=*dg(m,cell);const u8 hi=static_cast<u8>(((v>>4)-1u)&0x0Fu);
            *dg(m,cell)=static_cast<u8>((v&0x0Fu)|(hi<<4));
            if(kind!=0x57u&&(!out||*out==0u)){if(out)*out=value+0xF0u;changed=1u;}
        }
        if(load_le16(dg(m,0x218FCu+slot_off))==load_le16(dg(m,0x218FEu+slot_off))&&remaining)--*remaining;
    }
    return changed;
}
static_assert(0x0461Au-0x044D0u==330u);
