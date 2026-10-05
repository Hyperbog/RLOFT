from pathlib import Path

root=Path('.')
cpp=root/'Code2/Code2.cpp'
frag=root/'Code2/.audit_gap_36cec.fragment.cpp'

def integrate():
    s=cpp.read_text()
    if 'void function_36cec(' in s:
        return
    marker='// Batch 123: PC speaker/PIT helpers and radix alphabet, 372F6..3736C.'
    if marker not in s:
        raise SystemExit('Batch-123 insertion marker missing')
    s=s.replace(marker,frag.read_text()+'\n\n'+marker,1)
    cpp.write_text(s)

def verify():
    s=cpp.read_text()
    required=(
        'void function_36cec(',
        'static_assert(0x372F6u-0x36CECu==1546u);',
        'function_36b9c(', 'function_36bd8(', 'function_36c10(',
        'function_36c44(', 'function_36c90(', 'function_36cbc(',
        'function_36b14(',
    )
    for x in required:
        if x not in s: raise SystemExit('missing '+x)
    if s.count('void function_36cec(')!=1: raise SystemExit('duplicate 36CEC reconstruction')

if __name__=='__main__':
    import sys
    if len(sys.argv)!=2 or sys.argv[1] not in ('integrate','verify'):
        raise SystemExit('usage: integrate|verify')
    (integrate if sys.argv[1]=='integrate' else verify)()
