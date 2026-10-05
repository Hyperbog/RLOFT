// Batch 122: runtime callback-table search/patch/restore helpers, 36B9C..36CEB.
struct Runtime122Boundary { u8 saved_callback_bytes[128]{}; u16 saved_callback_words[128]{}; };

s32 function_36b9c(const u8* image,s32 threshold) noexcept {
    const u8* const table=image-0x102; u32 sum=0; s32 index=0x7f;
    do { const u16 word=load_le16(table+index*2); sum=(sum&0xffff0000u)|static_cast<u16>(static_cast<u16>(sum)+word); if(static_cast<s32>(sum)>threshold)break; --index; } while(index>=0);
    ++index; if(index>=0x7f)index=0x7f; return index;
}
static_assert(0x36BD8u-0x36B9Cu==60u);

s32 function_36bd8(const u8* image,s32 threshold) noexcept {
    const u8* const table=image-0x102; u32 sum=0; s32 index=0;
    while(index<0x80) { const u16 word=load_le16(table+index*2); sum=(sum&0xffff0000u)|static_cast<u16>(static_cast<u16>(sum)+word); if(static_cast<s32>(sum)>threshold)break; ++index; }
    return index;
}
static_assert(0x36C0Fu-0x36BD8u==55u);

void function_36c10(Runtime122Boundary& b,u8* image,s32 first) noexcept {
    u8* const table=image-0x204;
    for(s32 i=first;i<0x80;++i) { const u16 off=load_le16(table+i*2); b.saved_callback_bytes[i]=table[off]; table[off]=0xc3; }
}
static_assert(0x36C44u-0x36C10u==52u);

void function_36c44(Runtime122Boundary& b,u8* image,s32 count) noexcept {
    u8* const table=image-0x204;
    for(s32 i=0;i<count;++i) { const u16 off=load_le16(table+i*2); const u16 next=load_le16(table+(i+1)*2); b.saved_callback_words[i]=load_le16(table+off); const u16 delta=static_cast<u16>(next-off-2u); table[off]=0xeb; table[off+1]=static_cast<u8>(delta); }
}
static_assert(0x36C8Fu-0x36C44u==75u);

void function_36c90(Runtime122Boundary& b,u8* image,s32 last) noexcept {
    for(s32 i=0x7f;i>=last;--i) { const u16 off=load_le16(image+i*2); image[off]=b.saved_callback_bytes[i]; }
}
static_assert(0x36CB9u-0x36C90u==41u);

void function_36cbc(Runtime122Boundary& b,u8* image,s32 count) noexcept {
    for(s32 i=count-1;i>=0;--i) { const u16 off=load_le16(image+i*2); store_le16(image+off,b.saved_callback_words[i]); }
}
static_assert(0x36CE9u-0x36CBCu==45u);
// 36C0F, 36C8F, 36CB9..36CBB, and 36CE9..36CEB are eight bytes of no-op alignment.
