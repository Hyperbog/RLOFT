// -----------------------------------------------------------------------------
// Batch 15: 09B4C..0A790 party-effect animation/resolution and modal apply flow.
// -----------------------------------------------------------------------------

struct Application15Boundary {
    void (*call_2e608)()=nullptr;
    void (*call_1f494)(u32)=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_28168)()=nullptr;
    void (*call_2e614)(u32,u32)=nullptr;
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;
    void (*call_3210f)(u32,u32,u32)=nullptr;
    void (*call_1c21c)(u32)=nullptr;
    u32 (*call_22e40)(u32,u32)=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_1c258)()=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    u32 (*call_19cc4)()=nullptr;
    void (*call_1c2cc)()=nullptr;
    void (*call_22d70)()=nullptr;
};

[[nodiscard]] static s32 app15_panel_base(s32 slot) noexcept {
    return (slot%2)*0x35 + (slot/2)*0xD3;
}

static void app15_draw_effect_frame(Machine& m,Application15Boundary& b,
                                    u32 desc,u8 frame,s32 panel) noexcept {
    const u32 image=load_le32(dg(m,0x258DCu))+load_le32(dg(m,desc));
    if(b.call_2975c)
        b.call_2975c(frame,load_le32(dg(m,desc+4u)),
                     static_cast<u32>(panel)+load_le32(dg(m,desc+8u)),image,0u);
}

// 09B4C..09D8E: animate one descriptor from 66ECh. mode==0 draws the chosen
// party panel only; nonzero mode redraws every active party for each frame.
void function_09b4c(Machine& m,Application15Boundary& b,u32 slot,u32 visual,u32 mode) noexcept {
    const u32 desc=0x66ECu+visual*0x21u;
    const u32 frame_count=*dg(m,desc+0x0Cu);
    const u32 frames=desc+0x0Du;
    const s32 selected_panel=app15_panel_base(static_cast<s32>(slot));

    if(mode==0u) {
        for(u32 i=0u;i<frame_count;++i) {
            if(b.call_2e608)b.call_2e608();
            *dg(m,0xCF5Au+slot)=1u;
            if(b.call_1f494)b.call_1f494(0u);
            const u8 frame=*dg(m,frames+i);
            app15_draw_effect_frame(m,b,desc,frame,selected_panel);
            if(visual==5u)app15_draw_effect_frame(m,b,desc,frame,selected_panel+0x21);
            if(b.call_1a8c8)
                b.call_1a8c8(load_le32(dg(m,0xC050u)),0x113u,
                              static_cast<u32>(selected_panel),0x7Du,0x35u);
            if(b.call_28168)b.call_28168();
            if(b.call_2e614)b.call_2e614(3u,0u);
        }
    } else {
        for(u32 i=0u;i<frame_count;++i) {
            if(b.call_2e608)b.call_2e608();
            const u8 frame=*dg(m,frames+i);
            for(u32 party=0u,off=0u;party<4u;++party,off+=0x248u) {
                if(static_cast<u32>(load_le16(dg(m,0x218F0u+off)))>=0x4000u)continue;
                const s32 panel=app15_panel_base(static_cast<s32>(party));
                *dg(m,0xCF5Au+party)=1u;
                if(b.call_1f494)b.call_1f494(0u);
                app15_draw_effect_frame(m,b,desc,frame,panel);
                if(visual==5u)app15_draw_effect_frame(m,b,desc,frame,panel+0x21);
            }
            if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0x113u,0u,0x7Du,0x140u);
            if(b.call_28168)b.call_28168();
            if(b.call_2e614)b.call_2e614(3u,0u);
        }
    }

    *dg(m,0xCF5Au+slot)=1u;
    if(b.call_1f494)b.call_1f494(0u);
    if(b.call_1a8c8)
        b.call_1a8c8(load_le32(dg(m,0xC050u)),0x113u,
                      static_cast<u32>(selected_panel),0x7Du,0x35u);
}
static_assert(0x09D8Fu-0x09B4Cu==579u);

static void app15_message_name(Machine& m,Application15Boundary& b,u32 text,u32 slot) noexcept {
    (void)m;
    if(b.call_3210f)b.call_3210f(0x759Cu,text,0x218E0u+slot*0x248u);
    if(b.call_1c21c)b.call_1c21c(0x759Cu);
}

