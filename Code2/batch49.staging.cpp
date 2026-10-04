// -----------------------------------------------------------------------------
// Batch 49: 223F4..22E3D reverse interaction scanning, geometry dispatch,
// two CS-owned jump tables, click/party routing, and the 20-entry queued-input
// ring. Compiler separators and the two table objects are ledgered separately.
// The next routine begins at 22E40.
// -----------------------------------------------------------------------------

struct Application49Boundary {
    Application48Boundary* app48=nullptr;
    u32 (*call_2ac80)(u32)=nullptr;
    u32 (*call_2e6f0)(u32)=nullptr;
    u32 (*call_243e0)(u32,u32)=nullptr;
    void (*call_c704)()=nullptr;
    u32 (*call_1e8c8)(u32,u32)=nullptr;
    void (*call_33808)(s32,s32,s32,u32&,u32&,u32&,u32&,u32&)=nullptr;
    void (*call_23d84)(s32,u32,s32,s32)=nullptr;
    void (*call_1ac0c)()=nullptr;
    void (*call_1ac38)(s32,s32)=nullptr;
    void (*call_233a8)(s32)=nullptr;
    void (*call_12e70)(s32,s32,u32)=nullptr;
    void (*call_02db0)(s32,s32,u32)=nullptr;
    void (*divide_error)()=nullptr;
};

static u32 app49_xor_low_byte_1(u32 v) noexcept {
    return (v&0xFFFFFF00u)|((v^1u)&0xFFu);
}
static void app49_copy_z_pairs(Machine& m,u32 dst,u32 src) noexcept {
    for(;;) {
        const u8 a=*dg(m,src);
        *dg(m,dst)=a;
        if(a==0u)return;
        const u8 c=*dg(m,src+1u);
        *dg(m,dst+1u)=c;
        src+=2u; dst+=2u;
        if(c==0u)return;
    }
}
static void app49_append_z_pairs(Machine& m,u32 dst,u32 src) noexcept {
    while(*dg(m,dst)!=0u)++dst;
    app49_copy_z_pairs(m,dst,src);
}

