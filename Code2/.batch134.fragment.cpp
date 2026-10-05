
// Batch 134: 38867..38A09 -- printf field/precision/length parser.
struct Runtime134Format {
    u32 reserved0=0;
    u32 width=0;
    u32 precision=0xFFFFFFFFu;
    u32 reserved_c=0;
    u32 reserved_10=0;
    u8 flags=0;          // encoded byte at +14h
    u8 conversion=0;     // +15h, consumed by the formatter proper
    u8 pad=' ';          // +16h
    u8 reserved_17=0;    // +17h
};
static_assert(sizeof(Runtime134Format)==24u);

// 389A7..38A09: consume the flag prefix.  '+' dominates space exactly as encoded;
// '0' changes the pad byte without introducing a separate flag bit.
[[nodiscard]] const u8* function_389a7(const u8* p,Runtime134Format& f) noexcept {
    f.flags=0u;
    for(;;){
        const u8 ch=*p;
        if(ch=='-'){f.flags=static_cast<u8>(f.flags|0x08u);++p;continue;}
        if(ch=='#'){f.flags=static_cast<u8>(f.flags|0x01u);++p;continue;}
        if(ch=='+'){
            f.flags=static_cast<u8>((f.flags|0x04u)&0xFDu);
            ++p;continue;
        }
        if(ch==' '){
            if((f.flags&0x04u)==0u)f.flags=static_cast<u8>(f.flags|0x02u);
            ++p;continue;
        }
        if(ch=='0'){f.pad='0';++p;continue;}
        return p;
    }
}
static_assert(0x38A0Au-0x389A7u==99u);

// 38867..389A6: parse width, precision, and the compiler runtime's length/far
// modifiers.  args points at the cdecl variable-argument byte stream and is
// advanced by one 32-bit slot for each '*' consumed.
[[nodiscard]] const u8* function_38867(const u8* p,const u8*& args,Runtime134Format& f) noexcept {
    f.reserved_17=0u;
    f.pad=' ';
    p=function_389a7(p,f);
    f.width=0u;
    while(*p>='0' && *p<='9'){
        f.width=f.width*10u+static_cast<u32>(*p-'0');
        ++p;
    }
    if(*p=='*'){
        const u32 raw=load_le32(args); args+=4;
        f.width=raw;
        if(static_cast<s32>(raw)<0){
            f.width=0u-raw;
            f.flags=static_cast<u8>(f.flags|0x08u);
        }
        ++p;
    }
    f.precision=0xFFFFFFFFu;
    if(*p=='.'){
        ++p;
        f.precision=0u;
        if(*p=='*'){
            const u32 raw=load_le32(args); args+=4;
            f.precision=(static_cast<s32>(raw)<0)?0xFFFFFFFFu:raw;
            ++p;
        }else{
            while(*p>='0' && *p<='9'){
                f.precision=f.precision*10u+static_cast<u32>(*p-'0');
                ++p;
            }
        }
    }
    switch(*p){
        case 'l': case 'w': f.flags=static_cast<u8>(f.flags|0x20u); return p+1;
        case 'h': f.flags=static_cast<u8>(f.flags|0x10u); return p+1;
        case 'L': case 'N': f.flags=static_cast<u8>(f.flags|0x40u); return p+1;
        case 'F': f.flags=static_cast<u8>(f.flags|0x80u); return p+1;
        default: return p;
    }
}
static_assert(0x389A7u-0x38867u==320u);
