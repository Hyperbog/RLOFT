from pathlib import Path
import re

p=Path('Code1/Code1.cpp')
s=p.read_text()

def replace_func(name,end_assert,new):
    global s
    pat=re.compile(r'(?:\[\[nodiscard\]\]\s*)?(?:s32|u32|void|static void)\s+'+re.escape(name)+r'\([^)]*\)noexcept\{.*?'+re.escape(end_assert)+r';',re.S)
    s2,n=pat.subn(new,s,1)
    assert n==1,(name,n)
    s=s2

replace_func('function_be70','static_assert(0xBF35u-0xBE70u==197u)',r'''void function_be70(Machine&m,LowContinuation14Boundary&b,u32 palette)noexcept{u32 strength=lc14r8(b,load_le32(dgroup_ptr(m,dgroup(0x2aa0u)))+0x6cu);if(strength>0x3fu)strength=0x3fu;if(strength==0u)return;for(u32 i=0;i<0x100u;++i){const u32 p=palette+i*3u;u32 mx=0;for(u32 c=0;c<3u;++c){const u8 v=lc14r8(b,p+c);if(v>mx)mx=v;}const s32 scaled=static_cast<s32>(mx)+(static_cast<s32>(0x3f-mx)*static_cast<s32>(strength)+0x1f)/0x3f;if(scaled==0)continue;for(u32 c=0;c<3u;++c){const u8 v=lc14r8(b,p+c);if(v==0u)continue;if(mx==0u){if(b.divide_error)b.divide_error();return;}lc14w8(b,p+c,static_cast<u8>((static_cast<s32>(v)*scaled+static_cast<s32>(mx)/2)/static_cast<s32>(mx)));}}}static_assert(0xBF35u-0xBE70u==197u);''')

replace_func('function_bf38','static_assert(0xBFDFu-0xBF38u==167u)',r'''void function_bf38(LowContinuation14Boundary&b,u32 source,u32 first,u32 last,u8 r,u8 g,u8 bl)noexcept{const u8 rgb[3]={r,g,bl};const u32 count=last-first+1u;function_c07c(b,0x2c48u,0u,source,0u,0x100u);for(s32 step=7;step>=0;--step){function_c110(b,0x2c48u,first,count,step,3u,rgb);if(b.call_6618)b.call_6618(0x2c48u);function_c07c(b,0x2c48u,first,source,first,count);}if(b.call_9ab4)b.call_9ab4();}static_assert(0xBFDFu-0xBF38u==167u);''')

replace_func('function_bfe0','static_assert(0xC079u-0xBFE0u==153u)',r'''void function_bfe0(LowContinuation14Boundary&b,u32 source,u32 first,u32 last,u8 r,u8 g,u8 bl)noexcept{const u8 rgb[3]={r,g,bl};const u32 count=last-first+1u;function_c07c(b,0x2c48u,0u,source,0u,0x100u);for(s32 step=1;step<=8;++step){function_c110(b,0x2c48u,first,count,step,3u,rgb);if(b.call_6618)b.call_6618(0x2c48u);function_c07c(b,0x2c48u,first,source,first,count);}if(b.call_9ac4)b.call_9ac4();}static_assert(0xC079u-0xBFE0u==153u);''')

replace_func('function_c0b8','static_assert(0xC10Fu-0xC0B8u==87u)',r'''void function_c0b8(LowContinuation14Boundary&b,u32 base,u32 index,u8 r,u8 g,u8 bl,u32 count)noexcept{u32 p=base+index*3u;for(u32 i=0;static_cast<s32>(i)<static_cast<s32>(count);++i){lc14w8(b,p++,r);lc14w8(b,p++,g);lc14w8(b,p++,bl);}}static_assert(0xC10Fu-0xC0B8u==87u);''')

