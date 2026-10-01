from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
anchor='// CC1C..CD96: startup recovery scaffolding.'
assert anchor in s and 'function_1464(' not in s
code=r'''
// 1464..15AD: configure the eleven eight-byte UI slots, materialize optional label 7140,
// run the 10A4 presentation pass, preload visible resources, then dispatch the mode-dependent 07E4 range.
struct MidUiBoundaries{
 void(*call_10a4)(u32)=nullptr; void(*call_7e4)(s32,s32)=nullptr; void(*call_4c)(u32,u32)=nullptr;
 void(*call_72a0)(u32,u32)=nullptr; u32(*call_954c)(u32,u32)=nullptr;
 u32(*call_7868)(u32,u32,u32,u32,u32)=nullptr; void(*call_7830)(u32)=nullptr;
 void(*call_6588)(u32)=nullptr; void(*call_6c60)(s32,s32,s32,s32,u32)=nullptr;
 void(*call_7d80)(s32,s32,u32,u32,u32)=nullptr; void(*call_67c8)(u32,s32,s32,u32)=nullptr;
 s32(*call_43c)(Machine&)=nullptr; void(*call_15b0)(s32,s32,s32,s32)=nullptr;
 void(*call_550c)(u32,s32,s32,s32,s32)=nullptr;
};
void function_1464(Machine&m,MidUiBoundaries&b,u32 arg0,const u32 slots[11],s32 label)noexcept{
 for(u32 i=0;i<11u;++i)store_le32(dgroup_ptr(m,dgroup(0x08f4u+i*8u)),0x15u);
 for(u32 i=0;i<11u;++i)store_le32(dgroup_ptr(m,dgroup(0x08f8u+i*8u)),slots[i]);
 if(label==-1)*dgroup_ptr(m,dgroup(0x7140u))=0;
 else if(label==0x2b){if(b.call_72a0)b.call_72a0(0x7140u,load_le32(dgroup_ptr(m,dgroup(0x04b8u))));}
 else if(b.call_4c)b.call_4c(0x7140u,static_cast<u32>(label));
 if(b.call_10a4)b.call_10a4(arg0);
 for(u32 i=0;i<9u;++i){const s32 v=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f8u+i*8u))));if(v==-1||v<=10)continue;const s32 base=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu)));if(b.call_954c)(void)b.call_954c((v>=base&&v<base+0x2e)?1u:7u,static_cast<u32>(v));}
 const s32 mode=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x04d0u)));if(mode<2){if(b.call_7e4)b.call_7e4(2,4);}else if(mode==6){if(b.call_7e4)b.call_7e4(6,9);}else if(b.call_7e4)b.call_7e4(0,6);
}
static_assert(0x15AEu-0x1464u==330u);
// 15AE..15AF is LEA alignment padding.

// 1A04..1BD0: rebuild the two transient overlay resources. Existing handles 095C/0960 are freed first;
// the two flag bytes choose between record-backed 004C/00EC materialization and direct 6C60/7D80 rendering.
void function_1a04(Machine&m,MidUiBoundaries&b)noexcept{
 for(u32 cell:{0x095cu,0x0960u}){const u32 h=load_le32(dgroup_ptr(m,dgroup(cell)));if(h&&b.call_7830)b.call_7830(h);store_le32(dgroup_ptr(m,dgroup(cell)),0u);}
 if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));
 const u32 first=b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u;
 if(*dgroup_ptr(m,dgroup(0x04c4u))==0){if(b.call_4c)b.call_4c(0x55f8u,0u);}
 else{if(b.call_6c60){b.call_6c60(0,0,0x14,7,0);const s32 x=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(0x680au)))+static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))))+static_cast<s32>(*dgroup_ptr(m,dgroup(0x680bu)))*0x17;b.call_6c60(0,0x15,x,1,0);}if(b.call_7d80)b.call_7d80(2,0,0x30,0x67f8u,0);}
 store_le32(dgroup_ptr(m,dgroup(0x095cu)),b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u);
 if(*dgroup_ptr(m,dgroup(0x04c5u))==0){if(b.call_4c)b.call_4c(0x55f8u,0u);}
 else{if(b.call_6c60){b.call_6c60(0,0,0x14,7,0);const s32 x=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(0x6a52u)))+static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))))+static_cast<s32>(*dgroup_ptr(m,dgroup(0x6a53u)))*0x17;b.call_6c60(0,0x15,x,1,0);}if(b.call_7d80)b.call_7d80(2,0,0x30,0x6a40u,0);}
 store_le32(dgroup_ptr(m,dgroup(0x0960u)),b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u);
 if(b.call_67c8)b.call_67c8(first,0,0,0);if(b.call_7830)b.call_7830(first);
}
static_assert(0x1BD1u-0x1A04u==461u);
// 1BD1..1BD3 is LEA alignment padding.

// 1BD4..1CCF: temporary slot remap used by the selection screen. The loop accepts event 3 immediately;
// event 4 additionally clears the selected actor flag and writes 0x8000 at actor+6808.
void function_1bd4(Machine&m,MidUiBoundaries&b)noexcept{
 store_le32(dgroup_ptr(m,dgroup(0x0904u)),load_le32(dgroup_ptr(m,dgroup(0x0908u))));store_le32(dgroup_ptr(m,dgroup(0x090cu)),load_le32(dgroup_ptr(m,dgroup(0x0910u))));store_le32(dgroup_ptr(m,dgroup(0x0908u)),0x2fu);store_le32(dgroup_ptr(m,dgroup(0x0910u)),0x30u);
 if(b.call_7e4)b.call_7e4(0,6);if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_550c)b.call_550c(*dgroup_ptr(m,dgroup(0x7138u)),0xc8,0,0xc8,0x140);
 for(;;){const s32 e=b.call_43c?b.call_43c(m):0;if(e==3)break;if(e==4){const u32 actor=load_le32(dgroup_ptr(m,dgroup(0x04ccu)));const u32 base=actor*0x248u;store_le16(dgroup_ptr(m,dgroup(base+0x6808u)),0x8000u);*dgroup_ptr(m,dgroup(actor+0x04c4u))=0;store_le32(dgroup_ptr(m,dgroup(0x0940u)),2u);break;}}
 store_le32(dgroup_ptr(m,dgroup(0x0904u)),load_le32(dgroup_ptr(m,dgroup(0x0908u))));store_le32(dgroup_ptr(m,dgroup(0x090cu)),load_le32(dgroup_ptr(m,dgroup(0x0910u))));store_le32(dgroup_ptr(m,dgroup(0x0908u)),1u);store_le32(dgroup_ptr(m,dgroup(0x0910u)),1u);function_1a04(m,b);if(b.call_7e4)b.call_7e4(0,6);
}
static_assert(0x1CD0u-0x1BD4u==252u);

// 1CD0..1DD7: actor-choice loop. 0 exits with actor=-1; 3/4 select actor 0/1. An already-present actor enters 1BD4;
// otherwise 15B0 is called and the two per-actor dwords at 6903/6907 are cleared.
void function_1cd0(Machine&m,MidUiBoundaries&b)noexcept{
 function_1a04(m,b);const u32 setup[11]={0xffffffffu,0xffffffffu,1u,1u,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu};function_1464(m,b,0,setup,(*dgroup_ptr(m,dgroup(0x04c4u))&&*dgroup_ptr(m,dgroup(0x04c5u)))?1:2);
 for(;;){const s32 e=b.call_43c?b.call_43c(m):0;if(e<=0){store_le32(dgroup_ptr(m,dgroup(0x04ccu)),0xffffffffu);break;}if(e==3||e==4){const u32 actor=static_cast<u32>(e-3);store_le32(dgroup_ptr(m,dgroup(0x04ccu)),actor);if(*dgroup_ptr(m,dgroup(0x04c4u+actor))!=0){function_1bd4(m,b);continue;}if(b.call_15b0)b.call_15b0(-1,-1,-1,-1);store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))+1u);const u32 base=actor*0x248u;store_le32(dgroup_ptr(m,dgroup(base+0x6903u)),0u);store_le32(dgroup_ptr(m,dgroup(base+0x6907u)),0u);break;}}
 for(u32 cell:{0x095cu,0x0960u}){const u32 h=load_le32(dgroup_ptr(m,dgroup(cell)));if(h&&b.call_7830)b.call_7830(h);store_le32(dgroup_ptr(m,dgroup(cell)),0u);}
}
static_assert(0x1DD8u-0x1CD0u==264u);

// 1DD8..1E61: choose actor submode (events 3/4) or cancel (event 0); selected byte 680B receives event-3.
void function_1dd8(Machine&m,MidUiBoundaries&b)noexcept{const u32 slots[11]={0xffffffffu,0xffffffffu,0x0cu,0x0du,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,8u,0xffffffffu,0x26u};function_1464(m,b,0,slots,-1);for(;;){const s32 e=b.call_43c?b.call_43c(m):0;if(e<=0){store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))-1u);if(b.call_15b0)b.call_15b0(-1,-1,-1,-1);return;}if(e==3||e==4){const u32 base=load_le32(dgroup_ptr(m,dgroup(0x04ccu)))*0x248u;*dgroup_ptr(m,dgroup(base+0x680bu))=static_cast<u8>(e-3);store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))+1u);if(b.call_15b0)b.call_15b0(e-1,0x114,3,1);return;}}}
static_assert(0x1E62u-0x1DD8u==138u);
// 1E62..1E63 is LEA alignment padding.

// 1E64..1EE0: choose one of five byte values for actor+680C; event 0 cancels and decrements mode.
void function_1e64(Machine&m,MidUiBoundaries&b)noexcept{const u32 slots[11]={0u,1u,2u,3u,4u,5u,0xffffffffu,0xffffffffu,9u,0xffffffffu,0x27u};function_1464(m,b,1,slots,-1);const s32 e=b.call_43c?b.call_43c(m):0;if(e==0){store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))-1u);if(b.call_15b0)b.call_15b0(-1,-1,-1,-1);return;}const u32 base=load_le32(dgroup_ptr(m,dgroup(0x04ccu)))*0x248u;*dgroup_ptr(m,dgroup(base+0x680cu))=static_cast<u8>(e-1);store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))+1u);if(b.call_15b0)b.call_15b0(e-1,0x114,0x38,2);}
static_assert(0x1EE1u-0x1E64u==125u);
// 1EE1..1EE3 is LEA alignment padding.

// 1EE4..1EFE: exact one-byte swap helper.
void function_1ee4(u8*a,u8*b)noexcept{const u8 t=*a;*a=*b;*b=t;} static_assert(0x1EFFu-0x1EE4u==27u);
// 1EFF is NOP alignment; 1F00..1F23 is a nine-entry code-address jump table and is data, not translated code.

'''
s=s.replace(anchor,code+anchor)
p.write_text(s)

