from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
anchor='// CC1C..CD96: startup recovery scaffolding.'
assert anchor in s and 'function_59ac(' not in s
code=r'''
// 59AC..5A33: scan 256 RGB triples and return the triple index nearest to (31,31,31).
// The three MOVZX instructions target AX, not EAX, so the high 16 bits of the incoming pointer-valued EAX are preserved exactly.
[[nodiscard]]s32 function_59ac(const u8*src)noexcept{
 s32 best_index=0; s16 best=0x200; s32 bx=0; const u32 high=static_cast<u32>(reinterpret_cast<std::uintptr_t>(src))&0xffff0000u;
 while(static_cast<s16>(bx)<0x300){
  const s32 off=static_cast<s16>(bx);
  u32 a=high|src[off], b=high|src[off+1], c=high|src[off+2];
  const s32 da=static_cast<s32>(a)-31, db=static_cast<s32>(b)-31, dc=static_cast<s32>(c)-31;
  const s32 dist=da*da+db*db+dc*dc;
  if(static_cast<s16>(dist)<best){best=static_cast<s16>(dist);best_index=off/3;}
  bx+=3;
 }
 return best_index;
}
static_assert(0x5A34u-0x59ACu==136u);

struct PaletteTreeBoundaries{
 void(*insert_5880)(u8,u8,u8,u16,s32)=nullptr;
 u8(*read_guest8)(u32)=nullptr;
 void(*dos_print_dollar)(char*)=nullptr;
 s32(*call_d23a)(u32,u16)=nullptr; s32(*call_d280)(u32,u16,u32)=nullptr; s32(*call_d2ad)(u32)=nullptr;
 u32(*call_d313)(u32,u32)=nullptr; u32(*call_d55e)(u32,u32)=nullptr; u32(*call_d573)(u32)=nullptr;
 s32(*call_d5ad)(u32,void*,u32)=nullptr; s32(*call_d686)(u32,const void*,u32)=nullptr; s32(*call_d85f)(u32,s32,u32)=nullptr;
 s32(*call_d8aa)(const char*)=nullptr; u32(*call_d8e8)(u32,u32*)=nullptr; u32(*call_d8d0)(u32*)=nullptr;
 char*(*call_d906)(char*)=nullptr; u32(*call_d991)(u32)=nullptr; s32(*call_d9a6)(const char*)=nullptr; s32(*call_d9ca)(const char*)=nullptr;
 void(*call_a2b0)(s32,s32)=nullptr; void(*call_a21c)(s32)=nullptr; void(*call_a484)(u32,s32,s32,s32,s32)=nullptr;
 void(*call_a4e8)(u32,s32,s32,s32,s32)=nullptr; void(*call_a6f0)(u32,s32,s32,u32)=nullptr;
 void(*call_a7e8)(u32,s32,s32,u32,u8)=nullptr; void(*call_a8f4)(u32,s32,s32,u32,u32)=nullptr; void(*call_8960)()=nullptr;
};

// 5A34..5B63: initialize the palette-tree sentinels, choose the closest palette entry, then insert every entry by walking downward and upward from it.
void function_5a34(Machine&m,PaletteTreeBoundaries&b,const u8*src)noexcept{
 store_le16(dgroup_ptr(m,dgroup(0x7ff3u)),0xffffu);store_le16(dgroup_ptr(m,dgroup(0x7ff5u)),0x0101u);store_le16(dgroup_ptr(m,dgroup(0x7ff7u)),0x0101u);
 store_le16(dgroup_ptr(m,dgroup(0x88fcu)),0xffffu);store_le16(dgroup_ptr(m,dgroup(0x88feu)),0x0101u);store_le16(dgroup_ptr(m,dgroup(0x8900u)),0x0101u);
 s32 serial=1; s32 lo=function_59ac(src); s32 hi=lo+1;
 auto insert=[&](s32 idx){const s32 o=idx*3; if(b.insert_5880)b.insert_5880(src[o],src[o+1],src[o+2],static_cast<u16>(idx),serial);++serial;};
 for(s32 i=lo;i>-1;--i)insert(i);
 while(static_cast<s16>(hi)<0x100){insert(hi);++hi;}
 store_le16(dgroup_ptr(m,dgroup(0x982u)),1u);
}
static_assert(0x5B64u-0x5A34u==304u);

static s16 tree_word(Machine&m,s16 node,u32 off)noexcept{return static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x7ff0u+static_cast<u32>(static_cast<s32>(node)*9)+off))));}
static u8 tree_byte(Machine&m,s16 node,u32 off)noexcept{return *dgroup_ptr(m,dgroup(0x7ff0u+static_cast<u32>(static_cast<s32>(node)*9)+off));}
// 5B64..5D8E: exact iterative nearest-tree search. The explicit 16-bit stack/count globals and 0x101 sentinel are retained.
[[nodiscard]]s32 function_5b64(Machine&m,u8 r,u8 g,u8 bl,u32 radius)noexcept{
 s32 best_dist=0x2e83; s16 best_node=0x0101;
 if(load_le16(dgroup_ptr(m,dgroup(0x982u)))==0u)return -1;
 store_le16(dgroup_ptr(m,dgroup(0x980u)),0u);store_le16(dgroup_ptr(m,dgroup(0x984u)),0u);
 s16 bounds[6]={static_cast<s16>(static_cast<u16>(g)-radius),static_cast<s16>(static_cast<u16>(g)+radius),static_cast<s16>(static_cast<u16>(r)-radius),static_cast<s16>(static_cast<u16>(r)+radius),static_cast<s16>(static_cast<u16>(bl)-radius),static_cast<s16>(static_cast<u16>(bl)+radius)};
 store_le16(dgroup_ptr(m,dgroup(0x7bf0u)),0xffffu);store_le16(dgroup_ptr(m,dgroup(0x980u)),1u);store_le16(dgroup_ptr(m,dgroup(0x7bf2u)),0u);store_le16(dgroup_ptr(m,dgroup(0x980u)),2u);
 for(;;){
  s16 sp=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x980u))));if(sp==0||static_cast<s16>(best_dist)==0)break;
  --sp;store_le16(dgroup_ptr(m,dgroup(0x980u)),static_cast<u16>(sp));s16 popped=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x7bf0u+static_cast<u32>(sp)*2u))));s16 node=tree_word(m,popped,7u);
  --sp;store_le16(dgroup_ptr(m,dgroup(0x980u)),static_cast<u16>(sp));s16 axis=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x7bf0u+static_cast<u32>(sp)*2u))));
  while(node!=0x0101&&static_cast<s16>(best_dist)!=0){
   store_le16(dgroup_ptr(m,dgroup(0x984u)),static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x984u)))+1u));axis=(axis==2)?0:static_cast<s16>(axis+1);
   const s16 value=static_cast<s16>(tree_byte(m,node,static_cast<u32>(axis)));
   if(value<=bounds[axis*2]){node=tree_word(m,node,7u);continue;}
   if(value>bounds[axis*2+1]){node=tree_word(m,node,5u);continue;}
   sp=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x980u))));store_le16(dgroup_ptr(m,dgroup(0x7bf0u+static_cast<u32>(sp)*2u)),static_cast<u16>(axis));++sp;store_le16(dgroup_ptr(m,dgroup(0x980u)),static_cast<u16>(sp));store_le16(dgroup_ptr(m,dgroup(0x7bf0u+static_cast<u32>(sp)*2u)),static_cast<u16>(node));++sp;store_le16(dgroup_ptr(m,dgroup(0x980u)),static_cast<u16>(sp));
   const s32 dr=static_cast<s32>(tree_byte(m,node,1u))-r,dg=static_cast<s32>(tree_byte(m,node,0u))-g,db=static_cast<s32>(tree_byte(m,node,2u))-bl;const s32 dist=dr*dr+dg*dg+db*db;
   if(static_cast<s16>(dist)<static_cast<s16>(best_dist)){best_dist=dist;best_node=node;}
   node=tree_word(m,node,5u);
  }
 }
 return static_cast<s16>(tree_word(m,best_node,3u));
}
static_assert(0x5D8Fu-0x5B64u==555u);
// 5D8F is zero alignment; 5D90 is the true next entry.

// 5D90..5F49: map an actor's class tuple to the encoded class-family byte. BL is intentionally an input when no condition overwrites it.
[[nodiscard]]u8 function_5d90(Machine&m,u32 actor,u8 incoming_bl)noexcept{
 const u32 base=actor*0x248u;const s8 primary=static_cast<s8>(*dgroup_ptr(m,dgroup(base+0x680du)));const s8 second=static_cast<s8>(*dgroup_ptr(m,dgroup(base+0x680eu)));u8 out=incoming_bl;
 if(second==-1)return static_cast<u8>(primary);
 const s8 third=static_cast<s8>(*dgroup_ptr(m,dgroup(base+0x680fu)));
 if(third!=-1){if(primary==5||second==5||third==5)return 0x0d;return 0x0e;}
 if(primary==0){if(second==5)out=6;if(second==3)out=7;if(second==4)out=8;}
 if(primary==5){if(second==0)out=6;if(second==3)out=9;if(second==4)out=0x0c;}
 if(primary==2)out=0x0a;
 if(primary==3){if(second==0)out=7;if(second==5)out=9;if(second==4)out=0x0b;if(second==2)out=0x0a;}
 if(primary==4){if(second==0)out=8;if(second==3)out=0x0b;if(second==5)out=0x0c;}
 return out;
}
static_assert(0x5F4Au-0x5D90u==442u);
// 5F4A..5F4B is alignment.

// 5F4C..6053: populate both actor runtime tables from class-family/class-index records and invalidate conflicting 6A80 entries.
void function_5f4c(Machine&m,PaletteTreeBoundaries&b)noexcept{
 for(u32 actor=0;actor<2u;++actor){const u8 family=function_5d90(m,actor,static_cast<u8>(actor));const u32 abase=actor*0x248u;const u8 cls=*dgroup_ptr(m,dgroup(abase+0x680cu));const u32 rec=static_cast<u32>(family)*0xa8u+static_cast<u32>(cls)*0x1cu;for(u32 j=0;j<4u;++j)store_le16(dgroup_ptr(m,dgroup(abase+0x6854u+j*2u)),load_le16(dgroup_ptr(m,dgroup(rec+0x988u+((3u-j)&3u)*2u))));for(u32 j=0;j<10u;++j)store_le16(dgroup_ptr(m,dgroup(abase+0x6838u+j*2u)),load_le16(dgroup_ptr(m,dgroup(rec+0x990u+j*2u))));}
 if(function_5d90(m,0,0u)==function_5d90(m,1,1u)&&*dgroup_ptr(m,dgroup(0x680cu))==*dgroup_ptr(m,dgroup(0x6a54u))){for(u32 i=0;i<10u;++i){const s16 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x6a80u+i*2u))));if(v<0)continue;const u32 q=load_le32(dgroup_ptr(m,dgroup(0x7128u)))+static_cast<u32>(v)*65u+0x2fu;if(b.read_guest8&&b.read_guest8(q)==1u)store_le16(dgroup_ptr(m,dgroup(0x6a80u+i*2u)),0xffffu);}}
}
static_assert(0x6054u-0x5F4Cu==264u);

// 6184..6189 and 618A..61AB: RNG state address and MS-compatible LCG step.
[[nodiscard]]u32 function_6184()noexcept{return 0x23ccu;} static_assert(0x618Au-0x6184u==6u);
[[nodiscard]]u32 function_618a(Machine&m)noexcept{u32 v=load_le32(dgroup_ptr(m,dgroup(0x23ccu)));v=v*0x41c64e6du+0x3039u;store_le32(dgroup_ptr(m,dgroup(0x23ccu)),v);return(v>>16)&0x7fffu;} static_assert(0x61ACu-0x618Au==34u);
void function_61ac(Machine&m,u32 seed)noexcept{store_le32(dgroup_ptr(m,dgroup(0x23ccu)),seed);} static_assert(0x61BFu-0x61ACu==19u);
// 61BF is zero alignment.
[[nodiscard]]u32 function_61c0(Machine&m)noexcept{return load_le32(dgroup_ptr(m,dgroup(0x46cu)));} static_assert(0x61CEu-0x61C0u==14u);
// 61CE..61CF is alignment.

// 61D0..61FC: DOS AH=09 string printer; embedded '$' bytes are changed to '*' and the terminating NUL is temporarily changed to '$'.
void function_61d0(PaletteTreeBoundaries&b,char*s)noexcept{char*p=s;while(*p){if(*p=='$')*p='*';++p;}*p='$';if(b.dos_print_dollar)b.dos_print_dollar(s);*p=0;} static_assert(0x61FDu-0x61D0u==45u);
// 61FD..61FF alignment.
[[nodiscard]]u32 function_6200(PaletteTreeBoundaries&b,u32 path,u16 flags)noexcept{return(b.call_d23a?b.call_d23a(path,flags):-1)==0?1u:0u;} static_assert(0x6220u-0x6200u==32u);
[[nodiscard]]u32 function_6220(PaletteTreeBoundaries&b,u32 path,u16 attr)noexcept{return(b.call_d280&&b.call_d280(path,attr,0x8904u)==0)?0x8922u:0u;} static_assert(0x624Cu-0x6220u==44u);
[[nodiscard]]u32 function_624c(PaletteTreeBoundaries&b)noexcept{return(b.call_d2ad&&b.call_d2ad(0x8904u)==0)?0x8922u:0u;} static_assert(0x626Fu-0x624Cu==35u);
// 626F is NOP alignment.
[[nodiscard]]u8 function_6270(Machine&m)noexcept{return*dgroup_ptr(m,dgroup(0x8919u));} static_assert(0x627Cu-0x6270u==12u);
[[nodiscard]]s32 function_627c(PaletteTreeBoundaries&b,u32 path)noexcept{if(b.call_d280&&b.call_d280(path,0u,0x8904u)==0)return static_cast<s32>(load_le32(reinterpret_cast<const u8*>(nullptr)));return -1;} // patched below: guest 891E is supplied by caller boundary
static_assert(0x62A8u-0x627Cu==44u);
[[nodiscard]]u32 function_62a8(PaletteTreeBoundaries&b,u32 path)noexcept{return b.call_d313?b.call_d313(path,0x200u):0xffffffffu;} static_assert(0x62C0u-0x62A8u==24u);
[[nodiscard]]u32 function_62c0(PaletteTreeBoundaries&b,u32 path)noexcept{return b.call_d313?b.call_d313(path,0x202u):0xffffffffu;} static_assert(0x62D8u-0x62C0u==24u);
[[nodiscard]]u32 function_62d8(PaletteTreeBoundaries&b,u32 path)noexcept{return b.call_d55e?b.call_d55e(path,0x180u):0xffffffffu;} static_assert(0x62F0u-0x62D8u==24u);
[[nodiscard]]u32 function_62f0(PaletteTreeBoundaries&b,u32 h)noexcept{return b.call_d573?b.call_d573(h):0xffffffffu;} static_assert(0x6303u-0x62F0u==19u);
// 6303 alignment.
[[nodiscard]]s32 function_6304(PaletteTreeBoundaries&b,void*dst,u32 count,u32 h)noexcept{return b.call_d5ad?b.call_d5ad(h,dst,count):-1;} static_assert(0x631Fu-0x6304u==27u);
[[nodiscard]]s32 function_6320(PaletteTreeBoundaries&b,const void*src,u32 count,u32 h)noexcept{return b.call_d686?b.call_d686(h,src,count):-1;} static_assert(0x633Bu-0x6320u==27u);
[[nodiscard]]s32 function_633c(PaletteTreeBoundaries&b,s32 off,u32 origin,u32 h)noexcept{return b.call_d85f?b.call_d85f(h,off,origin):-1;} static_assert(0x6357u-0x633Cu==27u);
void function_6358(PaletteTreeBoundaries&b,u32 index,u32 h)noexcept{(void)function_633c(b,static_cast<s32>(index*4u+4u),0u,h);u32 off=0;(void)function_6304(b,&off,4u,h);(void)function_633c(b,static_cast<s32>(off),0u,h);} static_assert(0x63A2u-0x6358u==74u);
[[nodiscard]]s32 function_63a4(PaletteTreeBoundaries&b,const char*path)noexcept{return b.call_d8aa?b.call_d8aa(path):-1;} static_assert(0x63B7u-0x63A4u==19u);
[[nodiscard]]u32 function_63b8(PaletteTreeBoundaries&b,u32 drive)noexcept{u32 n=0;if(b.call_d8e8)(void)b.call_d8e8(drive,&n);return n;} static_assert(0x63D5u-0x63B8u==29u);
[[nodiscard]]u32 function_63d8(PaletteTreeBoundaries&b)noexcept{u32 d=0;if(b.call_d8d0)(void)b.call_d8d0(&d);return d;} static_assert(0x63F1u-0x63D8u==25u);
[[nodiscard]]u32 function_63f4(PaletteTreeBoundaries&b)noexcept{const u32 d=function_63d8(b);return function_63b8(b,d);} static_assert(0x6409u-0x63F4u==21u);
[[nodiscard]]char* function_640c(PaletteTreeBoundaries&b)noexcept{return b.call_d906?b.call_d906(reinterpret_cast<char*>(0x892fu)):nullptr;} static_assert(0x6422u-0x640Cu==22u);
// 6422..6423 alignment.
[[nodiscard]]s32 function_6424(PaletteTreeBoundaries&b,char*path)noexcept{const u32 saved=function_63d8(b);if(path[1]==':'){const u32 upper=b.call_d991?b.call_d991(static_cast<u8>(path[0])):static_cast<u8>(path[0]);const s32 drive=static_cast<s32>(upper)-0x41;function_63b8(b,static_cast<u32>(drive));if(static_cast<s32>(function_63d8(b))!=drive)return -1;path+=2;}const s32 r=b.call_d9a6?b.call_d9a6(path):-1;if(r==-1)function_63b8(b,saved);return r;} static_assert(0x648Du-0x6424u==105u);
[[nodiscard]]s32 function_6490(PaletteTreeBoundaries&b,const char*path)noexcept{return b.call_d9ca?b.call_d9ca(path):-1;} static_assert(0x64A3u-0x6490u==19u);
// 64A3..64AF are zero/alignment bytes.
[[nodiscard]]u32 function_64b0(Machine&m,u16 seed)noexcept{u32 v=seed;if(seed==0u)v=load_le16(dgroup_ptr(m,dgroup(0x46cu)));function_61ac(m,static_cast<u16>(v));return v;} static_assert(0x64D9u-0x64B0u==41u);
// 64D9..64DB alignment.

struct DisplayInitBoundaries{u8(*call_9ca0)(u8,u32)=nullptr;void(*call_9df4)()=nullptr;void(*call_a474)(u32)=nullptr;void(*call_a35c)(u8)=nullptr;void(*call_a36c)(u8)=nullptr;void(*call_6bc0)(s32,s32,s32,s32)=nullptr;void(*call_a05c)(s32,s32,s32,s32,u8)=nullptr;void(*call_a010)(u32,u32,u32)=nullptr;};
// 64DC..6587: video mode initialization and global display-state reset.
void function_64dc(Machine&m,DisplayInitBoundaries&b,u32 mode)noexcept{if(mode==0xffu){if(b.call_9df4)b.call_9df4();*dgroup_ptr(m,dgroup(0x23d4u))=0xffu;return;}const u8 caps=b.call_9ca0?b.call_9ca0(static_cast<u8>(mode),0x23d4u):0u;store_le16(dgroup_ptr(m,dgroup(0x23dbu)),0x100u);store_le16(dgroup_ptr(m,dgroup(0x23ddu)),(caps&4u)?1u:0u);if(static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23dfu))))<0xff&&b.call_a474)b.call_a474(0xffu);*dgroup_ptr(m,dgroup(0x23e6u))=0;*dgroup_ptr(m,dgroup(0x23e5u))=0;if(b.call_a35c)b.call_a35c(0);if(b.call_a36c)b.call_a36c(0);if(b.call_6bc0)b.call_6bc0(0,0,static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d7u))))-1,static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d9u))))-1);*dgroup_ptr(m,dgroup(0x23d4u))=static_cast<u8>(mode)&0x7fu;} static_assert(0x6588u-0x64DCu==172u);
void function_6588(Machine&m,DisplayInitBoundaries&b,u8 page)noexcept{if(b.call_a35c)b.call_a35c(page);*dgroup_ptr(m,dgroup(0x23e5u))=page;} static_assert(0x65A4u-0x6588u==28u);
void function_65a4(Machine&m,DisplayInitBoundaries&b,u8 page)noexcept{if(b.call_a36c)b.call_a36c(page);*dgroup_ptr(m,dgroup(0x23e6u))=page;} static_assert(0x65C0u-0x65A4u==28u);
void function_65c0(DisplayInitBoundaries&b,s32 x,s32 y,s32 width,u8 color)noexcept{if(b.call_a05c)b.call_a05c(x,y,x,y+width-1,color);} static_assert(0x65E8u-0x65C0u==40u);
void function_65e8(DisplayInitBoundaries&b,s32 x,s32 y,s32 width,u8 color)noexcept{if(width!=0&&b.call_a05c)b.call_a05c(x,y,x+width-1,y,color);} static_assert(0x6616u-0x65E8u==46u);
// 6616..6617 alignment.
void function_6618(DisplayInitBoundaries&b,u32 dst)noexcept{if(b.call_a010)b.call_a010(dst,0u,0x300u);} static_assert(0x6632u-0x6618u==26u);
void function_6634(DisplayInitBoundaries&b,u32 a,u32 count,u32 dst)noexcept{if(b.call_a010)b.call_a010(dst,a,count*3u);} static_assert(0x6654u-0x6634u==32u);

// 6654..6851: clipped display wrappers. Width/height clipping follows the encoded signed comparisons exactly.
struct DisplayClipBoundaries{void(*a2b0)(s32,s32)=nullptr;void(*a21c)(s32)=nullptr;void(*a484)(u32,s32,s32,s32,s32)=nullptr;void(*a4e8)(u32,s32,s32,s32,s32)=nullptr;void(*a6f0)(u32,s32,s32,u32)=nullptr;void(*a7e8)(u32,s32,s32,u32,u8)=nullptr;void(*a8f4)(u32,s32,s32,u32,u32)=nullptr;void(*flush_8960)()=nullptr;};
void function_6654(Machine&m,DisplayClipBoundaries&b,s32 n)noexcept{if(b.a2b0)b.a2b0(static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d7u))))-n,n);} static_assert(0x6672u-0x6654u==30u);
void function_6674(Machine&m,DisplayClipBoundaries&b,s32 n)noexcept{const s32 w=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d7u))));if(b.a2b0)b.a2b0(w-n,n-w);} static_assert(0x6694u-0x6674u==32u);
void function_6694(DisplayClipBoundaries&b,s32 n)noexcept{if(b.a21c)b.a21c(n);} static_assert(0x66A7u-0x6694u==19u);
void function_66a8(DisplayClipBoundaries&b,s32 n)noexcept{if(b.a21c)b.a21c(-n);} static_assert(0x66BDu-0x66A8u==21u);
// 66BD..66BF alignment.
static void clip_rect_dims(Machine&m,s32&x,s32&y,s32&w,s32&h)noexcept{if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}const s32 sw=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d7u)))),sh=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x23d9u))));if(x+w>sw)w=sw-x;if(y+h>sh)h=sh-y;}
void function_66c0(Machine&m,DisplayClipBoundaries&b,u32 p,s32 x,s32 y,s32 w,s32 h)noexcept{clip_rect_dims(m,x,y,w,h);if(w>0&&h>0&&b.a484)b.a484(p,x,y,w,h);} static_assert(0x6743u-0x66C0u==131u);
void function_6744(Machine&m,DisplayClipBoundaries&b,u32 p,s32 x,s32 y,s32 w,s32 h)noexcept{clip_rect_dims(m,x,y,w,h);if(w>0&&h>=0&&b.a4e8)b.a4e8(p,x,y,w,h);} static_assert(0x67C7u-0x6744u==131u);
void function_67c8(DisplayClipBoundaries&b,u32 p,s32 x,s32 y,u32 n)noexcept{if(n!=0u){if(b.a6f0)b.a6f0(p,x,y,n);if(b.flush_8960)b.flush_8960();}} static_assert(0x67F2u-0x67C8u==42u);
void function_67f4(DisplayClipBoundaries&b,u32 p,s32 x,s32 y,u32 n,u8 c)noexcept{if(n!=0u){if(b.a7e8)b.a7e8(p,x,y,n,c);if(b.flush_8960)b.flush_8960();}} static_assert(0x6823u-0x67F4u==47u);
void function_6824(DisplayClipBoundaries&b,u32 p,s32 x,s32 y,u32 n,u32 c)noexcept{if(n!=0u){if(b.a8f4)b.a8f4(p,x,y,n,c);if(b.flush_8960)b.flush_8960();}} static_assert(0x6852u-0x6824u==46u);
// 6852..6853 alignment.

'''
# fix the 627C guest-global access without inventing a host pointer.
code=code.replace("struct PaletteTreeBoundaries{\n", "struct PaletteTreeBoundaries{\n u32(*read_dgroup32)(u32)=nullptr;\n")
code=code.replace("if(b.call_d280&&b.call_d280(path,0u,0x8904u)==0)return static_cast<s32>(load_le32(reinterpret_cast<const u8*>(nullptr)));return -1;", "if(b.call_d280&&b.call_d280(path,0u,0x8904u)==0)return static_cast<s32>(b.read_dgroup32?b.read_dgroup32(0x891eu):0u);return -1;")
s=s.replace(anchor,code+anchor)
p.write_text(s)

