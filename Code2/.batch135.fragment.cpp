
// -----------------------------------------------------------------------------
// Batch 135: independent formatter-adjacent helpers at 38A0A..38BC0.
// 38A30..38A60 is intentionally excluded: it jumps into the still-unowned
// formatter shared tail at 38862.
// -----------------------------------------------------------------------------
struct Runtime135Boundary {
    u8 (*read_far8)(u16,u32)=nullptr;
    u32 (*call_12700)(u32,u32,u32)=nullptr;
};

// 38A0A..38A2F: selector-qualified bounded byte-string length.
[[nodiscard]] u32 function_38a0a(Runtime135Boundary& b,u32 off,u16 sel,u32 limit) noexcept {
    u32 n=0u;
    for(;;){
        const u8 c=b.read_far8?b.read_far8(sel,off+n):0u;
        if(c==0u || n==limit)return n;
        ++n;
    }
}
static_assert(0x38A30u-0x38A0Au==38u);

// 38A61..38AC2: hexadecimal materializer, right-aligned in an exact-width
// field and zero-filled on the left.  The encoded helper always writes the
// terminator at dst+width.
void function_38a61(u32 value,u8* dst,u32 width) noexcept {
    static constexpr char digits[]="0123456789abcdefghijklmnopqrstuvwxyz";
    u8 tmp[33]{}; u32 n=0u;
    do { tmp[n++]=static_cast<u8>(digits[value%16u]); value/=16u; } while(value!=0u);
    for(u32 i=0;i<n/2u;++i){const u8 t=tmp[i];tmp[i]=tmp[n-1u-i];tmp[n-1u-i]=t;}
    u32 lead=width-n;
    for(u32 i=0;i<lead;++i)dst[i]=static_cast<u8>('0');
    for(u32 i=0;i<n;++i)dst[lead+i]=tmp[i];
    dst[width]=0u;
}
static_assert(0x38AC3u-0x38A61u==98u);

// 38AC3..38BA5: fixed-point decimal helper used by the formatter family.
// The input dword is treated exactly as the encoded signed 16.16 quantity:
// the high word is materialized in base ten, while repeated wrapping *10
// steps expose the next fractional byte through bits 16..23.  Bit 7 of the
// second byte drives the legacy final-digit rounding pass.
void function_38ac3(u8* dst,u32 bits,u32 precision) noexcept {
    u32 v=bits; u8* p=dst;
    if(static_cast<s32>(v)<0){*p++=static_cast<u8>('-');v=0u-v;}
    if(precision==0xFFFFFFFFu)precision=4u;
    const u16 whole=static_cast<u16>(v>>16);
    u8 rev[6]{};u32 n=0u;u32 q=whole;
    do{rev[n++]=static_cast<u8>('0'+(q%10u));q/=10u;}while(q!=0u);
    for(u32 i=0;i<n;++i)p[i]=rev[n-1u-i];
    p+=n;u8* integer_end=p;
    if(precision!=0u){
        *p++=static_cast<u8>('.');
        for(u32 i=0;i<precision;++i){v*=10u;*p++=static_cast<u8>(((v>>16)&0xFFu)+'0');v&=0xFFFFu;}
        *p=0u;
    } else *p=0u;
    if((bits&0x00008000u)!=0u){
        u8* r=p;
        if(r==integer_end){*r++=static_cast<u8>('1');*r=0u;return;}
        --r;
        for(;;){
            if(*r==static_cast<u8>('.')){*r=static_cast<u8>('0');++r;*r=static_cast<u8>('.');while(r[1]==static_cast<u8>('0'))++r;*r=static_cast<u8>('0');r[1]=0u;break;}
            if(*r!=static_cast<u8>('9')){++*r;break;}
            *r=static_cast<u8>('0');
            if(r==dst){for(u8* s=p+1;s!=dst;--s)*s=*(s-1);*dst=static_cast<u8>('1');break;}
            --r;
        }
    }
}
static_assert(0x38BA6u-0x38AC3u==227u);

// 38BA6..38BC0: address-distinct three-argument forwarding wrapper through
// the runtime function pointer stored at 12700h.
[[nodiscard]] u32 function_38ba6(Runtime135Boundary& b,u32 a,u32 c,u32 d) noexcept {
    return b.call_12700?b.call_12700(a,c,d):0u;
}
static_assert(0x38BC1u-0x38BA6u==27u);