static void app15_add_current_word(Machine& m,u32 slot,u32 amount) noexcept {
    const u32 base=slot*0x248u;
    const u16 old=load_le16(dg(m,0x218FCu+base));
    const u16 sum=static_cast<u16>(old+static_cast<u16>(amount));
    store_le16(dg(m,0x218FCu+base),sum);
    const u16 maximum=load_le16(dg(m,0x218FEu+base));
    if(static_cast<s16>(sum)>static_cast<s16>(maximum))store_le16(dg(m,0x218FCu+base),maximum);
}

// 09D90..0A253: resolve one encoded effect type for party slot and auxiliary
// slot. Return value defaults to one; the message-only failure branches return
// zero exactly where the binary clears EBP.
[[nodiscard]] u32 function_09d90(Machine& m,Application15Boundary& b,
                                  s32 type,u32 slot,u32 auxiliary) noexcept {
    const u32 base=slot*0x248u;
    const u32 aux=auxiliary*0x248u;

    switch(type) {
        case -1: {
            *dg(m,0x218F1u+base)=static_cast<u8>(*dg(m,0x218F1u+base)&0xF3u);
            const u32 amount=b.call_1c328?b.call_1c328(1u,4u,0u,8u):0u;
            app15_add_current_word(m,slot,amount);
            return 1u;
        }
        case 0x51: {
            u32 amount=b.call_1c328?b.call_1c328(1u,8u,0u,0u):0u;
            if((*dg(m,0x21A13u+aux)&0x80u)!=0u) {
                amount=8u;
                *dg(m,0x21A13u+aux)=static_cast<u8>(*dg(m,0x21A13u+aux)&0x7Fu);
            }
            app15_add_current_word(m,slot,amount);
            return 1u;
        }
        case 0x57: {
            const u32 amount=(b.call_22e40?b.call_22e40(1u,auxiliary):0u)*2u;
            app15_add_current_word(m,slot,amount);
            return 1u;
        }
        case 0x64:
            if((load_le16(dg(m,0x218F0u+base))&0x0400u)!=0u)
                *dg(m,0x218F1u+base)=static_cast<u8>(*dg(m,0x218F1u+base)&0xFBu);
            return 1u;
        case 0x68:
            if((load_le16(dg(m,0x218F0u+base))&0x1000u)!=0u)
                *dg(m,0x218F1u+base)=static_cast<u8>(*dg(m,0x218F1u+base)&0xEFu);
            return 1u;
        case 0x6E: {
            u32 amount=b.call_1c328?b.call_1c328(2u,8u,0u,1u):0u;
            if((*dg(m,0x21A13u+aux)&0x80u)!=0u) {
                amount=0x10u;
                *dg(m,0x21A13u+aux)=static_cast<u8>(*dg(m,0x21A13u+aux)&0x7Fu);
            }
            app15_add_current_word(m,slot,amount);
            return 1u;
        }
        case 0x71:
            if((load_le16(dg(m,0x218F0u+base))&0x0800u)!=0u)
                *dg(m,0x218F1u+base)=static_cast<u8>(*dg(m,0x218F1u+base)&0xF7u);
            return 1u;
        case 0x78: {
            u32 amount=b.call_1c328?b.call_1c328(3u,8u,0u,3u):0u;
            if((*dg(m,0x21A13u+aux)&0x80u)!=0u) {
                amount=0x1Bu;
                *dg(m,0x21A13u+aux)=static_cast<u8>(*dg(m,0x21A13u+aux)&0x7Fu);
            }
            app15_add_current_word(m,slot,amount);
            return 1u;
        }
        case 0x7B: {
            const u16 state=load_le16(dg(m,0x218F0u+base));
            if((state&0x4000u)==0u) {
                app15_message_name(m,b,load_le32(dg(m,0x66C4u)),slot);
                return 0u;
            }
            if((state&0x8000u)!=0u)return 1u;
            if(*dg(m,0x218F4u+base)==5u) {
                if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x66BCu)));
                return 0u;
            }
            if(load_le16(dg(m,0x21900u+base))==0u ||
               load_le16(dg(m,0x21902u+base))==0u ||
               load_le16(dg(m,0x21904u+base))==0u) {
                app15_message_name(m,b,load_le32(dg(m,0x66B8u)),slot);
                return 0u;
            }
            if(*dg(m,0x21917u+base)!=0u) {
                const s32 delta=static_cast<s32>(*dg(m,0x21917u+base))-
                                static_cast<s32>(*dg(m,0x21916u+base));
                if(delta<5) {
                    store_le16(dg(m,0x218FCu+base),1u);
                    *dg(m,0x218F1u+base)=static_cast<u8>(*dg(m,0x218F1u+base)&0xBFu);
                    *dg(m,0xCF5Au+slot)=1u;
                    *dg(m,0x21917u+base)=static_cast<u8>(*dg(m,0x21917u+base)-1u);
                    app15_message_name(m,b,load_le32(dg(m,0x66B4u)),slot);
                    return 1u;
                }
            }
            app15_message_name(m,b,load_le32(dg(m,0x66B8u)),slot);
            return 0u;
        }
        case 0x82: {
            const u16 maximum=load_le16(dg(m,0x218FEu+base));
            store_le16(dg(m,0x218F0u+base),static_cast<u16>(load_le16(dg(m,0x218F0u+base))&0x01C0u));
            *dg(m,0xCF5Au+slot)=1u;
            store_le16(dg(m,0x218FCu+base),maximum);
            return 1u;
        }
        default:
            return 1u;
    }
}
static_assert(0x0A254u-0x09D90u==1220u);