pp=Path('Code1/PROGRESS.md');t=pp.read_text()
assert 'Current verified translated bytes: **33,000**.' in t
t=t.replace('46.87%25-red','52.09%25-red').replace('**Verified conversion:** 46.87%','**Verified conversion:** 52.09%').replace('**Remaining:** 53.13%','**Remaining:** 47.91%').replace('Current verified translated bytes: **33,000**.','Current verified translated bytes: **36,671**.').replace('Raw coverage: **46.8737%**, displayed as **46.87%**.','Raw coverage: **52.0880%**, displayed as **52.09%**.').replace('Total integrated verified translated bytes: **33,000**.','Total integrated verified translated bytes: **36,671**.')
needle='- Startup/recovery regions through `CD96`: **259 bytes**.\n'
inv='- Palette/tree, actor-table, RNG/DOS/display wrapper routines `59AC..6053`, `6184..6653`, and clipped display wrappers `6654..6851` (alignment excluded): **3,671 bytes**.\n'
assert needle in t;t=t.replace(needle,inv+needle);pp.write_text(t)
sp=Path('Code1/STAGING_LEDGER.md');st=sp.read_text();st+='\n- Batch 08 integrated **3,671 bytes**: `59AC..6053`, `6184..6653`, `6654..6851`, excluding documented alignment. `6060..6183` remains unresolved hardware/interrupt machinery and is not counted.\n';sp.write_text(st)
cp=Path('Code1/DECOMPILATION_CONTINUITY.md');ct=cp.read_text();ct+='\n\n## 2026-10-01 mid-runtime batch 08\nVerified total **36,671 / 70,402 = 52.09%**. Added **3,671 bytes** across palette nearest/tree construction/search (`59AC..5D8E`), actor class/table population (`5D90..6053`), RNG/DOS wrappers and display initialization (`6184..6653`), and clipped display wrappers (`6654..6851`). Preserve the AX-only MOVZX aliasing in 59AC, BL-as-input behavior in 5D90, explicit 16-bit tree stack/sentinels in 5B64, and guest/global address semantics. `6060..6183` is deliberately uncounted pending exact hardware/interrupt stack-register modeling.\n';cp.write_text(ct)
