from pathlib import Path

root=Path('.')
cpp=root/'Code2/Code2.cpp'
frag=root/'Code2/.audit_gap_39a8a.fragment.cpp'

def integrate():
    s=cpp.read_text()
    if 'function_39a8a(' in s:
        return
    marker='[[nodiscard]] u32 function_39bbc('
    pos=s.find(marker)
    if pos<0:
        raise SystemExit('39BBC insertion marker missing')
    s=s[:pos]+frag.read_text()+'\n\n'+s[pos:]
    cpp.write_text(s)

def verify():
    s=cpp.read_text()
    required=(
        'function_39a8a(',
        'static_assert(0x39BBCu-0x39A8Au==306u);',
        'dpmi_resize',
        'function_399eb(m,node)',
        'function_39bbc(m,new_node)',
    )
    for x in required:
        if x not in s: raise SystemExit('missing '+x)
    if s.count('function_39a8a(')!=1: raise SystemExit('duplicate 39A8A reconstruction')

if __name__=='__main__':
    import sys
    if len(sys.argv)!=2 or sys.argv[1] not in ('integrate','verify'):
        raise SystemExit('usage: integrate|verify')
    (integrate if sys.argv[1]=='integrate' else verify)()