replace_func('function_c110','static_assert(0xC17Bu-0xC110u==107u)',r'''static void function_c110(LowContinuation14Boundary&b,u32 palette,u32 start,u32 count,s32 factor,u8 shift,const u8 target[3])noexcept{u32 p=palette+start*3u;for(u32 n=0;static_cast<s32>(n)<static_cast<s32>(count);++n)for(u32 c=0;c<3u;++c,++p){const s32 base=target[c],cur=lc14r8(b,p);const u32 product=static_cast<u32>(cur-base)*static_cast<u32>(factor),sh=static_cast<u32>(shift)&31u;u32 shifted=product>>sh;if(sh!=0u&&(product&0x80000000u)!=0u)shifted|=(0xffffffffu<<(32u-sh));lc14w8(b,p,static_cast<u8>(static_cast<u32>(base)+shifted));}}static_assert(0xC17Bu-0xC110u==107u);''')

replace_func('function_c1b0','static_assert(0xC227u-0xC1B0u==119u)',r'''void function_c1b0(LowContinuation14Boundary&b,u32 base,u32 first,u32 offset,u32 run)noexcept{if(static_cast<s32>(run)<=1)return;u32 p=base+(first+offset)*3u;const u32 bytes=run*3u-3u;const u8 save[3]={lc14r8(b,p),lc14r8(b,p+1u),lc14r8(b,p+2u)};if(b.copy_d9ee)b.copy_d9ee(p,p+3u,bytes);else for(u32 i=0;i<bytes;++i)lc14w8(b,p+i,lc14r8(b,p+3u+i));for(u32 i=0;i<3u;++i)lc14w8(b,p+bytes+i,save[i]);}static_assert(0xC227u-0xC1B0u==119u);''')

replace_func('function_c2d0','static_assert(0xC49Du-0xC2D0u==461u)',r'''void function_c2d0(Machine&m,LowContinuation14Boundary&b,u32 path,u32 p2,u32 p3,u32 p4,u32 p5)noexcept{
 if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))!=0u)return;
 const u32 area=b.call_6f10?b.call_6f10(0x612b0u,0x2f6cu,0x2f70u):0u;store_le32(dgroup_ptr(m,dgroup(0x2f68u)),area);if(area==0u)return;
 const u32 h=b.call_62a8?b.call_62a8(path):0xffffffffu;if(h==0xffffffffu){if(b.call_6f74)(void)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2f70u))));return;}
 const u32 alloc_size=b.call_627c?b.call_627c(path):0u;const u32 tmp=b.call_71c8?b.call_71c8(alloc_size):0u;if(tmp==0u){if(b.call_6f74)(void)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2f70u))));return;}
 const u32 read_size=b.call_627c?b.call_627c(path):0u;if(b.call_6304)b.call_6304(tmp,read_size,h);if(b.call_62f0)b.call_62f0(h);
 const u32 image=b.call_f8c4?b.call_f8c4(tmp,5u,0u):0u;if(image==0u){if(b.call_6f74)(void)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2f70u))));if(b.call_7240)b.call_7240(tmp);return;}if(b.call_7240)b.call_7240(tmp);if(b.call_e680)b.call_e680();
 const u32 sys=b.call_ea0b?b.call_ea0b(image):0xffffffffu;store_le32(dgroup_ptr(m,dgroup(0x2f74u)),sys);if(sys==0xffffffffu){if(b.call_6f74)(void)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2f70u))));if(b.call_e6ea)b.call_e6ea(0u);return;}
 const u32 info=b.call_eac8?b.call_eac8(sys):0u;if(lc14r32(b,info+4u)!=2u){if(b.call_e6ea)b.call_e6ea(0u);if(b.call_6f74)(void)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2f70u))));return;}
 if((b.call_eae7?b.call_eae7(load_le32(dgroup_ptr(m,dgroup(0x2f74u))),p2,p3,p4,p5):0u)==0u){if(b.call_e6ea)b.call_e6ea(0u);if(b.call_6f74)(void)b.call_6f74(load_le16(dgroup_ptr(m,dgroup(0x2f70u))));return;}
 if(b.call_eaf1)b.call_eaf1(load_le32(dgroup_ptr(m,dgroup(0x2f74u))),p2,p3,p4,p5);store_le32(dgroup_ptr(m,dgroup(0x2f48u)),1u);
}static_assert(0xC49Du-0xC2D0u==461u);''')