u32 function_223f4(Machine& m,Application49Boundary& b,s32 x,s32 y,u32 flags) noexcept {
    if(b.app48==nullptr)return 0u;
    Application48Boundary& a48=*b.app48;
    const u32 other_page=app49_xor_low_byte_1(load_le32(dg(m,0xC050u)));
    s32 ordinal=static_cast<s32>(load_le16(dg(m,0xF038u+other_page*2u)))-1;
    for(;ordinal>=0;--ordinal) {
        const u32 page=app49_xor_low_byte_1(load_le32(dg(m,0xC050u)));
        u32 index=0u;
        u32 rel=0u;
        const u32 count=load_le16(dg(m,0xF038u+page*2u));
        while(static_cast<s32>(index)<static_cast<s32>(count)) {
            const u32 base=page*0x2260u;
            if(load_le16(dg(m,0x27152u+base+rel))==static_cast<u16>(ordinal))break;
            rel+=0x16u;
            ++index;
        }
        const u32 base=page*0x2260u+0x27148u;
        const u32 rec=base+index*0x16u;
        const u32 kind=load_le16(dg(m,rec+8u));
        if(kind==3u)continue;
        if(kind==1u) {
            const u32 object_index=load_le16(dg(m,rec+6u));
            const u32 object_type=load_le16(dg(m,0x22246u+object_index*0x16u));
            if((b.call_2ac80?b.call_2ac80(object_type):0u)==0u)continue;
        }
        if(function_21bf0(m,a48,rec,0x1E)==0u)continue;
        if(static_cast<s32>(load_le16(dg(m,rec+0x0Eu)))>x)continue;
        if(static_cast<s32>(load_le16(dg(m,rec+0x10u)))<x)continue;
        if(static_cast<s32>(load_le16(dg(m,rec+0x12u)))>y)continue;
        if(static_cast<s32>(load_le16(dg(m,rec+0x14u)))<y)continue;

        const u32 object_index=load_le16(dg(m,rec+6u));
        if(kind==2u&&(flags&1u)!=0u) {
            const u32 item=load_le16(dg(m,0x17626u+object_index*0x16u));
            function_21704(m,a48,object_index);
            const u32 text=0x759Cu;
            app49_copy_z_pairs(m,text,load_le32(dg(m,0xD67Cu)));
            const u32 env0=load_le32(dg(m,0x255D8u));
            app49_append_z_pairs(m,text,env0+item*65u);
            if(*dg(m,0x8602u)!=0u) {
                const u32 env1=load_le32(dg(m,0x255D8u));
                const u8 suffix=*dg(m,env1+item*65u+0x37u);
                if(suffix!=0u) {
                    app49_append_z_pairs(m,text,0x11C4u);
                    const u32 number=b.call_2e6f0?b.call_2e6f0(static_cast<u32>(suffix)):0u;
                    if(number!=0u)app49_append_z_pairs(m,text,number);
                }
            }
            if(a48.app46&&a48.app46->call_1c21c)a48.app46->call_1c21c(text);

            bool complete=false;
            if(item==0x87u) {
                if((a48.call_f4b0?a48.call_f4b0(0x88u):0u)!=0u&&
                   (a48.call_f4b0?a48.call_f4b0(0x74u):0u)!=0u&&
                   (a48.call_f4b0?a48.call_f4b0(0xE9u):0u)!=0u)complete=true;
            } else if(item==0x88u) {
                if((a48.call_f4b0?a48.call_f4b0(0x87u):0u)!=0u&&
                   (a48.call_f4b0?a48.call_f4b0(0x74u):0u)!=0u&&
                   (a48.call_f4b0?a48.call_f4b0(0xE9u):0u)!=0u)complete=true;
            } else if(item==0x74u) {
                if((a48.call_f4b0?a48.call_f4b0(0x87u):0u)!=0u&&
                   (a48.call_f4b0?a48.call_f4b0(0x88u):0u)!=0u&&
                   (a48.call_f4b0?a48.call_f4b0(0xE9u):0u)!=0u)complete=true;
            } else if(item==0xE9u) {
                if((a48.call_f4b0?a48.call_f4b0(0x87u):0u)!=0u&&
                   (a48.call_f4b0?a48.call_f4b0(0x88u):0u)!=0u&&
                   (a48.call_f4b0?a48.call_f4b0(0x74u):0u)!=0u)complete=true;
            }
            if(complete)function_21f70(m,a48);
            if(a48.early&&(a48.early->call_1c1dc?a48.early->call_1c1dc(0x8Fu):0u)==0u&&
               item>=0x61u&&item<=0x62u&&
               (a48.call_f4b0?a48.call_f4b0(0x35u):0u)!=0u)
                store_le32(dg(m,0xBF44u),0x0Fu);
            return 1u;
        }

        if(kind==0u) {
            const u32 object_type=load_le16(dg(m,0x14D8Eu+object_index*0x16u));
            if((a48.call_2ad18?a48.call_2ad18(object_type):0u)!=0u&&(flags&1u)!=0u)
                return function_21adc(m,a48,object_index,0u);
            if((a48.call_2ade0?a48.call_2ade0(object_type):0u)!=0u&&(flags&1u)!=0u)
                return function_21b44(m,a48,object_index,0u);
            if(a48.app46==nullptr||(a48.app46->call_2acdc?a48.app46->call_2acdc(object_type):0u)==0u)continue;
            if((*dg(m,0x14D8Bu+object_index*0x16u)&2u)!=0u)continue;
            const s32 phase=app48_sar(static_cast<s32>(load_le16(dg(m,rec+2u)))-0x7D00,3u);
            if(phase<0x39)continue;
            if((flags&1u)!=0u) {
                const u32 pair_byte=static_cast<u32>(*dg(m,0xCF68u));
                const u32 actor=pair_byte/2u;
                const u32 slot=pair_byte%2u;
                if(a48.app47&&(a48.app47->call_24478?a48.app47->call_24478(actor,slot):0u)!=0u&&
                   (b.call_243e0?b.call_243e0(actor,slot):0u)!=0u&&a48.app47->call_26018)
                    a48.app47->call_26018(actor,slot,object_index);
            }
            if(b.call_c704)b.call_c704();
            return 1u;
        }

        if(kind==1u) {
            const u32 object_type=load_le16(dg(m,0x22246u+object_index*0x16u));
            function_21f1c(m,a48,object_type);
            return 1u;
        }
    }
    return 0u;
}
static_assert(0x22891u-0x223F4u==1181u);

