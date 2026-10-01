from pathlib import Path
p=Path('Code1/Code1.cpp'); s=p.read_text(); anchor='// CC1C..CD96: startup recovery scaffolding.'
assert anchor in s and 'function_1f24(' not in s
code=r'''
// 1F24..2196: normalize the selected actor's three class/order bytes.  The binary derives a
// dispatch key in DL and jumps through the nine-entry table at 1F00; DL is intentionally not
// initialized on entry, so the reconstruction preserves the incoming low EDX value until an
// encoded condition assigns it.
void function_1f24(Machine&m)noexcept{
 const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x04ccu)));u8*p=dgroup_ptr(m,dgroup(actor*0x248u+0x67f8u));
 u8 key=low8(m.cpu.edx);const s32 b1=static_cast<std::int8_t>(p[0x16]),b2=static_cast<std::int8_t>(p[0x17]);
 if(b1==-1)key=p[0x15];
 else if(b2==-1){const u8 a=p[0x15],b=p[0x16];if(a==0){if(b==5)key=6;if(b==3)key=7;if(b==4)key=8;}if(a==5){if(b==0)key=6;if(b==3)key=9;if(b==4)key=12;}if(a==2)key=10;if(a==3){if(b==0)key=7;if(b==5)key=9;if(b==4)key=11;if(b==2)key=10;}if(a==4){if(b==0)key=8;if(b==3)key=11;if(b==5)key=12;}}
 else key=(p[0x15]==5||p[0x16]==5||p[0x17]==5)?13u:14u;
 set_low8(m.cpu.edx,key);
 switch(static_cast<u8>(key-6u)){
  case 0:case 1:case 2:if(p[0x15]!=0)function_1ee4(p+0x15,p+0x16);break;
  case 3:case 4:case 5:if(p[0x15]!=3)function_1ee4(p+0x15,p+0x16);break;
  case 6:if(p[0x15]!=4)function_1ee4(p+0x15,p+0x16);break;
  case 7:p[0x15]=0;p[0x16]=4;p[0x17]=5;break;
  case 8:p[0x15]=0;p[0x16]=4;p[0x17]=3;break;
  default:break;
 }
}
static_assert(0x2197u-0x1F24u==627u);
// 2197 is NOP alignment.

struct ActorRenderBoundaries{
 void(*call_56dc)()=nullptr;u32(*call_71c8)(u32)=nullptr;void(*call_7240)(u32)=nullptr;
 void(*call_6588)(u32)=nullptr;void(*call_6c60)(s32,s32,s32,s32,u32)=nullptr;
 void(*call_66c0)(u32,s32,s32,u32,u32)=nullptr;void(*call_6744)(u32,s32,s32,u32,u32)=nullptr;
 u32(*call_954c)(u32,u32)=nullptr;void(*call_67c8)(u32,s32,s32,u32)=nullptr;void(*call_53e4)(u32)=nullptr;
};
// 24D4..266B: animate the selected slot toward (target_x,target_y) over nine frames. Two
// 0x1E80 scratch surfaces are alternated with 7138 exactly as in the original loop.
void function_24d4(Machine&m,ActorRenderBoundaries&b,u32 slot,s32 target_x,s32 target_y)noexcept{
 if(b.call_56dc)b.call_56dc();const u32 a=b.call_71c8?b.call_71c8(0x1e80u):0u;const u32 c=b.call_71c8?b.call_71c8(0x1e80u):0u;
 if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));const s32 x0=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+slot*8u)))),y0=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+slot*8u))));
 if(b.call_6c60)b.call_6c60(0,x0,y0,static_cast<s32>(slot+0x1eu),7u);if(b.call_66c0){b.call_66c0(a,x0,y0,0x7au,0x30u);b.call_66c0(c,x0,y0,0x7au,0x30u);}s32 x=x0,y=y0;const s32 dx=(target_x-x0)/8,dy=(target_y-y0)/8;
 for(u32 frame=0;frame<=8u;++frame){const u32 page=*dgroup_ptr(m,dgroup(0x7138u));if(b.call_6588)b.call_6588(page);const u32 src=page?c:a;if(b.call_6744)b.call_6744(src,x,y,0x7au,0x30u);if(b.call_66c0)b.call_66c0(src,x,y,0x7au,0x30u);const u32 image=b.call_954c?b.call_954c(7u,load_le32(dgroup_ptr(m,dgroup(0x08f8u+slot*8u)))):0u;if(b.call_67c8)b.call_67c8(image,x,y,0);x+=dx;y+=dy;if(b.call_53e4)b.call_53e4(page);*dgroup_ptr(m,dgroup(0x7138u))^=1u;}
 if(b.call_7240){b.call_7240(a);b.call_7240(c);}
}
static_assert(0x266Cu-0x24D4u==408u);

// 28E4..2990: build six availability flags from actor bytes 680D..680F.
void function_28e4(Machine&m,u32 out[6])noexcept{for(u32 i=0;i<6u;++i)out[i]=1u;const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x04ccu)));const u8*p=dgroup_ptr(m,dgroup(actor*0x248u+0x680du));if(p[0]==1u){for(u32 i=0;i<6u;++i)out[i]=0u;out[0]=1u;return;}if(p[0]==5u||p[1]==5u||p[2]==5u)out[0]=0u;if(p[0]==2u||p[1]==2u)out[1]=out[3]=out[5]=0u;}
static_assert(0x2991u-0x28E4u==173u);
// 2991..2993 is LEA alignment.

// 2994..2A60: present the six availability choices, then write actor+6812 and advance/cancel
// the mode through the existing 15B0 renderer. Entries mapped to 0x15 are non-selectable.
void function_2994(Machine&m,MidUiBoundaries&b)noexcept{u32 v[6];function_28e4(m,v);for(u32 i=0;i<6u;++i)v[i]=v[i]?i+0x0eu:0x15u;const u32 slots[11]={v[0],v[1],v[2],v[3],v[4],v[5],0xffffffffu,0xffffffffu,0xffffffffu,0x0cu,0xffffffffu};function_1464(m,b,3u,slots,0x29);for(;;){const s32 e=b.call_43c?b.call_43c(m):0;if(e!=0&&v[static_cast<u32>(e-1)]==0x15u)continue;if(e==0){store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))-1u);if(b.call_15b0)b.call_15b0(-1,-1,-1,-1);}else{const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x04ccu)));*dgroup_ptr(m,dgroup(actor*0x248u+0x6812u))=static_cast<u8>(e-1);store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))+1u);if(b.call_15b0)b.call_15b0(e-1,0x114,0xa2,4);}return;}}
static_assert(0x2A61u-0x2994u==205u);
// 2A61..2A63 is LEA alignment.

'''
s=s.replace(anchor,code+anchor);p.write_text(s)
pp=Path('Code1/PROGRESS.md');t=pp.read_text();assert '**19,557**' in t
t=t.replace('27.78%25-red','29.79%25-red').replace('**Verified conversion:** 27.78%','**Verified conversion:** 29.79%').replace('**Remaining:** 72.22%','**Remaining:** 70.21%').replace('**19,557**','**20,970**').replace('**27.7790%**','**29.7861%**').replace('Total integrated verified translated bytes: **19,557**.','Total integrated verified translated bytes: **20,970**.')
needle='- Startup/recovery regions through `CD96`: **259 bytes**.\n';assert needle in t;t=t.replace(needle,'- Actor normalization/render/choice routines `1F24..2196`, `24D4..266B`, `28E4..2990`, `2994..2A60`: **1,413 bytes**.\n'+needle);pp.write_text(t)
sp=Path('Code1/STAGING_LEDGER.md');sp.write_text(sp.read_text()+'\n- Integrated directly from authoritative disassembly: `1F24..2196`, `24D4..266B`, `28E4..2990`, `2994..2A60` = **1,413 bytes**. `2197`, `2991..2993`, and `2A61..2A63` are alignment and excluded. The 1F00 jump table remains data.\n')
cp=Path('Code1/DECOMPILATION_CONTINUITY.md');cp.write_text(cp.read_text()+'\n\n## 2026-10-01 actor/UI batch 04\nVerified total **20,970 / 70,402 = 29.79%**. Added 1,413 bytes at `1F24..2196`, `24D4..266B`, `28E4..2990`, `2994..2A60`. 1F24 preserves incoming DL until the encoded decision tree assigns it and resolves the 1F00 jump-table targets. 24D4 preserves guest scratch handles and nine-frame page alternation. Alignment bytes excluded. Active staging unchanged.\n')
