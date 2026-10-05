
// Batch 143: final explicit CODE.2 reconciliation.
// 385B0..38866 is the formatter/output core. 38A30..38A60 is the
// selector-qualified bounded word-string helper whose epilogue is shared at
// 38862 in the binary. The latter had been implicitly present in the historic
// baseline byte total; this batch makes that ownership explicit.
struct Runtime143FormatState {
    u32 width=0u;
    u32 data_length=0u;
    u32 zero_count=0u;
    u8 flags=0u;
    u8 conversion=0u;
    u8 pad=' ';
    u8 prefix[3]{0u,0u,0u};
};

struct Runtime143Boundary {
    // Mirrors the encoded output callback: the first argument is the opaque
    // formatter destination/context, while the callback owns count updates.
    void (*emit)(u32 out_context,u8 ch,u32& count)=nullptr;
    // 38BC1 prepares data pointer/selector and the width/length/prefix fields.
    void (*convert)(const u8*& args,Runtime143FormatState& state,
                    u32& data_off,u16& data_selector)=nullptr;
    u8 (*read_far8)(u16 selector,u32 off)=nullptr;
    void (*store_far16)(u16 selector,u32 off,u16 value)=nullptr;
    void (*store_far32)(u16 selector,u32 off,u32 value)=nullptr;
    u16 (*read_far16)(u16 selector,u32 off)=nullptr;
};

static void batch143_emit(Runtime143Boundary& b,u32 out,u8 ch,u32& count) noexcept {
    if(b.emit)b.emit(out,ch,count);
}

// 385B0..38866: formatted-output driver. Parsing of width/precision/modifiers
// remains owned by 38867; conversion materialization remains owned by 38BC1.
// This fold owns the byte-for-byte control between those helpers: literal
// emission, %n width/far-pointer stores, pre/post padding, prefixes, zero fill,
// byte/word-string output and left-justified trailing spaces.
[[nodiscard]] u32 function_385b0(Runtime143Boundary& b,const u8* format,
                                 const u8*& args,u32 out_context) noexcept {
    const u8* p=format;
    u32 count=0u;
    for(;;){
        const u8 ch=*p;
        if(ch==0u)return count;
        ++p;
        if(ch!='%'){
            batch143_emit(b,out_context,ch,count);
            continue;
        }

        Runtime134Format parsed{};
        const u8* q=function_38867(p,args,parsed);
        Runtime143FormatState s{};
        s.width=parsed.width;
        s.flags=parsed.flags;
        s.conversion=*q++;
        s.pad=parsed.pad;
        p=q;
        if(s.conversion==0u)return count;

        if(s.conversion=='n'){
            const bool word=(s.flags&0x10u)!=0u;
            const bool farp=(s.flags&0x80u)!=0u;
            const bool near_explicit=(s.flags&0x40u)!=0u;
            if(farp){
                const u32 off=load_le32(args); const u16 sel=load_le16(args+4u); args+=8u;
                if(word){if(b.store_far16)b.store_far16(sel,off,static_cast<u16>(count));}
                else {if(b.store_far32)b.store_far32(sel,off,count);}
            }else{
                const u32 off=load_le32(args); args+=4u;
                (void)near_explicit;
                if(word){if(b.store_far16)b.store_far16(0u,off,static_cast<u16>(count));}
                else {if(b.store_far32)b.store_far32(0u,off,count);}
            }
            continue;
        }

        u32 data_off=0u; u16 data_sel=0u;
        if(b.convert)b.convert(args,s,data_off,data_sel);

        if((s.flags&0x08u)==0u && s.pad==' '){
            while(static_cast<s32>(--s.width)>=0)batch143_emit(b,out_context,s.pad,count);
        }
        for(u32 i=0u;i<3u && s.prefix[i]!=0u;++i)
            batch143_emit(b,out_context,s.prefix[i],count);
        while(s.zero_count--!=0u)batch143_emit(b,out_context,'0',count);
        if((s.flags&0x08u)==0u && s.pad!=' '){
            while(static_cast<s32>(--s.width)>=0)batch143_emit(b,out_context,s.pad,count);
        }

        if(s.conversion=='s'||s.conversion=='S'){
            if((s.flags&0x20u)!=0u){
                while(s.data_length--!=0u){
                    const u8 v=b.read_far8?b.read_far8(data_sel,data_off):0u;
                    batch143_emit(b,out_context,v,count); data_off+=2u;
                }
            }else{
                while(s.data_length--!=0u){
                    const u8 v=b.read_far8?b.read_far8(data_sel,data_off):0u;
                    batch143_emit(b,out_context,v,count); ++data_off;
                }
            }
        }else{
            while(s.data_length--!=0u){
                const u8 v=b.read_far8?b.read_far8(data_sel,data_off):0u;
                batch143_emit(b,out_context,v,count); ++data_off;
            }
        }
        if((s.flags&0x08u)!=0u && static_cast<s32>(s.width)>0)
            while(s.width--!=0u)batch143_emit(b,out_context,' ',count);
    }
}
static_assert(0x38867u-0x385B0u==695u);

// 38A30..38A60: bounded selector-qualified 16-bit-string length. The encoded
// zero/limit exits jump backward to the POP ES/EDI/ESI/EBX/RET tail at 38862.
[[nodiscard]] u32 function_38a30(Runtime143Boundary& b,u32 off,u16 selector,u32 limit) noexcept {
    u32 n=0u;
    for(;;){
        const u16 v=b.read_far16?b.read_far16(selector,off):0u;
        off+=2u;
        if(v==0u || n==limit)return n;
        ++n;
    }
}
static_assert(0x38A61u-0x38A30u==49u);
static_assert((0x38867u-0x385B0u)+(0x38A61u-0x38A30u)==744u);