# Update progress conservatively: 1,597 newly integrated code bytes.
pp=Path('Code1/PROGRESS.md'); t=pp.read_text(); old='17,960'; new='19,557'; assert old in t
t=t.replace('25.51%25-red','27.78%25-red').replace('**Verified conversion:** 25.51%','**Verified conversion:** 27.78%').replace('**Remaining:** 74.49%','**Remaining:** 72.22%').replace('**17,960**','**19,557**').replace('**25.5106%**','**27.7790%**').replace('Total integrated verified translated bytes: **17,960**.','Total integrated verified translated bytes: **19,557**.')
inv='- Mid-game UI configuration/selection routines `1464..15AD`, `1A04..1EE0`, and byte-swap helper `1EE4..1EFE`: **1,597 bytes**.\n'
needle='- Startup/recovery regions through `CD96`: **259 bytes**.\n'; assert needle in t;t=t.replace(needle,inv+needle);pp.write_text(t)

sp=Path('Code1/STAGING_LEDGER.md'); st=sp.read_text();st+='\n- Integrated directly from authoritative disassembly: `1464..15AD`, `1A04..1EE0`, `1EE4..1EFE` = **1,597 bytes**. Alignment at `15AE..15AF`, `1BD1..1BD3`, `1E62..1E63`, `1EE1..1EE3`, `1EFF` excluded; `1F00..1F23` classified as jump-table data.\n';sp.write_text(st)
cp=Path('Code1/DECOMPILATION_CONTINUITY.md'); ct=cp.read_text();ct+='\n\n## 2026-10-01 mid-UI batch 03\nVerified total is now **19,557 / 70,402 = 27.78%**. Newly integrated: `1464..15AD`, `1A04..1EE0`, `1EE4..1EFE` (1,597 bytes). `1F00..1F23` is the nine-entry CS jump table used by the following routine and must remain data. Continue independently at `07E4..10A1`, `10A4..1461`, `15B0..1A00`, or from the function beginning at `1F24`; do not count alignment bytes.\n';cp.write_text(ct)
