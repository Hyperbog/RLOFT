from pathlib import Path
import re

p=Path('Code1/Code1.cpp')
s=p.read_text()

def replace_func(name, end_assert, new):
    global s
    pat=re.compile(r'(?:\[\[nodiscard\]\]\s*)?(?:s32|u32|void)\s+'+re.escape(name)+r'\([^)]*\)noexcept\{.*?'+re.escape(end_assert)+r';', re.S)
    s2,n=pat.subn(new,s,1)
    assert n==1,(name,n)
    s=s2

replace_func('function_b5e4','static_assert(0xB6BEu-0xB5E4u==218u)',r'''void function_b5e4(Machine&m,LowGraphics13Boundary&b,u32 slot)noexcept{
 if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||*dgroup_ptr(m,dgroup(0x2c28u+slot))!=0u)return;
 const u32 h=b.call_ec93?b.call_ec93(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2b9cu))),slot,load_le32(dgroup_ptr(m,dgroup(0x2ba8u+slot*4u))),0u):0xffffffffu;
 store_le32(dgroup_ptr(m,dgroup(0x2c08u+slot*4u)),h);if(h==0xffffffffu)return;
 if(*dgroup_ptr(m,dgroup(0x2b94u))!=0u&&b.call_ecbb){for(;;){const u16 ax=b.call_ecbb(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c08u+slot*4u))));if(ax==0xffffu)break;const u32 high=static_cast<u32>(ax>>8),low=static_cast<u32>(ax&0xffu);const u32 rec=function_b4d4(m,b,static_cast<u16>(high),static_cast<u16>(low));if(rec==0u)return;if(b.call_ecc5)b.call_ecc5(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),high,low,rec);}}
 *dgroup_ptr(m,dgroup(0x2c28u+slot))=1u;
}static_assert(0xB6BEu-0xB5E4u==218u);''')

replace_func('function_b6ec','static_assert(0xB74Cu-0xB6ECu==96u)',r'''void function_b6ec(Machine&m,LowGraphics13Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u)return;for(u32 i=0;i<8u;++i)if(*dgroup_ptr(m,dgroup(0x2c28u+i))!=0u){const u32 st=b.call_ed0b?b.call_ed0b(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c08u+i*4u)))):0u;if((st==0u||st==2u)&&b.call_ec9d)b.call_ec9d(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c08u+i*4u))));*dgroup_ptr(m,dgroup(0x2c28u+i))=0u;}}static_assert(0xB74Cu-0xB6ECu==96u);''')

replace_func('function_b76c','static_assert(0xB821u-0xB76Cu==181u)',r'''void function_b76c(Machine&m,LowGraphics13Boundary&b,u32 slot)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||*dgroup_ptr(m,dgroup(0x2c44u))==0u||slot==load_le32(dgroup_ptr(m,dgroup(0x2c38u))))return;function_ba98(m,b);function_b5e4(m,b,slot);const u32 h=load_le32(dgroup_ptr(m,dgroup(0x2c08u+slot*4u)));store_le32(dgroup_ptr(m,dgroup(0x2c30u)),h);store_le32(dgroup_ptr(m,dgroup(0x2c38u)),slot);if(h==0xffffffffu||slot==0xffffffffu)return;const u32 rate=static_cast<u32>(function_b598(m,b));if(b.call_ed29)b.call_ed29(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),rate,0u);if(b.call_ed33)b.call_ed33(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),load_le32(dgroup_ptr(m,dgroup(0x2c40u))),0u);if(b.call_eced)b.call_eced(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))));}static_assert(0xB821u-0xB76Cu==181u);''')