[[nodiscard]] static bool app15_target_eligible(Machine& m,u32 effect,u32 slot) noexcept {
    const u16 state=load_le16(dg(m,0x218F0u+slot*0x248u));
    switch(effect) {
        case 0x7Bu:return (state&0x8000u)==0u;
        case 0x68u:return (state&0x1000u)!=0u;
        case 0x71u:
        case 0x5Du:return (state&0x0800u)!=0u;
        case 0x64u:return (state&0x0400u)!=0u;
        default:return static_cast<u32>(state)<0x4000u;
    }
}

// 0A254..0A5BB: modal four-party effect application. The eight arguments are
// retained positionally because several values serve unrelated encoded roles.
[[nodiscard]] u32 function_0a254(Machine& m,Application15Boundary& b,
                                  u32 a0,u32 effect,s32 a2,u32 a3,s32 visual,
                                  u32 auxiliary,u32 a6,u32 a7) noexcept {
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    const u8 saved_d681=*dg(m,0xD681u);
    *dg(m,0xD681u)=0u;
    const u8 saved_d682=*dg(m,0xD682u);
    *dg(m,0xD682u)=0u;
    if(b.call_1f494)b.call_1f494(1u);
    if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x66C0u)));
    if(b.call_1c258)b.call_1c258();

    const u32 timer_delta=a3*a7;
    const u32 deadline_off=effect*4u;
    u32 result=1u;
    bool done=false;

    while(!done) {
        if(b.call_1a1f4)while(b.call_1a1f4()!=0u){}
        if(b.call_1a1f4)while(b.call_1a1f4()==0u){}
        if((load_le16(dg(m,0x9E35u))&2u)!=0u) {
            result=0u;
            break;
        }

        const s32 x=static_cast<s16>(load_le16(dg(m,0x9E31u)));
        const s32 y=static_cast<s16>(load_le16(dg(m,0x9E33u)));
        for(u32 slot=0u;slot<4u;++slot) {
            const u32 roff=slot*8u;
            const s32 left=static_cast<s16>(load_le16(dg(m,0xD684u+roff)));
            const s32 top=static_cast<s16>(load_le16(dg(m,0xD686u+roff)));
            const s32 right=static_cast<s16>(load_le16(dg(m,0xD688u+roff)));
            const s32 bottom=static_cast<s16>(load_le16(dg(m,0xD68Au+roff)));
            if(x<left||x>right||y<top||y>bottom)continue;
            if(!app15_target_eligible(m,effect,slot))break;

            if(visual>=0 && b.call_28110)
                b.call_28110(load_le32(dg(m,0x258D4u))+static_cast<u32>(visual),0xFFu,0u,1u);

            if(a2>=0)function_09b4c(m,b,slot,static_cast<u32>(visual),0u);

            if(a2>=0 && a6==0u) {
                const u32 base=slot*0x248u;
                store_le32(dg(m,0x21A12u+base),load_le32(dg(m,0x21A12u+base))|a0);
                if(a3!=0u) {
                    const u32 now=b.call_19cc4?b.call_19cc4():0u;
                    store_le32(dg(m,0x21A16u+base+deadline_off),now+timer_delta);
                }
                if(a0==0x40u) {
                    const u32 r=b.call_22e40?b.call_22e40(4u,auxiliary):0u;
                    const s32 sr=static_cast<s32>(r);
                    const u32 half=static_cast<u32>(sr/2);
                    const u32 add=b.call_1c328?b.call_1c328(1u,4u,0u,half):0u;
                    store_le32(dg(m,0x21AD2u+base),load_le32(dg(m,0x21AD2u+base))+add);
                }
                if(a0==0x800u) {
                    const u32 add=b.call_1c328?b.call_1c328(1u,8u,0u,0u):0u;
                    store_le32(dg(m,0x21ACAu+base),load_le32(dg(m,0x21ACAu+base))+add);
                }
            } else {
                result=function_09d90(m,b,static_cast<s32>(effect),slot,auxiliary);
            }

            if(effect==0u) {
                const u32 base=slot*0x248u;
                store_le32(dg(m,0x21A16u+base),0u);
                const u32 r=b.call_22e40?b.call_22e40(4u,slot):0u;
                store_le32(dg(m,0x219F7u+base),r+8u);
            }
            done=true;
            break;
        }
    }

    if(b.call_1c2cc)b.call_1c2cc();
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    *dg(m,0xD681u)=saved_d681;
    *dg(m,0xD682u)=saved_d682;
    if(saved_d681!=0u)*dg(m,0xD681u)=2u;
    if(saved_d682!=0u)*dg(m,0xD682u)=2u;
    if(b.call_1f494)b.call_1f494(1u);
    if(b.call_22d70)b.call_22d70();
    return result;
}
static_assert(0x0A5BCu-0x0A254u==872u);

