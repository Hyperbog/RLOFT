// Batch 122: runtime callback-table search/patch/restore helpers, 36B9C..36CEB.
struct Runtime122Boundary {
    std::array<u8,128> saved_callback_bytes{};
    std::array<u16,128> saved_callback_words{};
};

s32 function_36b9c(const u8* image,s32 threshold) noexcept {
    const u8* const table=image-0x102;
    u32 sum=0; s32 index=0x7f;
    do { u16 word=0; std::memcpy(&word,table+index*2,2); sum=(sum&0xffff0000u)|static_cast<u16>(static_cast<u16>(sum)+word); if(static_cast<s32>(sum)>threshold)break; --index; } while(index>=0);
    ++index; if(index>=0x7f)index=0x7f; return index;
}
static_assert(0x36BD8u-0x36B9Cu==60u);

s32 function_36bd8(const u8* image,s32 threshold) noexcept {
    const u8* const table=image-0x102;
    u32 sum=0; s32 index=0;
    while(index<0x80) { u16 word=0; std::memcpy(&word,table+index*2,2); sum=(sum&0xffff0000u)|static_cast<u16>(static_cast<u16>(sum)+word); if(static_cast<s32>(sum)>threshold)break; ++index; }
    return index;
}
static_assert(0x36C0Fu-0x36BD8u==55u);

void function_36c10(Runtime122Boundary& b,u8* image,s32 first) noexcept {
    u8* const table=image-0x204;
    for(s32 i=first;i<0x80;++i) { u16 off=0; std::memcpy(&off,table+i*2,2); b.saved_callback_bytes[static_cast<std::size_t>(i)]=table[off]; table[off]=0xc3; }
}
static_assert(0x36C44u-0x36C10u==52u);

void function_36c44(Runtime122Boundary& b,u8* image,s32 count) noexcept {
    u8* const table=image-0x204;
    for(s32 i=0;i<count;++i) { u16 off=0,next=0,old=0; std::memcpy(&off,table+i*2,2); std::memcpy(&old,table+off,2); std::memcpy(&next,table+(i+1)*2,2); b.saved_callback_words[static_cast<std::size_t>(i)]=old; const u16 delta=static_cast<u16>(next-off-2u); table[off]=0xeb; table[off+1]=static_cast<u8>(delta); }
}
static_assert(0x36C8Fu-0x36C44u==75u);

void function_36c90(Runtime122Boundary& b,u8* image,s32 last) noexcept {
    for(s32 i=0x7f;i>=last;--i) { u16 off=0; std::memcpy(&off,image+i*2,2); image[off]=b.saved_callback_bytes[static_cast<std::size_t>(i)]; }
}
static_assert(0x36CB9u-0x36C90u==41u);

void function_36cbc(Runtime122Boundary& b,u8* image,s32 count) noexcept {
    for(s32 i=count-1;i>=0;--i) { u16 off=0; std::memcpy(&off,image+i*2,2); const u16 old=b.saved_callback_words[static_cast<std::size_t>(i)]; std::memcpy(image+off,&old,2); }
}
static_assert(0x36CE9u-0x36CBCu==45u);
// 36C0F, 36C8F, 36CB9..36CBB, and 36CE9..36CEB are eight bytes of no-op alignment.
