// Batch 84: 32954..32F49 world/grid sweep state machine with dynamic Jcc patch bytes.
struct Application84Boundary {
    void (*code_store8)(u32,u8)=nullptr;
    u32 (*call_356d8)(u32,u32)=nullptr;
    void (*call_32f4c)()=nullptr;
    void (*call_32f8c)()=nullptr;
    void (*call_33124)()=nullptr;
    void (*call_332b4)()=nullptr;
    void (*call_333bc)()=nullptr;
    u32 (*call_1e930)(u32)=nullptr;
};
static u32 app84_mul_words(s16 a,s16 b) noexcept { return static_cast<u32>(a)*static_cast<u32>(b); }
static u32 app84_add_ax(u32 eax,u16 value) noexcept { return (eax&0xFFFF0000u)|static_cast<u16>(static_cast<u16>(eax)+value); }
static u32 app84_sar1(u32 value) noexcept { return (value>>1)|(value&0x80000000u); }
static u32 app84_ror16(u32 value) noexcept { return (value>>16)|(value<<16); }
static u32 app84_table32(Machine& m,u32 base,s32 index) noexcept { return load_le32(dg(m,base+static_cast<u32>(index)*4u)); }

void function_32954(Machine& m,Application84Boundary& b) noexcept {
    store_le32(dg(m,0xFCE8u),0xFFFFFFFFu);
    store_le32(dg(m,0xFCECu),0xFFFFFFFFu);
    if(load_le32(dg(m,0xCF50u))==0u &&
       load_le32(dg(m,0xFD04u))==load_le32(dg(m,0xC658u)) &&
       load_le32(dg(m,0x255D0u))==load_le32(dg(m,0xFCFCu)) &&
       load_le32(dg(m,0x255D4u))==load_le32(dg(m,0xFD00u)))return;

    store_le32(dg(m,0xC6FCu),0xFFFFFFFFu);
    for(;;){
        const u32 slot=load_le32(dg(m,0xC6FCu))+1u;
        store_le32(dg(m,0xC6FCu),slot);
        if(load_le32(dg(m,0xC6D8u))<=slot)return;

        s32 angle=static_cast<s32>(load_le32(dg(m,0xC73Cu+slot*4u))+load_le32(dg(m,0xC714u)));
        const u32 table=load_le32(dg(m,0xC660u));
        store_le32(dg(m,0xFCD4u),0u);store_le32(dg(m,0xFCD8u),0u);
        u32 first_arg2=0u,second_arg2=0u;
        bool left_ge=false,right_ge=false;
        int region=0;
        if(angle<0){angle=static_cast<s32>(static_cast<u32>(angle)+0xE10u);region=3;}
        else if(angle<0x384)region=0;
        else if(angle<0x708)region=1;
        else if(angle<0xA8C)region=2;
        else if(angle<0xE10)region=3;
        else {angle=static_cast<s32>(static_cast<u32>(angle)-0xE10u);region=0;}

        if(region==0){
            store_le32(dg(m,0x25610u),0x00010000u);store_le32(dg(m,0x25604u),0xFFFF0000u);
            if(b.code_store8){b.code_store8(0x32C1Au,0x8Eu);b.code_store8(0x32D04u,0x8Du);}left_ge=false;right_ge=true;
            store_le32(dg(m,0x25608u),app84_table32(m,table,0x383-angle));
            store_le32(dg(m,0x25600u),0u-app84_table32(m,table,angle));
            first_arg2=load_le32(dg(m,0xC704u));second_arg2=load_le32(dg(m,0xC710u));
        }else if(region==1){
            store_le32(dg(m,0x25610u),0xFFFF0000u);store_le32(dg(m,0x25604u),0xFFFF0000u);
            if(b.code_store8){b.code_store8(0x32C1Au,0x8Eu);b.code_store8(0x32D04u,0x8Eu);}left_ge=false;right_ge=false;
            store_le32(dg(m,0x25608u),0u-app84_table32(m,table,angle-0x384));
            store_le32(dg(m,0x25600u),0u-app84_table32(m,table,0x707-angle));
            first_arg2=load_le32(dg(m,0xC704u));second_arg2=load_le32(dg(m,0xC70Cu));
        }else if(region==2){
            store_le32(dg(m,0x25610u),0xFFFF0000u);store_le32(dg(m,0x25604u),0x00010000u);
            if(b.code_store8){b.code_store8(0x32C1Au,0x8Du);b.code_store8(0x32D04u,0x8Eu);}left_ge=true;right_ge=false;
            store_le32(dg(m,0x25608u),0u-app84_table32(m,table,0xA8B-angle));
            store_le32(dg(m,0x25600u),app84_table32(m,table,angle-0x708));
            first_arg2=load_le32(dg(m,0xC708u));second_arg2=load_le32(dg(m,0xC70Cu));
        }else{
            store_le32(dg(m,0x25610u),0x00010000u);store_le32(dg(m,0x25604u),0x00010000u);
            if(b.code_store8){b.code_store8(0x32C1Au,0x8Du);b.code_store8(0x32D04u,0x8Du);}left_ge=true;right_ge=true;
            store_le32(dg(m,0x25608u),app84_table32(m,table,angle-0xA8C));
            store_le32(dg(m,0x25600u),app84_table32(m,table,0xE0F-angle));
            first_arg2=load_le32(dg(m,0xC708u));second_arg2=load_le32(dg(m,0xC710u));
        }

        const u32 r0=b.call_356d8?b.call_356d8(load_le32(dg(m,0x25608u)),first_arg2):0u;
        store_le32(dg(m,0x255F0u),r0+load_le32(dg(m,0x255ECu)));
        s16 cx=static_cast<s16>(load_le16(dg(m,0x255F2u)));
        const u32 r1=b.call_356d8?b.call_356d8(load_le32(dg(m,0x25600u)),second_arg2):0u;
        store_le32(dg(m,0x255E8u),r1+load_le32(dg(m,0x255F4u)));
        s16 bx=static_cast<s16>(load_le16(dg(m,0x255EAu)));

        store_le32(dg(m,0x248E4u),(load_le32(dg(m,0xC724u))<<16)+load_le32(dg(m,0x25610u)));
        store_le32(dg(m,0x248E8u),(load_le32(dg(m,0xC720u))<<16)+load_le32(dg(m,0x25604u)));
        const s16 scale=static_cast<s16>(load_le16(dg(m,0x25619u)));
        u32 left_expr=app84_mul_words(static_cast<s16>(load_le16(dg(m,0x248E6u))),scale)+static_cast<u32>(static_cast<s32>(bx));
        u32 right_expr=app84_add_ax(app84_mul_words(cx,scale),load_le16(dg(m,0x248EAu)));

        enum Sweep{CompareLeft,Left,CompareRight,Right};
        Sweep state=CompareLeft;bool done=false;
        while(!done){
            if(state==CompareLeft){
                const s16 edge=static_cast<s16>(load_le16(dg(m,0x248EAu)));
                const bool jump_right=left_ge?(bx>=edge):(bx<=edge);
                state=jump_right?Right:Left;continue;
            }
            if(state==CompareRight){
                const s16 edge=static_cast<s16>(load_le16(dg(m,0x248E6u)));
                const bool jump_left=right_ge?(cx>=edge):(cx<=edge);
                state=jump_left?Left:Right;continue;
            }
            if(state==Left){
                const u32 index=left_expr>>1;store_le32(dg(m,0xFCDCu),index);
                if(index>=load_le32(dg(m,0xFD0Cu))){if(b.call_32f4c)b.call_32f4c();done=true;continue;}
                const u32 rec=0x198E0u+index*2u;u16 flags=load_le16(dg(m,rec));
                const bool treat_empty=(flags&0x003Fu)==0u || (load_le32(dg(m,0xFD08u))!=0u && (flags&0x0800u)!=0u);
                if(!treat_empty){
                    flags=static_cast<u16>(flags|0x0440u);store_le16(dg(m,rec),flags);
                    store_le32(dg(m,0xFCD4u),(flags&0x0200u)>>9);store_le32(dg(m,0xC700u),flags&0x003Fu);
                    if((flags&0x0080u)==0u){
                        store_le32(dg(m,0x255F0u),load_le32(dg(m,0x248E4u))&0xFFFF0000u);store_le16(dg(m,0x248EAu),static_cast<u16>(bx));
                        if(b.call_32f8c)b.call_32f8c();cx=static_cast<s16>(load_le16(dg(m,0x255F2u)));done=true;continue;
                    }
                    u32 value=app84_sar1(load_le32(dg(m,0x25600u)))+load_le32(dg(m,0x255E8u));
                    u32 rotated=app84_ror16(value);
                    if(static_cast<u16>(bx)==static_cast<u16>(rotated)){
                        value=app84_ror16(rotated);
                        const u32 cell=load_le32(dg(m,0xFCDCu));
                        if(cell!=load_le32(dg(m,0xFCE8u))){const s16 q=static_cast<s16>(b.call_1e930?b.call_1e930(cell):0u);store_le32(dg(m,0xFCE0u),static_cast<u32>(static_cast<s32>(q)));store_le32(dg(m,0xFCE4u),0u-static_cast<u32>(static_cast<s32>(q)));store_le32(dg(m,0xFCE8u),cell);}
                        const u32 probe=static_cast<u32>(static_cast<s32>(static_cast<s16>(value)));
                        const u32 lo=load_le32(dg(m,0xFCE0u)),hi=load_le32(dg(m,0xFCE4u));
                        if(lo!=0u && (probe<=lo || probe>=hi)){
                            store_le32(dg(m,0x255E8u),value);store_le32(dg(m,0x255F0u),(load_le32(dg(m,0x248E4u))&0xFFFF0000u)|0x8000u);
                            if(b.call_332b4)b.call_332b4();cx=static_cast<s16>(load_le16(dg(m,0x255F2u)));bx=static_cast<s16>(load_le16(dg(m,0x255EAu)));done=true;continue;
                        }
                    }
                }else{flags=static_cast<u16>(flags|0x0440u);store_le16(dg(m,rec),flags);}
                store_le32(dg(m,0x248E4u),load_le32(dg(m,0x248E4u))+load_le32(dg(m,0x25610u)));
                store_le32(dg(m,0x255E8u),load_le32(dg(m,0x255E8u))+load_le32(dg(m,0x25600u)));
                bx=static_cast<s16>(load_le16(dg(m,0x255EAu)));
                left_expr=app84_mul_words(static_cast<s16>(load_le16(dg(m,0x248E6u))),scale)+static_cast<u32>(static_cast<s32>(bx));store_le32(dg(m,0xFCDCu),left_expr);state=CompareLeft;continue;
            }

            const u32 index=right_expr>>1;store_le32(dg(m,0xFCDCu),index);
            if(index>=load_le32(dg(m,0xFD0Cu))){if(b.call_32f4c)b.call_32f4c();done=true;continue;}
            const u32 rec=0x198E0u+index*2u;u16 flags=load_le16(dg(m,rec));
            const bool treat_empty=(flags&0x003Fu)==0u || (load_le32(dg(m,0xFD08u))!=0u && (flags&0x0800u)!=0u);
            if(!treat_empty){
                flags=static_cast<u16>(flags|0x0440u);store_le16(dg(m,rec),flags);
                store_le32(dg(m,0xFCD4u),(flags&0x0200u)>>9);store_le32(dg(m,0xC700u),flags&0x003Fu);
                if((flags&0x0080u)==0u){
                    store_le32(dg(m,0x255E8u),load_le32(dg(m,0x248E8u))&0xFFFF0000u);store_le16(dg(m,0x248E6u),static_cast<u16>(cx));
                    if(b.call_33124)b.call_33124();bx=static_cast<s16>(load_le16(dg(m,0x255EAu)));done=true;continue;
                }
                u32 value=app84_sar1(load_le32(dg(m,0x25608u)))+load_le32(dg(m,0x255F0u));
                u32 rotated=app84_ror16(value);
                if(static_cast<u16>(cx)==static_cast<u16>(rotated)){
                    value=app84_ror16(rotated);
                    const u32 cell=load_le32(dg(m,0xFCDCu));
                    if(cell!=load_le32(dg(m,0xFCE8u))){const s16 q=static_cast<s16>(b.call_1e930?b.call_1e930(cell):0u);store_le32(dg(m,0xFCE0u),static_cast<u32>(static_cast<s32>(q)));store_le32(dg(m,0xFCE4u),0u-static_cast<u32>(static_cast<s32>(q)));store_le32(dg(m,0xFCE8u),cell);}
                    const u32 probe=static_cast<u32>(static_cast<s32>(static_cast<s16>(value)));
                    const u32 lo=load_le32(dg(m,0xFCE0u)),hi=load_le32(dg(m,0xFCE4u));
                    if(lo!=0u && (probe<=lo || probe>=hi)){
                        store_le32(dg(m,0x255F0u),value);store_le32(dg(m,0x255E8u),(load_le32(dg(m,0x248E8u))&0xFFFF0000u)|0x8000u);
                        if(b.call_333bc)b.call_333bc();cx=static_cast<s16>(load_le16(dg(m,0x255F2u)));bx=static_cast<s16>(load_le16(dg(m,0x255EAu)));done=true;continue;
                    }
                }
            }else{flags=static_cast<u16>(flags|0x0440u);store_le16(dg(m,rec),flags);}
            store_le32(dg(m,0x248E8u),load_le32(dg(m,0x248E8u))+load_le32(dg(m,0x25604u)));
            store_le32(dg(m,0x255F0u),load_le32(dg(m,0x255F0u))+load_le32(dg(m,0x25608u)));
            cx=static_cast<s16>(load_le16(dg(m,0x255F2u)));
            right_expr=app84_add_ax(app84_mul_words(cx,scale),load_le16(dg(m,0x248EAu)));store_le32(dg(m,0xFCDCu),right_expr);state=CompareRight;
        }
    }
}
static_assert(0x32F4Au-0x32954u==1526u);
// 32F4A..32F4B is XCHG EBX,EBX alignment with no incoming direct control-flow target.
