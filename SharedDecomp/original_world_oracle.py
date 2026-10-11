#!/usr/bin/env python3
"""Read-only, independent original LEVELS.DAT/SINTAB oracle; exports metadata only."""
from __future__ import annotations
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import struct
import sys

IDENTITY = {
    "LEVELS.DAT": "50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91",
    "SINTAB": "19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87",
}

class OracleError(ValueError):
    pass

def expect(ok, message):
    if not ok: raise OracleError(message)

def sha(data):
    return hashlib.sha256(data).hexdigest()

def number(data, offset, size=4, signed=False):
    expect(0 <= offset <= len(data)-size, "bounded binary integer read failed")
    return int.from_bytes(data[offset:offset+size], "little", signed=signed)

def inspect_levels(data):
    expect(len(data)>=132 and number(data,0)==32, "invalid LEVELS directory")
    offsets=[number(data,4+4*i) for i in range(32)]+[len(data)]
    expect(offsets[0]==132, "invalid first level file offset")
    stats=Counter()
    cells=Counter()
    levels=[]
    for id in range(32):
        start,end=offsets[id:id+2]
        expect(132<=start<end<=len(data), f"level {id} directory outside source")
        rec=data[start:end]
        cuts=[number(rec,1+4*j) for j in range(7)]
        expect(cuts[0]==29 and cuts[-1]<=len(rec) and
               all(a<=b for a,b in zip(cuts,cuts[1:])),
               f"level {id} section partition invalid")
        w,h=number(rec,30,2,True),number(rec,32,2,True)
        expect(0<w<=512 and 0<h<=512, f"level {id} dimensions")
        count=w*h
        cell_start=cuts[0]+0x2a4
        expect(cuts[1]==cell_start+2*count, f"level {id} packed-cell size")
        raw_cells=rec[cell_start:cuts[1]]
        words=struct.unpack("<"+str(count)+"H",raw_cells)
        cells.update(words)
        cursor=cuts[1]
        controls=0
        for col in range(w):
            n=number(rec,cursor,2,True)
            expect(n>=0 and cursor+2+n*10<=cuts[2],
                   f"level {id} column {col} control list")
            cursor+=2+10*n
            controls+=n
        expect(cursor==cuts[2], f"level {id} leftover controls")
        a,c,b=(number(rec,cuts[0]+k,2,True)
               for k in (0x28a,0x28c,0x28e))
        expect(min(a,b,c)>=0 and
               tuple(cuts[j+1]-cuts[j] for j in (2,3,4))==(11*a,11*b,11*c),
               f"level {id} family lengths")
        expect(cuts[6]-cuts[5]==1300, f"level {id} event table size")
        events=commands=0
        for slot in range(50):
            at=cuts[5]+26*slot
            n=rec[at]
            expect(n<=5, f"level {id} slot {slot} command count")
            events+=int(n>0)
            commands+=n
            for j in range(n):
                expect(rec[at+1+j*5]<=12, f"level {id} slot {slot} opcode")
        cursor=cuts[6]
        expect(number(rec,cursor,2,True)==w and
               number(rec,cursor+2,2,True)==h,
               f"level {id} automap dimensions")
        tail=cursor+4+count
        annotations=number(rec,tail)
        expect(annotations<=0x7fe and tail+4+annotations==len(rec),
               f"level {id} annotation tail")
        levels.append({
            "id":id,"w":w,"h":h,"cells":count,"controls":controls,
            "a":a,"b":b,"c":c,"events":events,"commands":commands,
            "nonzero_words":sum(x!=0 for x in words),
            "bit2000_words":sum(bool(x&0x2000) for x in words),
            "record_sha256":sha(rec),"cells_sha256":sha(raw_cells),
            "controls_sha256":sha(rec[cuts[1]:cuts[2]]),
            "events_sha256":sha(rec[cuts[5]:cuts[6]]),
            "automap_sha256":sha(rec[cursor+4:tail]),
            "annotations_sha256":sha(rec[tail+4:])})
        stats.update(cells=count,controls=controls,event_slots=events,
                     event_commands=commands)
    summary={"level_count":32,"level_cells":stats["cells"],
             "controls":stats["controls"],"event_slots":stats["event_slots"],
             "event_commands":stats["event_commands"],
             "level_archive_bytes":len(data),
             "distinct_cell_words":len(cells),
             "nonzero_cell_words":sum(n for word,n in cells.items() if word),
             "bit2000_cell_words":sum(n for word,n in cells.items() if word&0x2000)}
    return summary,levels

