
// Batch 136: 38BC1..3908B — formatted-conversion dispatcher and case-fold helper.
// The dispatcher consumes arguments according to the parsed conversion record,
// materializes integer/string/fixed-point/pointer forms, derives sign/prefix and
// precision fields, then transfers to the already reconstructed formatter tail.
struct Runtime136FormatRecord {
    u32 width=0, precision=0, zero_count=0;
    u8 flags=0, conversion=0, pad=0, prefix0=0, prefix1=0, prefix2=0;
};
struct Runtime136Boundary {
    u32 (*next32)(u32&)=nullptr;
    u16 (*next16)(u32&)=nullptr;
    u32 (*strlen_far)(u32,u16,s32)=nullptr;
    void (*fixed_16_16)(u32,u32,Runtime136FormatRecord&)=nullptr;
    void (*utoa)(u32,u32,u32)=nullptr;
    void (*hex_width)(u32,u32,u32)=nullptr;
    u8 (*upper)(u8)=nullptr;
    void (*formatter_tail)(u32,u32,Runtime136FormatRecord&)=nullptr;
};

// 38BC1..3906B. The binary has one large switch-shaped dispatcher. It first
// preconsumes integer arguments for i/o/u/x/X/d, then dispatches E/F/G/e/f/g,
// integers, strings, pointers, chars and the literal/default case. Width and
// precision are normalized in-place; sign/prefix bytes occupy record +17..+19.
// The final encoded JMP 38861 is represented by formatter_tail rather than by
// duplicating the already-owned shared tail.
void function_38bc1(Runtime136Boundary& b,u32 out,u32& args,Runtime136FormatRecord& r) noexcept {
    u32 value=0u;
    const u8 c=r.conversion;
    const bool integer=(c=='i'||c=='d'||c=='o'||c=='u'||c=='x'||c=='X');
    if(integer && b.next32) value=b.next32(args);
    if(r.precision!=0xFFFFFFFFu) r.pad=' ';

    switch(c) {
    case 'o':
        if((r.flags&1u)!=0u && out!=0u) ++out;
        if(b.utoa) b.utoa(value,out,8u);
        break;
    case 'i': case 'd':
        if(b.utoa) b.utoa(value,out,10u);
        break;
    case 'u':
        if(b.utoa) b.utoa(value,out,10u);
        break;
    case 'x': case 'X':
        if((r.flags&1u)!=0u && value!=0u){r.prefix0='0';r.prefix1=c;r.prefix2=0u;}
        if(b.utoa) b.utoa(value,out,16u);
        break;
    case 'E': case 'F': case 'G': case 'e': case 'f': case 'g':
        if(b.fixed_16_16) b.fixed_16_16(out,b.next32?b.next32(args):0u,r);
        break;
    case 'p': case 'P':
        if(r.precision==0u) r.precision=(r.flags&0x80u)?13u:8u;
        if(b.hex_width) b.hex_width(value,out,(r.flags&0x80u)?4u:8u);
        break;
    case 'c':
        r.precision=1u;
        break;
    case 's': case 'S':
        if(b.strlen_far) (void)b.strlen_far(value,0xFFFFu,static_cast<s32>(r.precision));
        break;
    default:
        r.width=0u; r.precision=1u;
        break;
    }
    if(b.formatter_tail) b.formatter_tail(out,args,r);
}
static_assert(0x3906Cu-0x38BC1u==1195u);

// 3906C..3908B: uppercase a NUL-terminated byte string in place. The encoded
// empty-string exit jumps to the POP-EBX/RET epilogue at 38BBF; that shared tail
// is represented by ordinary function return here.
void function_3906c(Runtime136Boundary& b,u8* text) noexcept {
    if(text==nullptr) return;
    while(*text!=0u){ if(b.upper) *text=b.upper(*text); ++text; }
}
static_assert(0x3908Cu-0x3906Cu==32u);