replace_func('function_c4e0','static_assert(0xC57Bu-0xC4E0u==155u)',r'''void function_c4e0(Machine&m,LowContinuation14Boundary&b,u32 id)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))==0u||*dgroup_ptr(m,dgroup(0x2f78u))==0u)return;const u32 rec=b.call_954c?b.call_954c(id,load_le32(dgroup_ptr(m,dgroup(0x2f4cu)))):0u;const u32 len=lc14r32(b,rec),src=rec+4u;function_c57c(m,b);if(b.copy_d9ee)b.copy_d9ee(load_le32(dgroup_ptr(m,dgroup(0x2f68u))),src,len);if(b.call_ec25)b.call_ec25(load_le32(dgroup_ptr(m,dgroup(0x2f74u))),load_le32(dgroup_ptr(m,dgroup(0x2f68u))),static_cast<u32>(m.cpu.ds),0xffffffffu);if(b.call_ec1b)b.call_ec1b(load_le32(dgroup_ptr(m,dgroup(0x2f74u))),load_le32(dgroup_ptr(m,dgroup(0x2f68u))),static_cast<u32>(m.cpu.ds),load_le32(dgroup_ptr(m,dgroup(0x2f6cu))),0xffffffffu);if(b.call_ec39)b.call_ec39(load_le32(dgroup_ptr(m,dgroup(0x2f74u))));}static_assert(0xC57Bu-0xC4E0u==155u);''')

replace_func('function_c57c','static_assert(0xC5B0u-0xC57Cu==52u)',r'''void function_c57c(Machine&m,LowContinuation14Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2f48u)))==0u)return;if((b.call_ec2f?b.call_ec2f(load_le32(dgroup_ptr(m,dgroup(0x2f74u)))):0u)==2u&&b.call_ec43)b.call_ec43(load_le32(dgroup_ptr(m,dgroup(0x2f74u))));}static_assert(0xC5B0u-0xC57Cu==52u);''')

p.write_text(s)

a=Path('Code1/FINAL_BINARY_AUDIT.md')
t=a.read_text()
head='## Semantic audit checkpoint 10g — `BE70..C79E`'
if head not in t:
    t += r'''

## Semantic audit checkpoint 10g — `BE70..C79E`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the palette-transition and second external-resource subsystem through `C79E`. `BE70` now reads each component once during its max scan, matching the encoded load/compare/update sequence. More importantly, `BF38` and `BFE0` no longer misinterpret their third dword argument as a ready-made count: the machine code converts the inclusive final palette index to `count = last - first + 1` before every fade step and source-segment restore.

The helper loops were hardened to the actual signed branches. `C0B8` and `C110` use signed `JL` termination, so zero/high-bit counts are zero-iteration rather than enormous unsigned loops; `C110` also reproduces 32-bit IMUL wrap and x86's masked arithmetic-SAR count. `C1B0` uses the encoded signed `run <= 1` gate and routes its overlapping middle move through the existing `D9EE` copy boundary.

`C2D0` now issues the two distinct `627C(path)` calls present in the binary—one before allocation and a second immediately before the file read—instead of caching one size result. Its failure cleanup order is path-accurate: an `EA0B == FFFFFFFFh` result frees selector `2F70h` before calling `E6EA(0)`, whereas the type-check and `EAE7` failures call `E6EA(0)` first and then free the selector. `EAE7/EAF1` reload `2F74h` at their encoded call sites. `C4E0` and `C57C` likewise re-read `2F74h`/`2F68h` after opaque subsystem calls instead of retaining stale cached values. The remaining routines `C07C`, `C17C`, `C228`, `C254`, `C4A0`, `C5B0`, `C608`, `C62C`, `C650`, `C660`, `C690`, `C6F0`, and `C6F8` were rechecked directly and required no additional semantic change. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `C79E`; `C79F` is a zero alignment byte and the next code begins at `C7A0`.
'''
    a.write_text(t)