replace_func('function_b824','static_assert(0xB8F3u-0xB824u==207u)',r'''void function_b824(Machine&m,LowGraphics13Boundary&b,u32 slot,u32 value)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||*dgroup_ptr(m,dgroup(0x2c44u))==0u||slot==load_le32(dgroup_ptr(m,dgroup(0x2c38u))))return;function_ba98(m,b);function_b5e4(m,b,slot);const u32 h=load_le32(dgroup_ptr(m,dgroup(0x2c08u+slot*4u)));store_le32(dgroup_ptr(m,dgroup(0x2c30u)),h);store_le32(dgroup_ptr(m,dgroup(0x2c38u)),slot);if(h==0xffffffffu||slot==0xffffffffu)return;if(b.call_ed33)b.call_ed33(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),load_le32(dgroup_ptr(m,dgroup(0x2c40u))),0u);if(b.call_eced)b.call_eced(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))));if(b.call_ed29)b.call_ed29(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),0u,0u);const u32 rate=static_cast<u32>(function_b598(m,b));if(b.call_ed29)b.call_ed29(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),rate,value);}static_assert(0xB8F3u-0xB824u==207u);''')

replace_func('function_b8f4','static_assert(0xBA01u-0xB8F4u==269u)',r'''void function_b8f4(Machine&m,LowGraphics13Boundary&b,u32 slot,u32 value)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||*dgroup_ptr(m,dgroup(0x2c44u))==0u||slot==load_le32(dgroup_ptr(m,dgroup(0x2c38u))))return;function_ba98(m,b);const u32 h=load_le32(dgroup_ptr(m,dgroup(0x2c08u+slot*4u)));store_le32(dgroup_ptr(m,dgroup(0x2c30u)),h);store_le32(dgroup_ptr(m,dgroup(0x2c38u)),slot);if(h==0xffffffffu||slot==0xffffffffu)return;if(*dgroup_ptr(m,dgroup(0x2c28u+slot))==0u){function_b824(m,b,slot,value);return;}const u32 rate=static_cast<u32>(function_b598(m,b));if(b.call_ed29)b.call_ed29(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),rate,0u);if(b.call_ed01)b.call_ed01(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))));if(value!=0u){if(b.call_ed29)b.call_ed29(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),0u,0u);const u32 next_rate=static_cast<u32>(function_b598(m,b));if(b.call_ed29)b.call_ed29(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),next_rate,value);}else if(b.call_ed33)b.call_ed33(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),load_le32(dgroup_ptr(m,dgroup(0x2c40u))),0u);}static_assert(0xBA01u-0xB8F4u==269u);''')

replace_func('function_ba98','static_assert(0xBB80u-0xBA98u==232u)',r'''void function_ba98(Machine&m,LowGraphics13Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||load_le32(dgroup_ptr(m,dgroup(0x2c38u)))==0xffffffffu)return;u32 st=b.call_ed0b?b.call_ed0b(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u)))):0u;if(st!=0u&&st!=2u&&b.call_ecf7)b.call_ecf7(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))));function_b5e4(m,b,0u);const u32 h0=load_le32(dgroup_ptr(m,dgroup(0x2c08u)));store_le32(dgroup_ptr(m,dgroup(0x2c30u)),h0);store_le32(dgroup_ptr(m,dgroup(0x2c38u)),0u);if(h0!=0xffffffffu){if(b.call_ed29)b.call_ed29(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),0u,0u);if(b.call_ed33)b.call_ed33(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),load_le32(dgroup_ptr(m,dgroup(0x2c40u))),0u);if(b.call_eced)b.call_eced(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))));do{st=b.call_ed0b?b.call_ed0b(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u)))):2u;}while(st!=2u);}store_le32(dgroup_ptr(m,dgroup(0x2c38u)),0xffffffffu);store_le32(dgroup_ptr(m,dgroup(0x2c30u)),0xffffffffu);}static_assert(0xBB80u-0xBA98u==232u);''')

replace_func('function_bb80','static_assert(0xBBE9u-0xBB80u==105u)',r'''void function_bb80(Machine&m,LowGraphics13Boundary&b,u32 value)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||load_le32(dgroup_ptr(m,dgroup(0x2c38u)))==0xffffffffu)return;const u32 st=b.call_ed0b?b.call_ed0b(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u)))):0u;if(st==0u||st==2u){store_le32(dgroup_ptr(m,dgroup(0x2c30u)),0xffffffffu);store_le32(dgroup_ptr(m,dgroup(0x2c38u)),0xffffffffu);}else if(b.call_ed29)b.call_ed29(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),0u,value);}static_assert(0xBBE9u-0xBB80u==105u);''')

