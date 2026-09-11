import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import sqlite3
import struct
import subprocess
from model import Game, dump

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'build'
SOURCE=ROOT/'game/campaign.ec'


def pack(game):
    records=game.materialize()
    strings=bytearray(b'\0');string_ids={'':0};values=bytearray();intern={}
    keys=sorted(game.records);ids={k:i for i,k in enumerate(keys)}
    def string(s):
        if s not in string_ids:
            string_ids[s]=len(strings);strings.extend(s.encode()+b'\0')
        return string_ids[s]
    def node(v):
        if v is None:return 0
        if isinstance(v,bool):return (6<<28)|int(v)
        if isinstance(v,int):
            if not -(1<<27)<=v<(1<<27):raise ValueError(f'integer out of representation: {v}')
            return (1<<28)|(v&0x0fffffff)
        if isinstance(v,str):
            if v in ids:return (5<<28)|ids[owner(v)]
            return (2<<28)|string(v)
        if isinstance(v,dict):
            tag=4;data=struct.pack('<I',len(v))+b''.join(struct.pack('<II',string(k),node(x)) for k,x in v.items())
        else:
            tag=3;data=struct.pack('<I',len(v))+b''.join(struct.pack('<I',node(x)) for x in v)
        if (tag,data) not in intern:
            intern[tag,data]=len(values);values.extend(data)
        return (tag<<28)|intern[tag,data]
    def owner(key):
        seen=set()
        while isinstance(game.records.get(key),dict) and 'alias' in game.records[key]:
            if key in seen:raise ValueError(f'cyclic alias: {key}')
            seen.add(key);key=game.records[key]['alias']
        return key
    slots={};counts={k:0 for k in ('item','var','flag')}
    for key in keys:
        kind=key.split(':')[0]
        if kind in counts:
            root=owner(key)
            if root not in slots:
                slots[root]=counts[kind];counts[kind]+=1
            slots[key]=slots[root]
    table=b''.join(struct.pack('<III',string(k),node(records[k]),slots.get(k,0xffffffff)) for k in keys)
    strings.extend(b'\0'*((-len(strings))%4))
    header=struct.pack('<10I',0x32474345,len(keys),40,40+len(table),len(strings),40+len(table)+len(strings),len(values),counts['item'],counts['var'],counts['flag'])
    result=header+table+strings+values
    (OUT/'campaign.bin').write_bytes(result)
    return {'records':len(keys),'bytes':len(result),'sha256':hashlib.sha256(result).hexdigest()}


def index(game):
    path=OUT/'index.sqlite'
    path.unlink(missing_ok=True)
    with sqlite3.connect(path) as db:
        db.executescript('CREATE TABLE record(id TEXT PRIMARY KEY,kind TEXT,line INTEGER); CREATE TABLE edge(source TEXT,target TEXT,relation TEXT,field TEXT); CREATE INDEX incoming ON edge(target); CREATE INDEX outgoing ON edge(source);')
        db.executemany('INSERT INTO record VALUES(?,?,?)',((k,k.split(':')[0],line) for k,line in game.lines.items()))
        db.executemany('INSERT INTO edge VALUES(?,?,?,?)',((a,b,c,json.dumps(d)) for edges in game.outgoing.values() for a,b,c,d in edges))


def context():
    files=['Makefile','game/campaign.ec','game/model.py','game/build.py','game/runtime.h','game/runtime.c','game/boot.s','game/gba.ld']
    source='\n'.join('\n@@ '+name+'\n'+(ROOT/name).read_text() for name in files)
    (OUT/'context.txt').write_text(source)
    try:
        import tiktoken
        tokens=len(tiktoken.get_encoding('o200k_base').encode(source))
    except ImportError:tokens=None
    return {'bytes':len(source.encode()),'tokens':tokens,'tokenizer':'o200k_base' if tokens else None,'fits_1m':tokens<=1000000 if tokens else None}


def status(game):
    kinds=Counter(k.split(':')[0] for k in game.records)
    unresolved=game.unresolved()
    return {'records':dict(kinds),'undefined_references':dict(Counter(k.split(':')[0] for k in unresolved)),
            'moveset_conflicts':len(game.compatibility()),
            'unported_native_functions':sum(k.startswith('native:') and v[0]=='unported' for k,v in game.records.items()),
            'pixel_fidelity_verified':False,'playable_rom':False}


def main():
    p=argparse.ArgumentParser();p.add_argument('action',choices=('build','context','impact','economy','check','status','rom'));p.add_argument('id',nargs='?');args=p.parse_args()
    OUT.mkdir(exist_ok=True)
    if args.action=='context':print(json.dumps(context(),indent=2));return
    game=Game(SOURCE)
    if args.action=='build':
        removed=game.reconcile()
        if removed:
            dump(game.records,SOURCE)
            game=Game(SOURCE)
        result=pack(game);index(game)
        subprocess.run(['cc','-std=c11','-Wall','-Wextra','-Werror','-c',str(ROOT/'game/runtime.c'),'-o',str(OUT/'runtime.o')],check=True)
        print(json.dumps(result|status(game)|{'removed_moves':removed},indent=2))
    elif args.action=='rom':
        raise SystemExit('ROM link unavailable: the replacement battle, rendering, audio, save, and native event handlers are unfinished. campaign.bin is data, not a playable ROM.')
    elif args.action=='status':print(json.dumps(status(game),indent=2))
    elif args.action=='check':
        issues=game.compatibility();print(json.dumps(issues,indent=2));raise SystemExit(bool(issues))
    elif args.action=='impact':
        if not args.id:p.error('impact requires a record id')
        distance,via=game.impact(args.id)
        print(json.dumps([{'id':k,'line':game.lines.get(k),'distance':d,'via':via.get(k)} for k,d in sorted(distance.items(),key=lambda x:(x[1],x[0]))],indent=2))
    elif args.action=='economy':
        if not args.id:p.error('economy requires an item id')
        print(json.dumps(game.economy(args.id),indent=2))
if __name__=='__main__':main()