void function_22894(Machine& m,Application49Boundary& b,s32 scale,s32 coordinate,s32 descriptor_index,
                     u32& out_value,u32& out_shifted,u32& out_bucket,u32& out_remainder,u32& out_quotient) noexcept {
    const u32 desc=0x15A18u+static_cast<u32>(descriptor_index)*0x1Cu;
    out_value=load_le32(dg(m,desc+8u));
    out_shifted=static_cast<u32>(app48_sar(static_cast<s32>(load_le32(dg(m,desc+0x10u))),6u));
    const s32 numerator=static_cast<s32>(load_le32(dg(m,desc+0x14u)));
    const s32 divisor=static_cast<s16>(load_le16(dg(m,0x25619u)));
    if(divisor==0||(numerator==(-2147483647-1)&&divisor==-1)) {
        if(b.divide_error)b.divide_error();
        return;
    }
    out_remainder=static_cast<u32>(numerator%divisor);
    out_quotient=static_cast<u32>(numerator/divisor);

    const u32 scaled_bits=static_cast<u32>(scale)*4u;
    const s32 scaled=static_cast<s32>(scaled_bits);
    u32 base_bits=0x84u-static_cast<u32>(scaled/2);
    base_bits+=static_cast<u32>(scaled/8);
    const s32 base=static_cast<s32>(base_bits);
    u32 accumulator=0x40u;
    for(u32 i=0u;i<0x80u;++i) {
        const s32 step=static_cast<s32>(accumulator)/0x80;
        const s32 threshold=static_cast<s32>(static_cast<u32>(base)+static_cast<u32>(step));
        if(coordinate<threshold) {
            out_bucket=i;
            return;
        }
        accumulator+=scaled_bits;
    }
}
static_assert(0x2294Du-0x22894u==185u);

static constexpr u32 kFunction229ccRegionDispatch[21]={
    0x22A33u,0x22C02u,0x22C28u,0x22C48u,0x22C6Eu,
    0x22C02u,0x22C28u,0x22C48u,0x22C6Eu,
    0x22C02u,0x22C28u,0x22C48u,0x22C6Eu,
    0x22C02u,0x22C28u,0x22C48u,0x22C6Eu,
    0x22C94u,0x22CB4u,0x22CD4u,0x22B95u
};
static constexpr u32 kFunction229ccTypeDispatch[10]={
    0x22B3Cu,0x22BA6u,0x22BF5u,0x22BC1u,0x22BF5u,
    0x22BF5u,0x22BF5u,0x22BF5u,0x22BF5u,0x22BE3u
};

