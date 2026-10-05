from pathlib import Path
import re,sys

p=Path('Code2/Code2.cpp')
s=p.read_text()

EXPECTED_BLOB='ae7abe2bb54211a880f04d52322a41976dd5707c'

def once(old,new,label):
    global s
    if new in s: return
    n=s.count(old)
    if n!=1: raise SystemExit(f'{label}: expected one target, got {n}')
    s=s.replace(old,new,1)

def nth(old,new,n,label):
    global s
    if new in s: return
    starts=[]; i=0
    while True:
        i=s.find(old,i)
        if i<0: break
        starts.append(i); i+=len(old)
    if len(starts)<n: raise SystemExit(f'{label}: expected occurrence {n}, got {len(starts)}')
    i=starts[n-1]; s=s[:i]+new+s[i+len(old):]

def insert_fragment(path,marker,symbol):
    global s
    if symbol in s:return
    pos=s.find(marker)
    if pos<0: raise SystemExit(f'{symbol}: insertion marker missing')
    frag=Path(path).read_text().rstrip()+'\n\n'
    s=s[:pos]+frag+s[pos:]

def integrate():
    global s
    # CP39-46 confirmed repairs.
    once('if((gate&3u)!=3u)','if((gate&3u)==0u)','372F6')
    old="do { const u32 q=value/radix,r=value%radix; value=q; tmp[++n]=static_cast<u8>(r<10u?('0'+r):('a'+r-10u)); } while(value!=0u);"
    a="do { u32 slot=radix; const u32 q=value/slot,r=value%slot; slot=q; value=slot; tmp[++n]=static_cast<u8>(r<10u?('0'+r):('a'+r-10u)); } while(value!=0u);"
    b="do { u32 slot=radix; const u32 q=value/slot,r=value%slot; slot=r; const u32 index=slot; value=q; tmp[++n]=static_cast<u8>(index<10u?('0'+index):('a'+index-10u)); } while(value!=0u);"
    nth(old,a,1,'3736D'); nth(old,b,1,'37433')
    once('const u32 q=static_cast<u32>(chosen);\n        const u32 c=','const u32 q=pos;\n        const u32 c=','37810')
    once("else if(s.buffer!=0u){s.flags0&=0xEFu;if(!(s.flags1&0x20u)&&s.pos!=0u&&b.stream_seek&&b.stream_seek(s.handle,-static_cast<s32>(s.pos),1u)==-1){s.flags0|=0x20u;rc=-1;}}","else if(s.buffer!=0u){s.flags0&=0xEFu;if(!(s.flags1&0x20u)){s32 seek_result=static_cast<s32>(s.pos);if(s.pos!=0u&&b.stream_seek)seek_result=b.stream_seek(s.handle,-static_cast<s32>(s.pos),1u);if(seek_result==-1){s.flags0|=0x20u;rc=-1;}}}",'37D60')
    once('if((mode&3u)!=0u && handle!=-1 && b.descriptor_is_device && b.descriptor_is_device(static_cast<u32>(handle))){','if((mode&3u)!=0u && handle!=-1 && b.descriptor_is_device && !b.descriptor_is_device(static_cast<u32>(handle))){','37EFA')

    # CP53 / 39589: failure base is zero, not ESP.
    once('const u32 scratch=(aligned<available)?aligned:0u;\n    store_le32(dg(m,0x12650u),m.cpu.esp-scratch+raw);','const u32 base=(aligned<available)?(m.cpu.esp-aligned):0u;\n    store_le32(dg(m,0x12650u),base+raw);','39589')

    # CP57 / 39C7B: CF is the failure signal; base zero is legal; BX:CX and SI:DI are distinct outputs.
    once('    u32 (*dpmi_alloc)(u32 bytes,u16* handle_hi,u16* handle_lo)=nullptr;', '    struct Dpmi0501Result { bool carry=false; u32 base=0; u16 handle_hi=0; u16 handle_lo=0; };\n    Dpmi0501Result (*dpmi_alloc)(u32 bytes)=nullptr;', '39C7B boundary')
    old2='''    if(b.dpmi_alloc){\n        u16 hi=0u,lo=0u;\n        const u32 base=b.dpmi_alloc(bytes,&hi,&lo);\n        if(base!=0u){\n            store_le16(dg(m,base),hi); store_le16(dg(m,base+2u),lo);\n            store_le32(dg(m,base+4u),0u); store_le32(dg(m,base+8u),bytes-8u);\n            return base+8u;\n        }\n    }'''
    new2='''    if(b.dpmi_alloc){\n        const auto r=b.dpmi_alloc(bytes);\n        if(!r.carry){\n            const u32 base=r.base;\n            store_le16(dg(m,base),r.handle_lo); store_le16(dg(m,base+2u),r.handle_hi);\n            store_le32(dg(m,base+4u),0u); store_le32(dg(m,base+8u),bytes-8u);\n            return base+8u;\n        }\n    }'''
    once(old2,new2,'39C7B body')

    # Explicit audited gaps. Insert only at stable address-level successors.
    insert_fragment('Code2/.audit_gap_39a8a.fragment.cpp','[[nodiscard]] u32 function_39bbc(','function_39a8a(')
    insert_fragment('Code2/.audit_gap_39d4d.fragment.cpp','// Final reconciliation batch 143','function_39d4d(')
    insert_fragment('Code2/.audit_tail_39fed_fold1.fragment.cpp','// Final reconciliation batch 143','function_39fed(')
    insert_fragment('Code2/.audit_tail_3a1b3.fragment.cpp','// Final reconciliation batch 143','function_3a1b3(')
    p.write_text(s)

def verify():
    t=p.read_text()
    req=['if((gate&3u)==0u)','slot=q; value=slot;','slot=r; const u32 index=slot; value=q;','const u32 q=pos;','s32 seek_result=static_cast<s32>(s.pos);','b.descriptor_is_device && !b.descriptor_is_device','base+raw','Dpmi0501Result','if(!r.carry)','function_39a8a(','function_39d4d(','function_39fed(','function_3a1b3(']
    for x in req:
        if x not in t: raise SystemExit('missing: '+x)
    stale=['if((gate&3u)!=3u)','m.cpu.esp-scratch+raw','u32 (*dpmi_alloc)(u32 bytes,u16* handle_hi,u16* handle_lo)']
    for x in stale:
        if x in t: raise SystemExit('stale: '+x)
    for x in ['function_39a8a(','function_39d4d(','function_39fed(','function_3a1b3(']:
        if t.count(x)!=1: raise SystemExit(f'duplicate {x}: {t.count(x)}')

if __name__=='__main__':
    if len(sys.argv)!=2 or sys.argv[1] not in ('integrate','verify'): raise SystemExit('usage: integrate|verify')
    integrate() if sys.argv[1]=='integrate' else verify()
