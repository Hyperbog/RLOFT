#!/usr/bin/env python3
"""Validate Ravenloft RES0..RES12 and confirmed resource families."""
from __future__ import annotations
import argparse, hashlib, json, struct
from collections import Counter, defaultdict
from pathlib import Path


def parse_res(data: bytes):
    if len(data) < 4: raise ValueError("short RES")
    n = struct.unpack_from("<i", data, 0)[0]
    if n < 0: raise ValueError("negative count")
    end = 4 + 4*n
    if end > len(data): raise ValueError("offset table exceeds EOF")
    offs = struct.unpack_from(f"<{n}i", data, 4) if n else ()
    if n and offs[0] != end: raise ValueError("first offset != table end")
    prev = end
    for i, off in enumerate(offs):
        if off < prev or off > len(data): raise ValueError(f"bad offset {i}: {off}")
        prev = off
    entries = tuple(data[o:(offs[i+1] if i+1<n else len(data))] for i,o in enumerate(offs))
    return tuple(offs), entries


def encode_res(entries):
    n=len(entries); cur=4+4*n; offs=[]
    for e in entries: offs.append(cur); cur += len(e)
    return struct.pack("<i",n)+(struct.pack(f"<{n}i",*offs) if n else b"")+b"".join(entries)


def lzss(src: bytes, target: int):
    hist=bytearray([0xFE])*0x1000; hp=0xFEE; flags=ip=0; out=bytearray()
    while len(out)<target:
        flags >>= 1
        if not flags & 0x100:
            if ip>=len(src): raise ValueError("LZSS control EOF")
            flags=src[ip]|0xFF00; ip+=1
        if flags & 1:
            if ip>=len(src): raise ValueError("LZSS literal EOF")
            v=src[ip]; ip+=1; out.append(v); hist[hp]=v; hp=(hp+1)&0xFFF
        else:
            if ip+1>=len(src): raise ValueError("LZSS backref EOF")
            lo,hi=src[ip],src[ip+1]; ip+=2
            off=lo|((hi&0xF0)<<4); count=(hi&0x0F)+3
            for j in range(count):
                v=hist[(off+j)&0xFFF]; out.append(v); hist[hp]=v; hp=(hp+1)&0xFFF
                if len(out)==target: break
    return bytes(out), ip


def image_descriptor(entry, base):
    count,height=struct.unpack_from("<HH",entry,base)
    width=entry[base+4]; field5=entry[base+5]; method=entry[base+6]; field7=entry[base+7]
    return count,height,width,field5,method,field7


def validate_image(entry, base, payload_at):
    count,height,width,field5,method,field7=image_descriptor(entry,base)
    target=count*height*width*8; payload=entry[payload_at:]
    if method==0:
        if len(payload)!=target: raise ValueError(f"raw {len(payload)} != {target}")
        used=len(payload)
    elif method==3:
        _,used=lzss(payload,target)
        if used!=len(payload): raise ValueError(f"LZSS trailing {len(payload)-used}")
    else: used=-1              # method 1 exists in CODE.2; method 2 is error path; absent here
    return dict(count=count,height=height,width_units=width,field5=field5,
                compression=method,field7=field7,decoded_bytes=target,
                payload_bytes=len(payload),consumed_bytes=used)


