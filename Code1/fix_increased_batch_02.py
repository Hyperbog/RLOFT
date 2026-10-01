from pathlib import Path
p=Path('Code1/Code1.cpp');s=p.read_text()
# Repair the two missing close-parens in 035C.
s=s.replace('const s32 left=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(q)));','const s32 left=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(q))));')
s=s.replace('const s32 top=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(q+4u)));','const s32 top=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(q+4u))));')
s=s.replace('const s32 top=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08a0u)));','const s32 top=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08a0u))));')
# Extend the UI boundary with the exact seven-argument 00EC call used by 064C.
s=s.replace('void(*call_38a4)(s32,s32)=nullptr;};','void(*call_38a4)(s32,s32)=nullptr;void(*call_00ec)(s32,s32,s32,s32,u32,u32,u32)=nullptr;};')
start=s.index('void function_04b0(');end=s.index(' static_assert(0x649u-0x4B0u==409u);',start)+len(' static_assert(0x649u-0x4B0u==409u);')
new=r'''void function_04b0(Machine&m,EarlyUiBoundaries&b,s32 stage)noexcept{
 const s32 actor=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x04ccu))));const u32 base=static_cast<u32>(actor)*0x248u;
 if(stage>=1&&b.call_6eb0)b.call_6eb0(0,0x114,3,static_cast<s32>(*dgroup_ptr(m,dgroup(base+0x680bu)))+0x0c,7,0x5ff8u);
 if(stage>=2&&b.call_6eb0)b.call_6eb0(0,0x114,0x38,*dgroup_ptr(m,dgroup(base+0x680cu)),7,0x5ff8u);
 if(stage>=3)for(s32 i=2;i>=0;--i){const std::int8_t v=static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(base+0x680du+static_cast<u32>(i))));if(v!=-1&&b.call_6eb0)b.call_6eb0(0,0x114-i*0x14,0x6d-i,static_cast<s32>(v)+6,7,0x5ff8u);}
 if(stage>=4&&b.call_6eb0)b.call_6eb0(0,0x114,0xa2,static_cast<s32>(*dgroup_ptr(m,dgroup(base+0x6812u)))+0x0e,7,0x5ff8u);
 if(stage>=5){if(b.call_6c60)b.call_6c60(0,0x114,0xd7,0x14,7);const u32 v=static_cast<u32>(*dgroup_ptr(m,dgroup(base+0x680bu)))*0x17u+load_le32(dgroup_ptr(m,dgroup(0x713cu)))+static_cast<s32>(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(base+0x680au))));if(b.call_6c60)b.call_6c60(0,0x129,0xd7,static_cast<s32>(v),1);if(*dgroup_ptr(m,dgroup(static_cast<u32>(actor)+0x04c4u))!=0u&&b.call_7d80)b.call_7d80(0x116,0xd7,0x30u,static_cast<u32>(actor)*0x248u+0x67f8u,0u);}
 if(stage>=6&&b.call_38a4)b.call_38a4(0x114,0x10c);
} static_assert(0x649u-0x4B0u==409u);'''
s=s[:start]+new+s[end:]
start=s.index('void function_064c(');end=s.index(' static_assert(0x7E2u-0x64Cu==406u);',start)+len(' static_assert(0x7E2u-0x64Cu==406u);')
new=r'''void function_064c(Machine&m,EarlyUiBoundaries&b,EarlyGameBoundaries&io)noexcept{
 for(s32 i=0;i<9;++i){const u32 q=0x08f8u+static_cast<u32>(i)*8u;const s32 state=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(q))));if(state==-1)continue;const s32 x=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+static_cast<u32>(i)*8u))));const s32 y=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+static_cast<u32>(i)*8u))));
  if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&state==1){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0954u+static_cast<u32>(i)*4u))));continue;}
  if(i==7&&state!=0x14){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0958u))));continue;}
  const s32 pivot=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))));if(state>=pivot&&state<pivot+0x2e){if(b.call_6c60)b.call_6c60(0,x,y,0x14,7);if(b.call_6eb0)b.call_6eb0(0,x+0x15,y,state,1,0x5af8u);}else if(b.call_6eb0)b.call_6eb0(0,x,y,state,7,0x5ff8u);
 }
 u8 tmp[256]{};for(const u32 slot:{0x0940u,0x0948u}){const u32 rec=load_le32(dgroup_ptr(m,dgroup(slot)));if(rec==0xffffffffu)continue;function_004c(m,io,tmp,rec);if(b.call_00ec)b.call_00ec(0x114,slot==0x0940u?0x10c:0xd7,0x7a,0x30,0x55f8u,0u,0x5ff8u);}
} static_assert(0x7E2u-0x64Cu==406u);'''
s=s[:start]+new+s[end:]
p.write_text(s)

# The renderer ABI is now reconciled directly from push order at every call site, so add its 815 bytes to coverage.
q=Path('Code1/PROGRESS.md');t=q.read_text();t=t.replace('24.35%25-red','25.51%25-red').replace('**Verified conversion:** 24.35%','**Verified conversion:** 25.51%').replace('**Remaining:** 75.65%','**Remaining:** 74.49%').replace('Current verified translated bytes: **17,145**.','Current verified translated bytes: **17,960**.').replace('Raw coverage: **24.3530%**, displayed as **24.35%**.','Raw coverage: **25.5106%**, displayed as **25.51%**.').replace('Total integrated verified translated bytes: **17,145**.','Total integrated verified translated bytes: **17,960**.')
t=t.replace('- Early-game hit-test/input routines `035C..043A` and `043C..04AF`: **339 bytes**.','- Early-game hit-test/input routines `035C..043A` and `043C..04AF`: **339 bytes**.\n- Early-game staged UI renderers `04B0..0648` and `064C..07E1`: **815 bytes**.')
q.write_text(t)
r=Path('Code1/STAGING_LEDGER.md');u=r.read_text().replace('- `04B0..0648` and `064C..07E1` have source-visible translations for continued ABI reconciliation but are deliberately **not counted** in verified coverage yet.','- `04B0..0648` and `064C..07E1` renderer push-order contracts were reconciled and their 815 bytes are integrated/counted.');r.write_text(u)
c=Path('Code1/DECOMPILATION_CONTINUITY.md');v=c.read_text().replace('Verified total after this fold: **17,145 / 70,402 = 24.3530% (24.35%)**.','Verified total after this fold: **17,960 / 70,402 = 25.5106% (25.51%)**.').replace('Newly counted this fold: 884 bytes:','Newly counted this fold: 1,699 bytes:').replace('- Early renderer translations `04B0..0648` and `064C..07E1` are present but uncounted until external renderer ABI argument contracts are independently reconciled.','- Early renderer translations `04B0..0648` and `064C..07E1` are integrated/countable after direct push-order reconciliation of 6EB0/6C60/7D80/67C8/00EC call sites.');c.write_text(v)