void function_229cc(Machine& m,Application49Boundary& b,s32 x,s32 y,u32 flags) noexcept {
    if(b.app48==nullptr)return;
    Application48Boundary& a48=*b.app48;
    u32 region=0u;
    u32 roff=0u;
    for(;;) {
        const s32 left=static_cast<s16>(load_le16(dg(m,0xD684u+roff)));
        if(left==-1)break;
        const s32 top=static_cast<s16>(load_le16(dg(m,0xD686u+roff)));
        const s32 right=static_cast<s16>(load_le16(dg(m,0xD688u+roff)));
        const s32 bottom=static_cast<s16>(load_le16(dg(m,0xD68Au+roff)));
        if(x>=left&&x<=right&&y>=top&&y<=bottom)break;
        roff+=8u;
        ++region;
    }
    if(region>0x14u)return;

    if(region==0u) {
        bool default_action=true;
        bool consume_held=false;
        s32 y_work=y;
        if((flags&2u)!=0u) {
            if(b.call_c704)b.call_c704();
        } else {
            const s32 held=static_cast<s16>(load_le16(dg(m,0x8600u)));
            if(held!=-1&&x>0xBC) {
                function_2195c(m,a48,static_cast<u32>(y_work),static_cast<u32>(held),
                               static_cast<u32>(*dg(m,0x8602u)),load_le32(dg(m,0x255D0u)),
                               load_le32(dg(m,0x255D4u)),1u,1u);
            } else {
                y_work=static_cast<s32>(static_cast<u32>(y_work)-0x20u);
                if(function_223f4(m,b,x,y_work,flags)!=0u) {
                    default_action=false;
                } else {
                    const u32 desc=0x15A18u+static_cast<u32>(y_work)*0x1Cu;
                    const s32 scaled=static_cast<s32>(load_le32(dg(m,desc)))/8;
                    if(scaled<0x2D)consume_held=true;
                    else {
                        u32 out_q=0u,out_r=0u,out_shift=0u,out_bucket=0u,out_value=0u;
                        if(scaled>=static_cast<s32>(load_le32(dg(m,0xC738u))))
                            function_22894(m,b,scaled,x,y_work,out_value,out_shift,out_bucket,out_r,out_q);
                        else if(b.call_33808)
                            b.call_33808(scaled,x,y_work,out_value,out_shift,out_bucket,out_r,out_q);
                        const u32 record=b.call_1e8c8?b.call_1e8c8(out_r,out_q):0u;
                        if(record==0u||static_cast<s32>(out_shift)<0)consume_held=true;
                        else {
                            const u8 selector=static_cast<u8>(*dg(m,record)-1u);
                            if(selector>9u)consume_held=true;
                            else switch(selector) {
                                case 0u:
                                    if(a48.app46&&function_20c0c(m,*a48.app46,out_r,out_q,record)!=0u)
                                        default_action=false;
                                    break;
                                case 1u:
                                    if(a48.app47&&function_210b0(m,*a48.app47,static_cast<s32>(out_bucket),
                                                                 static_cast<s32>(out_shift),out_value,record)!=0u)
                                        default_action=false;
                                    break;
                                case 3u:
                                    if(a48.app47&&function_20e08(m,*a48.app47,static_cast<s32>(out_bucket),
                                                                 static_cast<s32>(out_shift),out_value,record)!=0u)
                                        default_action=false;
                                    break;
                                case 9u:
                                    function_21db4(m,a48,static_cast<u32>(*dg(m,record+9u)));
                                    break;
                                default:
                                    consume_held=true;
                                    break;
                            }
                        }
                    }
                }
            }
        }
        if(consume_held) {
            const s32 held=static_cast<s16>(load_le16(dg(m,0x8600u)));
            if(held!=-1) {
                if(b.call_23d84)b.call_23d84(held,static_cast<u32>(*dg(m,0x8602u)),y_work,-1);
                if(a48.app47&&a48.app47->call_1a344)a48.app47->call_1a344(-1,0u);
                default_action=false;
            }
        }
        if(default_action) {
            if(b.call_1ac0c)b.call_1ac0c();
            if(b.call_1ac38)b.call_1ac38(x,y_work);
        }
        return;
    }

    if(region==20u) {
        if(b.call_1ac38)b.call_1ac38(x,y);
        return;
    }
    if(region==17u) {
        if(*dg(m,0xD681u)!=0u&&a48.app47&&a48.app47->call_23438)a48.app47->call_23438(0,x,y);
        return;
    }
    if(region==18u) {
        if(*dg(m,0xD682u)!=0u&&a48.app47&&a48.app47->call_23438)a48.app47->call_23438(1,x,y);
        return;
    }
    if(region==19u) {
        if(a48.app46)function_20994(m,*a48.app46);
        return;
    }

    const u32 lane=(region-1u)&3u;
    const s32 actor=static_cast<s32>((region-1u)/4u);
    if(lane==0u) {
        if(a48.app47)function_214b8(m,*a48.app47,actor,0u,flags);
        return;
    }
    if(lane==1u) {
        if(b.call_233a8)b.call_233a8(static_cast<s32>((region-2u)/4u));
        return;
    }
    if(lane==2u) {
        if(a48.app47)function_214b8(m,*a48.app47,static_cast<s32>((region-3u)/4u),1u,flags);
        return;
    }
    if(a48.app47)function_211d8(m,*a48.app47,static_cast<s32>((region-4u)/4u),
                                static_cast<u32>(x),static_cast<u32>(y),flags);
}
static_assert(0x22CE0u-0x229CCu==788u);