def inspect_sintab(data):
    expect(len(data)>=0x1f42+512, "SINTAB short source")
    count=number(data,0x1b38)
    lookup=number(data,0x1b3c)
    aux=number(data,0x1b40)
    expect((count,lookup,aux)==(127,508,128), "SINTAB renderer table headers")
    cursor=0x1b44+(lookup+1)*2
    declared=number(data,cursor)
    cursor+=4
    blocks=[]
    total=0
    for index in range(count):
        length=number(data,cursor,2)
        cursor+=2
        expect(length>=0x205 and cursor+length<=len(data)-512,
               f"SINTAB block {index} length")
        payload=data[cursor:cursor+length]
        offsets=struct.unpack_from("<129H",payload)
        expect(offsets[0]==0x204 and offsets[-1]<length and
               all(a<=b for a,b in zip(offsets,offsets[1:])),
               f"SINTAB block {index} offsets")
        expect(payload[-1]==0xc3, f"SINTAB block {index} missing RET")
        steps=struct.unpack_from("<129H",payload,0x102)
        blocks.append({"id":index,"bytes":length,"step_sum":sum(steps),
                       "first_code_offset":offsets[0],
                       "last_code_offset":offsets[-1],
                       "sha256":sha(payload)})
        cursor+=length
        total+=length
    expect(total==declared,"SINTAB declared scale sum")
    expect(data[cursor:]==bytes(512),"SINTAB reserved trailer")
    return {"sintab_bytes":len(data),"sintab_scales":count,
            "sintab_scale_bytes":total},blocks

def audit(levels,sintab,require_known=False):
    hashes={"LEVELS.DAT":sha(levels),"SINTAB":sha(sintab)}
    known=hashes==IDENTITY
    expect(known or not require_known,"original SHA256 does not match pinned version")
    left,records=inspect_levels(levels)
    right,blocks=inspect_sintab(sintab)
    return {"schema":"rloft-original-world-oracle/v1",
            "status":"PINNED_ORIGINAL" if known else "STRUCTURAL_ONLY_UNPINNED_VERSION",
            "original_sha256":hashes,"pinned_original":known,
            "summary":{**left,**right},"levels":records,"sintab_blocks":blocks,
            "raw_original_bytes_included":False,
            "dos_execution_verified":False,"native_gameplay_parity":False}

def compare(original,native):
    expect(original.get("schema")=="rloft-original-world-oracle/v1" and
           native.get("schema")=="rloft-native-world-system/v1",
           "different original/native world report schemas")
    differences=[]
    keys=("level_count","level_cells","controls","event_slots","event_commands",
          "level_archive_bytes","sintab_bytes","sintab_scales","sintab_scale_bytes")
    for key in keys:
        expected=original["summary"][key]
        observed=native.get(key)
        if type(observed) is not int or observed!=expected:
            differences.append({"field":key,"original":expected,"native":observed})
    rows=native.get("levels")
    expect(isinstance(rows,list) and len(rows)<=64,"native world level list invalid")
    if len(rows)!=32:
        differences.append({"field":"levels/count","original":32,"native":len(rows)})
    for i,row in enumerate(rows[:32]):
        expect(isinstance(row,dict),"native level metadata not object")
        reference=original["levels"][i]
        for key in ("id","w","h","cells","controls","a","b","c","events","commands"):
            observed=row.get(key)
            if type(observed) is not int or observed!=reference[key]:
                differences.append({"field":f"levels/{i}/{key}",
                                    "original":reference[key],"native":observed})
    return {"schema":"rloft-original-native-world-diff/v1",
            "status":"STRUCTURAL_MATCH" if not differences else "STRUCTURAL_MISMATCH",
            "difference_count":len(differences),
            "first_divergence":differences[0] if differences else None,
            "differences":differences[:64],
            "original_sha256":original["original_sha256"],
            "pinned_original":original["pinned_original"],
            "native_original_sha256_attested":False,
            "native_cell_bytes_compared":False,
            "native_sintab_bytes_compared":False,
            "dos_execution_verified":False,"native_gameplay_parity":False}

def main(argv=None):
    from workflow import atomic_json
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--levels",required=True,type=Path)
    p.add_argument("--sintab",required=True,type=Path)
    p.add_argument("--require-known",action="store_true")
    p.add_argument("--native-report",type=Path)
    p.add_argument("--report",required=True,type=Path)
    args=p.parse_args(argv)
    try:
        for name in (args.levels,args.sintab):
            expect(0<name.stat().st_size<=8*1024*1024,"original file outside size limit")
        source=audit(args.levels.read_bytes(),args.sintab.read_bytes(),
                     require_known=args.require_known)
        outcome=source
        if args.native_report:
            outcome=compare(source,json.loads(args.native_report.read_text(encoding="utf-8-sig")))
        atomic_json(args.report,outcome)
        print(json.dumps({"status":outcome["status"],
                          "pinned_original":source["pinned_original"],
                          "native_gameplay_parity":False},sort_keys=True))
        return int(outcome["status"]=="STRUCTURAL_MISMATCH")
    except (OracleError,OSError,ValueError,KeyError,TypeError,struct.error) as exc:
        print("ORIGINAL WORLD ORACLE ERROR:",exc,file=sys.stderr)
        return 2

if __name__=="__main__":
    raise SystemExit(main())
