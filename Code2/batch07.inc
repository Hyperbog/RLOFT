// -----------------------------------------------------------------------------
// Batch 07: 0461C..0518B four-party simulation/update loop and classifier.
// -----------------------------------------------------------------------------

struct Application07Boundary {
    void (*call_1c21c)(u32)=nullptr;
    void (*call_1c258)()=nullptr;
    u32 (*call_7d44)(u32)=nullptr;
    u32 (*call_23eac)(u32,u32)=nullptr;
    void (*call_cd44)(u32,u32)=nullptr;
    void (*call_31f94)()=nullptr;
    void (*call_893c)()=nullptr;
    u32 (*call_30ca2)()=nullptr;
    void (*call_3210f3)(u32,u32,u32)=nullptr;
    void (*call_3210f4)(u32,u32,u32,u32)=nullptr;
    void (*call_1c368)(u32,u32)=nullptr;
    void (*call_30aec)()=nullptr;
    void (*call_231c0)(u32,u32)=nullptr;
    void (*call_1e74c)(u32)=nullptr;
};

[[nodiscard]] u32 function_050dc(Machine& m,u32 mode,u32 slot) noexcept;

static u32 app07_string_end(Machine& m,u32 p) noexcept {
    while(*dg(m,p)!=0u)++p;
    return p;
}
static void app07_emit(Application07Boundary& b) noexcept {
    if(b.call_1c21c)b.call_1c21c(0x759Cu);
    if(b.call_1c258)b.call_1c258();
}
static void app07_format_record_line(Machine& m,Application07Boundary& b,u32 rec,u32 suffix) noexcept {
    if(b.call_3210f3)b.call_3210f3(0x759Cu,load_le32(dg(m,0x6098u)),rec);
    if(b.call_1c368)b.call_1c368(app07_string_end(m,0x759Cu),suffix);
    app07_emit(b);
}