void function_22ce0(Machine& m,u32 first,u32 second,u32 third) noexcept {
    const u8 mode=*dg(m,0xD680u);
    if(mode!=0u&&mode!=1u&&mode!=4u)return;
    const u32 tail=static_cast<u32>(*dg(m,0xA63Bu));
    const u32 next=(tail+1u)%20u;
    if(next==static_cast<u32>(*dg(m,0xA63Au)))return;
    const u32 p=0xA63Cu+tail*6u;
    store_le16(dg(m,p),static_cast<u16>(first));
    store_le16(dg(m,p+2u),static_cast<u16>(second));
    store_le16(dg(m,p+4u),static_cast<u16>(third));
    *dg(m,0xA63Bu)=static_cast<u8>(next);
}
static_assert(0x22D70u-0x22CE0u==144u);

void function_22d70(Machine& m) noexcept {
    *dg(m,0xCF4Au)=0u;
    store_le16(dg(m,0xA6BAu),0u);
    *dg(m,0xA63Au)=*dg(m,0xA63Bu);
}
static_assert(0x22D8Bu-0x22D70u==27u);

void function_22d8c(Machine& m,Application49Boundary& b) noexcept {
    for(;;) {
        if(b.app48&&b.app48->app46&&b.app48->app46->app45&&b.app48->app46->app45->call_2f868)
            b.app48->app46->app45->call_2f868();
        const u8 head=*dg(m,0xA63Au);
        if(head==*dg(m,0xA63Bu))return;
        const u32 p=0xA63Cu+static_cast<u32>(head)*6u;
        const s32 x=static_cast<s16>(load_le16(dg(m,p)));
        const s32 y=static_cast<s16>(load_le16(dg(m,p+2u)));
        const u32 flags=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,p+4u)))));
        *dg(m,0xA63Au)=static_cast<u8>((static_cast<u32>(head)+1u)%20u);
        store_le16(dg(m,0x9E21u),0xFFFFu);
        store_le16(dg(m,0x9E23u),0xFFFFu);
        store_le16(dg(m,0x9E25u),0xFFFFu);
        store_le16(dg(m,0x9E27u),0xFFFFu);
        const u8 mode=*dg(m,0xD680u);
        if(mode==0u)function_229cc(m,b,x,y,flags);
        else if(mode==1u) { if(b.call_12e70)b.call_12e70(x,y,flags); }
        else if(mode==4u) { if(b.call_02db0)b.call_02db0(x,y,flags); }
    }
}
static_assert(0x22E3Eu-0x22D8Cu==178u);

static_assert((0x22891u-0x223F4u)+(0x2294Du-0x22894u)+
              (0x22CE0u-0x229CCu)+(0x22D70u-0x22CE0u)+
              (0x22D8Bu-0x22D70u)+(0x22E3Eu-0x22D8Cu)==2503u);
