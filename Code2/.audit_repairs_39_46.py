from pathlib import Path
import re,sys

p=Path('Code2/Code2.cpp')
s=p.read_text()

def sub_once(pattern,repl,label,flags=0):
    global s
    s2,n=re.subn(pattern,repl,s,count=1,flags=flags)
    if n!=1: raise SystemExit(f'{label}: target not found or ambiguous ({n})')
    s=s2

def integrate():
    global s
    # CP39 / 372F6: TEST AL,03h ; JNE skips setup. Setup therefore runs only
    # when both low gate bits are clear.
    sub_once(r'if\(\(gate&3u\)!=3u\)', 'if((gate&3u)==0u)', '372f6 gate predicate')

    # CP40 / 3736D: make the encoded [ESP+24h] divisor/quotient alias explicit.
    old="do { const u32 q=value/radix,r=value%radix; value=q; tmp[++n]=static_cast<u8>(r<10u?('0'+r):('a'+r-10u)); } while(value!=0u);"
    new="do { u32 slot=radix; const u32 q=value/slot,r=value%slot; slot=q; value=slot; tmp[++n]=static_cast<u8>(r<10u?('0'+r):('a'+r-10u)); } while(value!=0u);"
    if s.count(old)!=1: raise SystemExit('3736d alias target not found or ambiguous')
    s=s.replace(old,new,1)

    # CP40 / 37433: encoded slot is restored to radix before DIV, overwritten
    # with remainder, then reloaded for the alphabet index while EAX keeps q.
    old="do { const u32 q=value/radix,r=value%radix; value=q; tmp[++n]=static_cast<u8>(r<10u?('0'+r):('a'+r-10u)); } while(value!=0u);"
    new="do { u32 slot=radix; const u32 q=value/slot,r=value%slot; slot=r; const u32 index=slot; value=q; tmp[++n]=static_cast<u8>(index<10u?('0'+index):('a'+index-10u)); } while(value!=0u);"
    if s.count(old)!=1: raise SystemExit('37433 alias target not found or ambiguous')
    s=s.replace(old,new,1)

    # CP42 / 37810: payload source is matched table POSITION, not displaced value.
    sub_once(r'const u32 q=static_cast<u32>\(chosen\);\n        const u32 c=',
             'const u32 q=pos;\n        const u32 c=', '37810 matched position')

    # CP45 / 37D60: preserve the encoded zero-unread path through the shared
    # post-seek compare. EAX is the zero pos value when the seek call is skipped.
    old="else if(s.buffer!=0u){s.flags0&=0xEFu;if(!(s.flags1&0x20u)&&s.pos!=0u&&b.stream_seek&&b.stream_seek(s.handle,-static_cast<s32>(s.pos),1u)==-1){s.flags0|=0x20u;rc=-1;}}"
    new="else if(s.buffer!=0u){s.flags0&=0xEFu;if(!(s.flags1&0x20u)){s32 seek_result=static_cast<s32>(s.pos);if(s.pos!=0u&&b.stream_seek)seek_result=b.stream_seek(s.handle,-static_cast<s32>(s.pos),1u);if(seek_result==-1){s.flags0|=0x20u;rc=-1;}}}"
    if old not in s: raise SystemExit('37d60 shared-tail target not found')
    s=s.replace(old,new,1)

    # CP46 / 37EFA: first 3A0A8 site falls through only on zero; later positive
    # device publication remains unchanged.
    old="if((mode&3u)!=0u && handle!=-1 && b.descriptor_is_device && b.descriptor_is_device(static_cast<u32>(handle))){"
    new="if((mode&3u)!=0u && handle!=-1 && b.descriptor_is_device && !b.descriptor_is_device(static_cast<u32>(handle))){"
    if s.count(old)!=1: raise SystemExit('37efa predicate target not found or ambiguous')
    s=s.replace(old,new,1)

    p.write_text(s)

def verify():
    t=p.read_text()
    checks=(
        ('372f6', 'if((gate&3u)==0u)'),
        ('3736d', 'slot=q; value=slot;'),
        ('37433', 'slot=r; const u32 index=slot; value=q;'),
        ('37810', 'const u32 q=pos;'),
        ('37d60', 's32 seek_result=static_cast<s32>(s.pos);'),
        ('37efa', 'b.descriptor_is_device && !b.descriptor_is_device(static_cast<u32>(handle))'),
    )
    for label,text in checks:
        if text not in t: raise SystemExit(f'{label}: repair missing')
    if 'if((gate&3u)!=3u)' in t: raise SystemExit('372f6 stale predicate remains')

if __name__=='__main__':
    if len(sys.argv)!=2 or sys.argv[1] not in ('integrate','verify'):
        raise SystemExit('usage: integrate|verify')
    integrate() if sys.argv[1]=='integrate' else verify()