void function_0461c(Machine& m,Application07Boundary& b,Application06Boundary& a6,
                    Application03Boundary& a3,Application04Boundary& a4) noexcept {
    if(function_04100(m,a6,a3,a4)!=0u) {
        u32 src=load_le32(dg(m,0x60ACu)),dst=0x759Cu;
        for(;;) {
            const u8 a=*dg(m,src);*dg(m,dst)=a;
            if(a==0u)break;
            const u8 c=*dg(m,src+1u);*dg(m,dst+1u)=c;
            src+=2u;dst+=2u;
            if(c==0u)break;
        }
        app07_emit(b);
        return;
    }

    u32 deadlines[4]={0u,0u,0u,0u};
    u32 pending=0u;
    s32 deficit=0;
    s32 queued=0;
    u32 special=0u;
    u32 force_exit=0u;
    u32 tick=0u;
    bool cleanup_only=false;

    for(u32 p=0;p<4u;++p) {
        const u32 rec=0x218E0u+p*0x248u;
        if((load_le16(dg(m,rec+0x10u))&0x1000u)!=0u) {
            *dg(m,0xCF5Au+p)=1u;
            *dg(m,rec+0x11u)=static_cast<u8>(*dg(m,rec+0x11u)&0xEFu);
        }
    }

    for(u32 p=0;p<4u;++p) {
        const u32 rec=0x218E0u+p*0x248u;
        const u16 flags=load_le16(dg(m,rec+0x10u));
        const u16 cur=load_le16(dg(m,rec+0x1Cu));
        const u16 target=load_le16(dg(m,rec+0x1Eu));
        if((flags&0xC000u)==0u&&cur!=target)++pending;
        if(cur==target&&(flags&0x0C00u)!=0u)++pending;
        if((flags&0xC000u)!=0u)continue;
        for(u32 row=0;row<8u;++row) {
            const u32 rb=rec+row*10u;
            for(u32 col=0;col<10u;++col) {
                if((*dg(m,rec+0x131u)&2u)!=0u) {
                    const u32 cell=rb+col+0x6Au;const u8 raw=*dg(m,cell);
                    const u8 lo=raw&0x0Fu,hi=raw>>4;
                    if(lo!=0x0Fu&&hi!=lo) {
                        if(hi>lo)*dg(m,cell)=static_cast<u8>(lo|(lo<<4));
                        else deficit+=static_cast<s32>(lo-hi);
                    }
                }
                if((*dg(m,rec+0x131u)&5u)!=0u) {
                    const u32 cell=rb+col+0xBAu;const u8 raw=*dg(m,cell);
                    const u8 lo=raw&0x0Fu,hi=raw>>4;
                    if(lo!=0x0Fu&&hi!=lo) {
                        if(hi>lo)*dg(m,cell)=static_cast<u8>(lo|(lo<<4));
                        else deadlines[p]=0x2D0u;
                    }
                }
            }
        }
        if(deadlines[p]!=0u)++queued;
    }
    for(u32 p=0;p<4u;++p)*dg(m,0xCF5Au+p)=1u;
    const u32 had_work=static_cast<u32>(queued)|static_cast<u32>(deficit);

    if(pending!=0u) {
        for(u32 p=0;p<4u;++p) {
            const u32 rec=0x218E0u+p*0x248u;
            if((load_le16(dg(m,rec+0x10u))&0xC000u)!=0u)continue;
            const s32 cls=static_cast<s32>(function_050dc(m,0u,p));
            bool trigger=false;
            if(cls>=0) {
                const u8 a=*dg(m,rec+0xBBu)&0x0Fu;
                const u8 c=*dg(m,rec+0xD8u)&0x0Fu;
                const u8 d=*dg(m,rec+0xE2u)&0x0Fu;
                const u8 e=*dg(m,rec+0xECu)&0x0Fu;
                trigger=a!=0u&&(a!=0x0Fu||c!=0x0Fu||d!=0x0Fu||e!=0x0Fu);
            } else if(b.call_23eac)trigger=b.call_23eac(p,1u)!=0u;
            if(trigger) {
                special=b.call_7d44?b.call_7d44(load_le32(dg(m,0x60D0u))):0u;
                break;
            }
        }
    }

    if(pending!=0u||deficit!=0||queued!=0) {
        for(;;) {
            for(u32 step=0;step<10u;++step) {
                ++tick;
                store_le32(dg(m,0xA7E4u),load_le32(dg(m,0xA7E4u))+0x438u);
                store_le32(dg(m,0xCF4Cu),load_le32(dg(m,0xCF4Cu))+0x445u);
                if(b.call_31f94)b.call_31f94();
                if(load_le16(dg(m,0xCF4Eu))==0u) {
                    for(u32 p=0;p<4u;++p) {
                        const u32 rec=0x218E0u+p*0x248u;
                        if((load_le16(dg(m,rec+0x10u))&0xC000u)!=0u)continue;
                        if(static_cast<s8>(*dg(m,rec+0x15u))==1)*dg(m,rec+0xC1u)=0x11u;
                        if(static_cast<s32>(function_050dc(m,0u,p))>=0)*dg(m,rec+0xC2u)=0x11u;
                    }
                }
            }
            if(static_cast<s32>(tick)>=0x1E0)force_exit=1u;
            if(b.call_893c)b.call_893c();
            if(*dg(m,0x258A8u)!=0xFFu) {
                const u32 r=b.call_30ca2?b.call_30ca2():0u;
                if(((r*100u)>>15)>0x5Fu&&function_04228(m,a6)!=0u){cleanup_only=true;break;}
            }

            bool early=false;
            for(u32 p=0;p<4u&&!early;++p) {
                const u32 rec=0x218E0u+p*0x248u;
                const u16 flags=load_le16(dg(m,rec+0x10u));
                if(static_cast<u32>(flags)>=0x4000u)continue;
                if((flags&0x0800u)!=0u) {
                    if(static_cast<s16>(load_le16(dg(m,rec+0x1Cu)))==1) {
                        --pending;function_043f4(m,a6,p,&queued,&deficit);
                    }
                    if(b.call_cd44)b.call_cd44(p,1u);
                    *dg(m,0xCF5Au+p)=1u;
                    if(p==0u) {
                        force_exit=1u;
                        if((b.call_7d44?b.call_7d44(load_le32(dg(m,0x60D4u))):0u)==0u)early=true;
                    }
                }
                if(!early&&(flags&0x0400u)!=0u) {
                    if(static_cast<s16>(load_le16(dg(m,rec+0x1Cu)))<=3) {
                        --pending;function_043f4(m,a6,p,&queued,&deficit);
                    }
                    if(b.call_cd44)b.call_cd44(p,3u);
                    *dg(m,0xCF5Au+p)=1u;
                    if(p==0u) {
                        force_exit=1u;
                        if((b.call_7d44?b.call_7d44(load_le32(dg(m,0x60D8u))):0u)==0u)early=true;
                    }
                }
            }
            if(early){cleanup_only=true;break;}

            if(special!=0u) {
                for(u32 p=0;p<4u;++p) {
                    const u32 rec=0x218E0u+p*0x248u;
                    if((load_le16(dg(m,rec+0x10u))&0xC000u)!=0u)continue;
                    if(static_cast<s32>(function_050dc(m,0u,p))>=0) {
                        queued+=static_cast<s32>(function_044d0(m,a6,p,0u,1u,0x51u,&deadlines[p],&pending,tick));
                        queued+=static_cast<s32>(function_044d0(m,a6,p,3u,0u,0x6Eu,&deadlines[p],&pending,tick));
                        queued+=static_cast<s32>(function_044d0(m,a6,p,4u,0u,0x78u,&deadlines[p],&pending,tick));
                    }
                    if(b.call_23eac&&b.call_23eac(p,1u)!=0u)
                        (void)function_044d0(m,a6,p,0u,7u,0x57u,nullptr,&pending,tick);
                }
            }

            if(static_cast<s32>(tick)%60==0) {
                for(u32 p=0;p<4u;++p) {
                    const u32 rec=0x218E0u+p*0x248u;
                    if((load_le16(dg(m,rec+0x10u))&0x8000u)==0u&&static_cast<s8>(*dg(m,rec+0x15u))==1)
                        *dg(m,rec+0xC1u)=0x11u;
                }
            }
            if(pending!=0u&&static_cast<s32>(tick)%0x5A0==0) {
                for(u32 p=0;p<4u;++p) {
                    const u32 rec=0x218E0u+p*0x248u;
                    if((load_le16(dg(m,rec+0x10u))&0xC000u)!=0u)continue;
                    s16 cur=static_cast<s16>(load_le16(dg(m,rec+0x1Cu)));
                    const s16 target=static_cast<s16>(load_le16(dg(m,rec+0x1Eu)));
                    if(cur==target)continue;
                    cur=static_cast<s16>(cur+3);store_le16(dg(m,rec+0x1Cu),static_cast<u16>(cur));
                    if(cur>=target){store_le16(dg(m,rec+0x1Cu),static_cast<u16>(target));--pending;}
                    *dg(m,0xCF5Au+p)=1u;
                }
            }
            if(a6.call_1f494)a6.call_1f494(1u);

            if(force_exit!=0u&&queued!=0) {
                for(u32 p=0;p<4u;++p) {
                    const u32 rec=0x218E0u+p*0x248u;
                    if((load_le16(dg(m,rec+0x10u))&0xC000u)!=0u||(*dg(m,rec+0x131u)&5u)==0u||
                       deadlines[p]==0u||static_cast<s32>(tick)<=static_cast<s32>(deadlines[p]))continue;
                    --queued;deadlines[p]=0u;
                    for(u32 row=0;row<8u;++row)for(u32 col=0;col<10u;++col) {
                        const u32 cell=rec+row*10u+col+0xBAu;const u8 raw=*dg(m,cell);
                        const u8 lo=raw&0x0Fu,hi=raw>>4;
                        if(lo==0x0Fu||hi==lo)continue;
                        *dg(m,cell)=static_cast<u8>(lo|(lo<<4));
                        app07_format_record_line(m,b,rec,0x92u+row*10u+col);
                    }
                }
            }
            if(force_exit!=0u&&deficit!=0) {
                for(u32 p=0;p<4u;++p) {
                    const u32 rec=0x218E0u+p*0x248u;
                    if((load_le16(dg(m,rec+0x10u))&0xC000u)!=0u||(*dg(m,rec+0x131u)&2u)==0u)continue;
                    const s32 q=(static_cast<s32>(tick)-static_cast<s32>(deadlines[p]))/10;
                    if(q<=0)continue;
                    bool changed=false;
                    for(u32 rowoff=0u;rowoff<0x50u&&static_cast<s32>(rowoff)<q*10&&!changed;rowoff+=10u) {
                        for(u32 col=0;col<10u;++col) {
                            const u32 cell=rec+rowoff+col+0x6Au;const u8 raw=*dg(m,cell);
                            const u8 lo=raw&0x0Fu,hi=raw>>4;
                            if(lo==0x0Fu||hi==lo)continue;
                            const u8 nhi=static_cast<u8>((hi+1u)&0x0Fu);
                            *dg(m,cell)=static_cast<u8>(lo|(nhi<<4));
                            deadlines[p]=tick;
                            app07_format_record_line(m,b,rec,0x42u+rowoff+col);
                            --deficit;changed=true;break;
                        }
                    }
                }
            }

            const u32 key=a6.call_30b40?a6.call_30b40():0x1Bu;
            if(key==0x1Bu)break;
        }
    }

    if(!cleanup_only) {
        if(a6.call_30b40&&a6.call_30b40()!=0u&&b.call_30aec)b.call_30aec();
        if(had_work!=0u) {
            if(*dg(m,0xD681u)!=0u) {
                const u32 p=load_le32(dg(m,0xD8A8u));*dg(m,0xD681u)=2u;
                if(b.call_231c0)b.call_231c0(p,*dg(m,0x21A0Fu+p*0x248u));
            }
            if(*dg(m,0xD682u)!=0u) {
                const u32 p=load_le32(dg(m,0xD8ACu));*dg(m,0xD682u)=2u;
                if(b.call_231c0)b.call_231c0(p,*dg(m,0x21A0Fu+p*0x248u));
            }
        }
        const s32 q=static_cast<s32>(tick)/60,r=static_cast<s32>(tick)%60;
        if(q!=0) {if(b.call_3210f4)b.call_3210f4(0x759Cu,load_le32(dg(m,0x609Cu)),static_cast<u32>(q),static_cast<u32>(r));}
        else {if(b.call_3210f3)b.call_3210f3(0x759Cu,load_le32(dg(m,0x60A0u)),static_cast<u32>(r));}
        app07_emit(b);
    }
    if(b.call_1e74c)b.call_1e74c(load_le32(dg(m,0xA7E4u)));
}
static_assert(0x050D9u-0x0461Cu==2749u);

[[nodiscard]] u32 function_050dc(Machine& m,u32 mode,u32 slot) noexcept {
    const u32 rec=0x218E0u+slot*0x248u;
    if((load_le16(dg(m,rec+0x10u))&0xF240u)!=0u)return 0xFFFFFFFFu;
    if(mode==0u) {
        const s8 a=static_cast<s8>(*dg(m,rec+0x15u));
        if(a==3)return 0u;
        if(static_cast<s8>(*dg(m,rec+0x16u))==3)return 1u;
        if(static_cast<s8>(*dg(m,rec+0x17u))==3)return 2u;
        if(a==1&&static_cast<s16>(load_le16(dg(m,rec+0x20u)))>=9)return 0u;
    } else {
        if(static_cast<s8>(*dg(m,rec+0x15u))==4)return 0u;
        if(static_cast<s8>(*dg(m,rec+0x16u))==4)return 1u;
        if(static_cast<s8>(*dg(m,rec+0x17u))==4)return 2u;
    }
    return 0xFFFFFFFFu;
}
static_assert(0x0518Cu-0x050DCu==176u);
