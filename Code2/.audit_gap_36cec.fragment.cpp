
// Semantic-audit repair: explicit reconstruction of 36CEC..372F5.
// Immutable span SHA-256: f0924e15e3109a1fe0b00a0cdc1e8748af213eaadc26048b32800dfbdc7dd80b
// This routine was present only in the historical aggregate byte baseline.
struct Runtime36cecBoundary {
    Runtime121Boundary* runtime121=nullptr;
    Runtime122Boundary* runtime122=nullptr;
    u32 (*call_3f29c)(u32,u32)=nullptr;
};

void function_36cec(Machine& m,Runtime36cecBoundary& b,
                    u32 arg08,u32 resource,u32 span_bits,u32 table_index,
                    u32 scale,u32 threshold) noexcept {
    s32 count=static_cast<s32>(span_bits>>3);
    u32 half_aligned=(span_bits&0xFFF8u)>>1;
    if(count<=0)return;
    const s32 cap=static_cast<s32>(load_le32(dg(m,0x0C738u)));
    if(count>cap)count=cap;

    u32 adjust;
    if(resource>=0x117u&&resource<0x13Bu){
        adjust=static_cast<u32>(count)<<1;
        store_le32(dg(m,0x10450u),adjust);
        adjust>>=3;
        store_le32(dg(m,0x10450u),load_le32(dg(m,0x10450u))+adjust);
    }else{
        adjust=static_cast<u32>(count)>>2;
        store_le32(dg(m,0x10450u),adjust);
    }
    count-=static_cast<s32>(adjust>>1);

    const u32 ti=half_aligned+table_index;
    if(static_cast<s32>(ti)>static_cast<s32>(load_le32(dg(m,0x0C730u))))
        store_le32(dg(m,0x105E7u),0u);
    else {
        const u32 table=load_le32(dg(m,0x258C4u));
        store_le32(dg(m,0x105E7u),load_le16(dg(m,table+ti*2u)));
    }
    if(load_le32(dg(m,0x105E7u))>0x808u)store_le32(dg(m,0x105E7u),0x808u);

    u32 count4=static_cast<u32>(count)<<2;
    u32 image=load_le32(dg(m,0x18FE0u+count4));
    store_le32(dg(m,0x105D7u),image);
    s32 patch_bound;
    if(scale==0u){
        s32 v=static_cast<s32>(load_le32(dg(m,0x10450u)))+
              ((0x100-static_cast<s32>(count4))>>1)+static_cast<s32>(count4>>3);
        patch_bound=0x80;
        if(v>0x100 && b.runtime122){
            patch_bound=function_36b9c(dg(m,image),v-0x100);
        }
        if(b.runtime122)function_36c10(*b.runtime122,dg(m,image),patch_bound);
    }else{
        s32 v=static_cast<s32>(load_le32(dg(m,0x10450u)))+
              static_cast<s32>(count4>>3)+((0x100-static_cast<s32>(count4))>>1);
        if(v<=0)return;
        patch_bound=v;
        const u64 prod=static_cast<u64>(count4<<1)*static_cast<u64>(scale);
        count4=static_cast<u32>((prod+0x40u)>>8);
        store_le32(dg(m,0x10450u),load_le32(dg(m,0x10450u))-count4);
        v-=static_cast<s32>(count4);
        patch_bound=-1;
        if(v<0 && b.runtime122)patch_bound=function_36bd8(dg(m,image),-v);
        if(patch_bound>=0 && b.runtime122)function_36c44(*b.runtime122,dg(m,image),patch_bound);
    }

    const u32 object=b.call_3f29c?b.call_3f29c(resource,3u):0u;
    const u32 p=object+2u;
    store_le32(dg(m,0x105D7u),load_le32(dg(m,0x105D7u))-0x204u);
    store_le32(dg(m,0x105DBu),p);
    const u32 word0=load_le16(dg(m,p));
    const u32 half=word0>>1;
    const u32 word2=load_le16(dg(m,p+2u));
    store_le32(dg(m,0x105E3u),arg08);
    s32 record_off=(static_cast<s32>(half)-1-static_cast<s32>(word2))*2+0x0A;
    s32 cursor=static_cast<s32>(half);

    auto record_value=[&](s32 idx)->s32{
        return static_cast<s32>(load_le32(dg(m,0x15A18u+static_cast<u32>(idx)*0x1Cu)));
    };
    auto invoke=[&](){ if(b.runtime121)function_36b14(*b.runtime121,p); };

    // Descending half of the encoded two-pass table walk.
    for(;;){
        store_le32(dg(m,0x105DBu),p);
        const u32 rp=p+static_cast<u32>(record_off);
        store_le32(dg(m,0x105DBu),load_le32(dg(m,0x105DBu))+load_le16(dg(m,rp)));
        record_off-=2;
        const u32 im=load_le32(dg(m,0x105D7u));
        const s32 run=static_cast<s32>(load_le16(dg(m,im+static_cast<u32>(cursor)*2u+0x102u)));
        store_le32(dg(m,0x105DFu),static_cast<u32>(run));
        if(run!=0){
            s32 pos=static_cast<s32>(load_le32(dg(m,0x105E3u)));
            if(run==1){
                --pos; store_le32(dg(m,0x105E3u),static_cast<u32>(pos));
                if(pos<static_cast<s32>(load_le32(dg(m,0x0C6D8u))) && record_value(pos)<threshold)invoke();
            }else{
                const s32 limit=static_cast<s32>(load_le32(dg(m,0x0C6D8u)));
                if(pos<=limit){ if(run>pos)store_le32(dg(m,0x105DFu),static_cast<u32>(pos)); }
                else {
                    pos-=run; store_le32(dg(m,0x105E3u),static_cast<u32>(pos));
                    const s32 clipped=limit-pos; store_le32(dg(m,0x105DFu),static_cast<u32>(clipped));
                    if(clipped<1)goto descend_next;
                }
                pos=static_cast<s32>(load_le32(dg(m,0x105E3u)));
                s32 n=static_cast<s32>(load_le32(dg(m,0x105DFu)));
                const bool first_low=record_value(pos)<threshold;
                const bool last_low=record_value(pos+n-1)<threshold;
                if(first_low){
                    if(last_low)invoke();
                    else { while(n>0&&record_value(pos+n-1)>=threshold)--n; store_le32(dg(m,0x105DFu),static_cast<u32>(n)); invoke(); }
                }else if(last_low){
                    while(n>0&&record_value(pos)>=threshold){++pos;--n;}
                    store_le32(dg(m,0x105E3u),static_cast<u32>(pos)); store_le32(dg(m,0x105DFu),static_cast<u32>(n)); invoke();
                }
            }
        }
descend_next:
        --cursor;
        if(cursor<static_cast<s32>(word2) || static_cast<s32>(load_le32(dg(m,0x105E3u)))<=0)break;
    }

    // Ascending half.
    store_le32(dg(m,0x105E3u),arg08);
    const u32 word4=load_le16(dg(m,p+4u));
    const s32 stop=static_cast<s32>(word4)+1;
    const s32 last=static_cast<s32>(half)-1;
    if(static_cast<s32>(word2)<last){ cursor=last; record_off=(last+1-static_cast<s32>(word2))*2+0x0A; }
    else { cursor=static_cast<s32>(word2)-1; record_off=0x0A; }
    store_le32(dg(m,0x105DFu),0u);
    for(;;){
        store_le32(dg(m,0x105DBu),p);
        const u32 rp=p+static_cast<u32>(record_off);
        store_le32(dg(m,0x105DBu),load_le32(dg(m,0x105DBu))+load_le16(dg(m,rp)));
        record_off+=2;
        const u32 im=load_le32(dg(m,0x105D7u));
        s32 run=static_cast<s32>(load_le16(dg(m,im+static_cast<u32>(cursor)*2u+0x102u)));
        store_le32(dg(m,0x105DFu),static_cast<u32>(run));
        if(run!=0){
            s32 pos=static_cast<s32>(load_le32(dg(m,0x105E3u)));
            if(run==1){
                if(pos>=0 && record_value(pos)<threshold)invoke();
            }else{
                const s32 limit=static_cast<s32>(load_le32(dg(m,0x0C6D8u)));
                if(pos<0){ if(-pos>=run)goto ascend_next; run+=pos; pos=0; store_le32(dg(m,0x105E3u),0u); store_le32(dg(m,0x105DFu),static_cast<u32>(run)); }
                else if(pos+run>limit){ run=limit-pos; store_le32(dg(m,0x105DFu),static_cast<u32>(run)); }
                if(run>0){
                    const bool first_low=record_value(pos)<threshold;
                    const bool last_low=record_value(pos+run-1)<threshold;
                    if(first_low){
                        if(last_low)invoke();
                        else { while(run>0&&record_value(pos+run-1)>=threshold)--run; store_le32(dg(m,0x105DFu),static_cast<u32>(run)); invoke(); }
                    }else if(last_low){
                        while(run>0&&record_value(pos)>=threshold){++pos;--run;}
                        store_le32(dg(m,0x105E3u),static_cast<u32>(pos)); store_le32(dg(m,0x105DFu),static_cast<u32>(run)); invoke();
                    }
                }
            }
        }
ascend_next:
        ++cursor;
        if(cursor>=stop)break;
        const s32 pos=static_cast<s32>(load_le32(dg(m,0x105E3u)))+static_cast<s32>(load_le32(dg(m,0x105DFu)));
        store_le32(dg(m,0x105E3u),static_cast<u32>(pos));
        if(pos>=static_cast<s32>(load_le32(dg(m,0x0C6D8u))))break;
    }

    image=load_le32(dg(m,0x105D7u));
    if(scale==0u){ if(b.runtime122)function_36c90(*b.runtime122,dg(m,image),patch_bound); }
    else if(patch_bound>=0 && b.runtime122)function_36cbc(*b.runtime122,dg(m,image),patch_bound);
}
static_assert(0x372F6u-0x36CECu==1546u);