replace_func('function_bcb4','static_assert(0xBD20u-0xBCB4u==108u)',r'''void function_bcb4(Machine&m,LowGraphics13Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||load_le32(dgroup_ptr(m,dgroup(0x2c38u)))==0xffffffffu)return;const u32 st=b.call_ed0b?b.call_ed0b(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u)))):0u;if(st==0u||st==2u){store_le32(dgroup_ptr(m,dgroup(0x2c38u)),0xffffffffu);store_le32(dgroup_ptr(m,dgroup(0x2c30u)),0xffffffffu);return;}while((b.call_ed15?b.call_ed15(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u)))):0u)!=0u){}function_ba98(m,b);}static_assert(0xBD20u-0xBCB4u==108u);''')

replace_func('function_bd20','static_assert(0xBD83u-0xBD20u==99u)',r'''void function_bd20(Machine&m,LowGraphics13Boundary&b,u32 value)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2c3cu)))==value)return;store_le32(dgroup_ptr(m,dgroup(0x2c3cu)),value);if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||load_le32(dgroup_ptr(m,dgroup(0x2c38u)))==0xffffffffu)return;if((b.call_ed15?b.call_ed15(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u)))):0u)==0x64u){const u32 rate=static_cast<u32>(function_b598(m,b));if(b.call_ed29)b.call_ed29(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u))),rate,0u);}}static_assert(0xBD83u-0xBD20u==99u);''')

replace_func('function_bde4','static_assert(0xBE26u-0xBDE4u==66u)',r'''[[nodiscard]]u32 function_bde4(Machine&m,LowGraphics13Boundary&b)noexcept{if(load_le32(dgroup_ptr(m,dgroup(0x2b84u)))==0u||load_le32(dgroup_ptr(m,dgroup(0x2c38u)))==0xffffffffu)return 0u;const u32 st=b.call_ed0b?b.call_ed0b(load_le32(dgroup_ptr(m,dgroup(0x2b90u))),load_le32(dgroup_ptr(m,dgroup(0x2c30u)))):0u;return(st==0u||st==2u)?1u:st;}static_assert(0xBE26u-0xBDE4u==66u);''')

assert 'void function_b430(Machine&m,LowGraphics13Boundary&b)noexcept{' in s
assert '[[nodiscard]]s32 function_b598(Machine&m,LowGraphics13Boundary&b)noexcept{' in s
p.write_text(s)

a=Path('Code1/FINAL_BINARY_AUDIT.md')
t=a.read_text()
head='## Semantic audit checkpoint 10f — `B5E4..BE60`'
if head not in t:
    t += r'''

## Semantic audit checkpoint 10f — `B5E4..BE60`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the remainder of the audio/resource state block through `BE60`. `B5E4` now preserves the encoded early exit when `B4D4` cannot resolve an `ECBB` subrecord: that path returns immediately and therefore does **not** set the slot-loaded byte at `2C28h+slot`; only the `FFFFh` enumeration terminator reaches the loaded-flag store. The routine also reloads `2B90h` and the slot handle for each external enumeration/attach call, matching the machine code rather than retaining pre-call copies across opaque boundaries.

The playback-control routines `B6EC`, `B76C`, `B824`, `B8F4`, `BA98`, `BB80`, `BCB4`, and `BD20` now likewise preserve the binary's repeated reloads of `2B90h`, `2C30h`, `2C40h`, and per-slot handles after external audio calls. This matters because the assembly deliberately re-reads those globals between state, stop, rate, pan, start, and polling calls instead of assuming they are unchanged. Calls to `B598` are materialized at their encoded points before the corresponding `ED29` arguments are loaded.

`BDE4` was materially corrected: after `ED0B`, states `0` and `2` map to return value `1`, but every other state returns the original `ED0B` value still in EAX. The previous C++ incorrectly collapsed all nonterminal states to zero. `B6C0`, `B74C`, `B75C`, `BA04`, `BBEC`, `BC1C`, `BD84`, `BDCC`, `BDD8`, `BE28`, `BE34`, and `BE40` were rechecked and required no further semantic change. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `BE60`; `BE61..BE6F` is a fifteen-byte zero/alignment island and the next code begins at `BE70`.
'''
    a.write_text(t)