def summarize(path: Path):
    data=path.read_bytes(); offs,entries=parse_res(data)
    kinds=Counter(); types=Counter(); headers=Counter(); comps=Counter(); vals=Counter()
    sizes=defaultdict(Counter); anomalies=[]; non_eh=[]
    for idx,e in enumerate(entries):
        if not e: kinds["zero-length"]+=1; continue
        if len(e)<5 or e[:2]!=b"EH":
            kinds["non-EH"]+=1
            rec={"index":idx,"bytes":len(e),"sha256":hashlib.sha256(e).hexdigest()}
            if path.name in ("RES9","RES12") and len(e)>=2:
                word_count=struct.unpack_from("<H",e,0)[0]
                expected=2+4*word_count
                rec.update(family="interaction-wordcode",word_count=word_count,expected_bytes=expected)
                if len(e)!=expected:
                    anomalies.append(dict(index=idx,type="non-EH",error=f"wordcode bytes {len(e)} != {expected}"))
                else:
                    companion=e[2+2*word_count:]
                    rec["companion_words"]=word_count
                    rec["companion_all_zero"]=not any(companion)
                    if any(companion): anomalies.append(dict(index=idx,type="non-EH",error="wordcode serialized companion bank is not zero"))
                    else: vals["non_eh_wordcode_shape_exact"]+=1
            elif path.name=="RES1" and e==b"\x60\x00\x00\x00":
                rec.update(family="four-byte-sentinel",value_u32=0x60); vals["non_eh_res1_sentinel"]+=1
            else:
                rec.update(family="unclassified")
            non_eh.append(rec)
            continue
        kinds["EH"]+=1; t=e[2]; hs=struct.unpack_from("<H",e,3)[0]; types[t]+=1; headers[(t,hs)]+=1
        try:
            if t==1:
                r=validate_image(e,5,13); comps[(1,r["compression"])]+=1; sizes[1][r["decoded_bytes"]]+=1; vals["type1_valid"]+=1
            elif t==2:
                if len(e)<141: raise ValueError("short type2")
                r=validate_image(e,133,141); comps[(2,r["compression"])]+=1; sizes[2][r["decoded_bytes"]]+=1
                if len(e[5:133])!=128: raise ValueError("font width table != 128")
                vals["type2_valid"]+=1
            elif t in (3,4):
                if len(e)<9: raise ValueError("short length-prefixed resource")
                declared=struct.unpack_from("<I",e,5)[0]; stored=len(e)-9
                sig=(e[9:13]==b"FORM" and e[17:21]==b"XDIR") if t==3 else e[9:].startswith(b"Creative Voice File\x1a")
                if not sig or declared!=stored:
                    anomalies.append(dict(index=idx,type=t,declared_bytes=declared,stored_bytes=stored,
                                          declared_minus_stored=declared-stored,signature_ok=sig))
                else: vals["type3_xdir_exact" if t==3 else "type4_voc_exact"]+=1
            elif t in (6,7):
                if len(e)<7: raise ValueError("short type6/7")
                target=struct.unpack_from("<H",e,5)[0]; decoded,used=lzss(e[7:],target)
                if used!=len(e)-7: raise ValueError(f"LZSS trailing {len(e)-7-used}")
                sizes[t][target]+=1; vals[f"type{t}_lzss_exact"]+=1
                if t==6:
                    if target % 128: raise ValueError(f"type6 decoded size {target} is not divisible by 128-byte column stride")
                    columns=target//128
                    vals[f"type6_columns_{columns}"]+=1
                    if columns not in (32,64): raise ValueError(f"unexpected supplied type6 column count {columns}")
                if t==7 and len(decoded)<10: raise ValueError("type7 decoded body < 10")
            else: vals[f"type{t}_unvalidated"]+=1
        except Exception as exc: anomalies.append(dict(index=idx,type=t,error=str(exc)))
    return dict(file=path.name,bytes=len(data),sha256=hashlib.sha256(data).hexdigest(),entry_count=len(entries),
        duplicate_adjacent_offsets=sum(a==b for a,b in zip(offs,offs[1:])),roundtrip_identical=encode_res(entries)==data,
        entry_kinds=dict(sorted(kinds.items())),eh_types={str(k):types[k] for k in sorted(types)},
        header_sizes={f"type{t}:size{s}":n for (t,s),n in sorted(headers.items())},
        compression={f"type{t}:method{m}":n for (t,m),n in sorted(comps.items())},validations=dict(sorted(vals.items())),
        decoded_sizes={f"type{t}":{str(k):v for k,v in sorted(c.items())} for t,c in sorted(sizes.items())},non_eh=non_eh,anomalies=anomalies)


def main():
    ap=argparse.ArgumentParser(); ap.add_argument("files",nargs="+",type=Path); ap.add_argument("--json",action="store_true"); a=ap.parse_args()
    reports=[summarize(p) for p in a.files]
    if a.json: print(json.dumps(reports,indent=2))
    else:
        for r in reports: print(f"{r['file']}: entries={r['entry_count']} EH={r['eh_types']} duplicates={r['duplicate_adjacent_offsets']} anomalies={len(r['anomalies'])} roundtrip={r['roundtrip_identical']}")
    return 0

if __name__=="__main__": raise SystemExit(main())