// 0A5BC..0A608: save and suppress the two input-state bytes while marking all
// four party panels dirty.
void function_0a5bc(Machine& m,Application15Boundary& b) noexcept {
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    store_le32(dg(m,0x12BA0u),static_cast<u32>(*dg(m,0xD681u)));
    *dg(m,0xD681u)=0u;
    store_le32(dg(m,0x12BA4u),static_cast<u32>(*dg(m,0xD682u)));
    *dg(m,0xD682u)=0u;
    if(b.call_1f494)b.call_1f494(1u);
}
static_assert(0x0A609u-0x0A5BCu==77u);

// 0A60C..0A666: restore the saved input-state bytes, normalize nonzero states
// to two, and mark all four party panels dirty.
void function_0a60c(Machine& m,Application15Boundary& b) noexcept {
    *dg(m,0xD681u)=*dg(m,0x12BA0u);
    *dg(m,0xD682u)=*dg(m,0x12BA4u);
    if(*dg(m,0xD681u)!=0u)*dg(m,0xD681u)=2u;
    if(*dg(m,0xD682u)!=0u)*dg(m,0xD682u)=2u;
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    if(b.call_1f494)b.call_1f494(1u);
}
static_assert(0x0A667u-0x0A60Cu==91u);

// 0A668..0A790: play the encoded sound, animate visual 2 across all party
// panels, then install the 21A6A timer/21A14 bit on each non-8000h party slot.
void function_0a668(Machine& m,Application15Boundary& b,u32 value) noexcept {
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x3Eu,0xFFu,0u,1u);
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    const u8 saved_d681=*dg(m,0xD681u);
    *dg(m,0xD681u)=0u;
    const u8 saved_d682=*dg(m,0xD682u);
    *dg(m,0xD682u)=0u;
    store_le32(dg(m,0x12BA0u),saved_d681);
    store_le32(dg(m,0x12BA4u),saved_d682);
    if(b.call_1f494)b.call_1f494(1u);

    function_09b4c(m,b,value,2u,1u);

    *dg(m,0xD681u)=*dg(m,0x12BA0u);
    *dg(m,0xD682u)=*dg(m,0x12BA4u);
    if(*dg(m,0xD681u)!=0u)*dg(m,0xD681u)=2u;
    if(*dg(m,0xD682u)!=0u)*dg(m,0xD682u)=2u;
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    if(b.call_1f494)b.call_1f494(1u);

    for(u32 slot=0u,base=0u;slot<4u;++slot,base+=0x248u) {
        if((load_le16(dg(m,0x218F0u+base))&0x8000u)!=0u)continue;
        *dg(m,0xCF5Au+slot)=1u;
        *dg(m,0x21A14u+base)=static_cast<u8>(*dg(m,0x21A14u+base)|0x20u);
        const u32 factor=b.call_22e40?b.call_22e40(3u,value):0u;
        const u32 now=b.call_19cc4?b.call_19cc4():0u;
        store_le32(dg(m,0x21A6Au+base),now+factor*0x438u);
    }
}
static_assert(0x0A791u-0x0A668u==297u);
