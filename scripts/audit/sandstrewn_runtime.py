#!/usr/bin/env python3
"""Studio native walk: tower-side entry -> basement -> researcher, without Surf.

The path planner only proposes inputs. Native position, flags, cap, and money
are the assertions. This is synthetic chapter setup, never an earned clear.
"""
import argparse
import asyncio
from collections import deque
import json
from pathlib import Path
import re
import struct
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/studio'))
import server
from scenes import Recorder, keys, packet_state


def navigation():
    layouts = {l['id']: l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']}
    heads = (ROOT/'src/data/tilesets/headers.h').read_text()
    metas = (ROOT/'src/data/tilesets/metatiles.h').read_text()
    enum = re.sub(r'//[^\n]*', '', (ROOT/'include/constants/metatile_behaviors.h').read_text()).split('enum')[1]
    names = list(dict.fromkeys(re.findall(r'\bMB_\w+', enum)))
    water = {i for i, n in enumerate(names) if 'WATER' in n and 'SHALLOW' not in n}
    maps, ids = {}, {}
    for p in (ROOT/'data/maps').glob('*/map.json'):
        if p.parent.name != 'Route111' and not p.parent.name.startswith(('SandstrewnRuins', 'MirageTower')):
            continue
        m = json.loads(p.read_text()); l = layouts[m['layout']]
        m.update(w=l['width'], h=l['height'])
        m['cells'] = struct.unpack('<'+'H'*(m['w']*m['h']), (ROOT/l['blockdata_filepath']).read_bytes())
        attrs = []
        for ident in (l['primary_tileset'], l['secondary_tileset']):
            body = re.search(r'struct Tileset '+ident+r'\s*=\s*\{(.*?)\};', heads, re.S)[1]
            ref = re.search(r'\.metatileAttributes\s*=\s*(\w+)', body)[1]
            path = re.search(ref+r'\[\].*?\("([^"]+)"', metas)[1]
            b = (ROOT/path).read_bytes(); v = list(struct.unpack('<'+'H'*(len(b)//2), b))
            attrs += v+[0]*(512-len(v))
        m['water'] = {i for i, c in enumerate(m['cells']) if (attrs[c&1023]&255) in water}
        m['blocked'] = {(o['x'], o['y']) for o in m['object_events']}
        maps[p.parent.name] = m; ids[m['id']] = p.parent.name
    return maps, ids


def next_input(start, goal, maps, ids, blocked):
    queue = deque([start]); seen = {start: None}
    while queue:
        a = queue.popleft()
        if a == goal:
            while seen[a][0] != start:
                a = seen[a][0]
            return seen[a][1]
        name, x, y = a; m = maps[name]; elevation = m['cells'][y*m['w']+x] >> 12
        for dx, dy, key in ((1,0,'RIGHT'),(-1,0,'LEFT'),(0,1,'DOWN'),(0,-1,'UP')):
            xx, yy = x+dx, y+dy
            if (a,key) in blocked or not 0 <= xx < m['w'] or not 0 <= yy < m['h']:
                continue
            index = yy*m['w']+xx; cell = m['cells'][index]; other = cell >> 12
            if cell&0xc00 or (elevation and other and elevation != other) or index in m['water'] or (xx,yy) in m['blocked']:
                continue
            b = name, xx, yy
            for warp in m['warp_events']:
                if (xx, yy) == (warp['x'], warp['y']) and warp['dest_map'] in ids:
                    dest = ids[warp['dest_map']]; w = maps[dest]['warp_events'][int(warp['dest_warp_id'])]
                    b = dest, w['x'], w['y']; break
            if b not in seen:
                seen[b] = a, key; queue.append(b)
    raise RuntimeError(f'No dry path from {start} to {goal}; inspect native obstruction')


async def run(out):
    server.WORK = out/'runtime'
    studio = server.Studio(0); build = await studio.stage()
    studio.core, packet = await studio.boot(build, chapter=3)
    studio.build, studio.build_id = build, build['id']; studio.ingest(packet)
    flags = {f'FLAG_BADGE0{i}_GET': True for i in range(1,5)}
    flags.update(FLAG_EC_RESOLVED_MOLTRES=True, FLAG_BADGE05_GET=False, FLAG_RECEIVED_HM_SURF=False, FLAG_EC_REPEL_SPRAY_ACTIVE=True)
    setup = dict(flags=flags, vars={'VAR_CHANSEY_NURSE_STATE':7, 'VAR_REPEL_STEP_COUNT':0, 'VAR_EC_REPEL_SPRAY_STEPS':500}, items={'ITEM_REPEL_SPRAY':1, 'ITEM_GO_GOGGLES':1})
    await studio.command(dict(op='setup', **setup))
    await studio.command(dict(op='party', party=['SPECIES_GEODUDE','SPECIES_ZIGZAGOON']))
    await studio.command(dict(op='warp', map='Route111', x=19, y=59, facing=2))
    for _ in range(180): studio.ingest(await studio.core.tick(frames=1))
    initial, portable = await studio.scene_start_files(out)
    rec = Recorder(ROOT, out, 'C26 basement roundtrip before Norman and Surf', studio.build_info(), initial, portable)
    rec.observe(studio.packet, force=True, label='Route111 approach with four badges; no Surf')
    maps, ids = navigation(); blocked = set(); moves = []
    async def tick(mask=0, frames=1):
        for _ in range(frames):
            studio.ingest(await studio.core.tick(mask, frames=1)); rec.observe(studio.packet, mask)
    async def query(kind, name=0):
        ident = studio.cat.constants[name] if isinstance(name,str) else name
        await studio.core.write([(studio.core.syms['gEcHeadlessCampaignQueryKind'],kind),(studio.core.syms['gEcHeadlessCampaignQueryId'],ident)])
        await tick(frames=3)
        return struct.unpack('<I',await studio.core.rpc(4,server.words(studio.core.syms['gEcHeadlessCampaignQueryValue'],4)))[0]
    def position(): return studio.current_map, studio.state[4], studio.state[5]
    async def walk(goal, label):
        for _ in range(250):
            a = position()
            if a == goal:
                rec.mark(label,studio.packet); return
            key = next_input(a,goal,maps,ids,blocked)
            await tick(keys(key),16); await tick(frames=160)
            if studio.state[1]: raise RuntimeError('Unexpected wild battle during traversal fixture')
            b = position()
            # Arrow warps trigger when continuing off their tile, not merely
            # when arriving on it. Never repeat across an already-completed warp.
            if b != goal and b[0] == a[0] and abs(b[1]-a[1])+abs(b[2]-a[2]) == 1 and any((w['x'],w['y']) == b[1:] for w in maps[b[0]]['warp_events']):
                await tick(keys(key),16); await tick(frames=160)
                b = position()
            moves.append(dict(before=a,key=key,after=b))
            if a == b: blocked.add((a,key))
        raise RuntimeError('Bounded native walk did not reach '+str(goal))
    async def talk(label):
        await tick(keys('UP'),2); await tick(frames=30)
        await tick(keys('A'),1); await tick(frames=149)
        rec.mark(label,studio.packet)
        if not studio.state[11]: raise RuntimeError('Researcher dialogue did not start')
        idle = 0
        for frame in range(4000):
            await tick(keys('A') if frame%100 == 0 else 0)
            idle = idle+1 if studio.state[0] else 0
            if idle >= 20: return
        raise RuntimeError('Researcher did not return controls')
    outcome = {'passed':False, 'setup':setup, 'moves':moves}
    try:
        await walk(('SandstrewnRuins',9,131),'Researcher on dry entrance path')
        await talk('Warning and unfinished basement directions')
        if await query(1,'FLAG_EC_REPORT_C26_COMPLETE'): raise RuntimeError('Report completed before basement')
        await walk(('SandstrewnRuins_B1F',4,3),'Lowest chamber reached without Surf')
        if not await query(1,'FLAG_EC_SURVEYED_DESERT_DEPTHS'): raise RuntimeError('Native basement entry was not recorded')
        await walk(('SandstrewnRuins',9,131),'Return through the ruin stairways')
        await talk('Research credit and Norman/Surf handoff')
        if not await query(1,'FLAG_EC_REPORT_C26_COMPLETE'): raise RuntimeError('Research report not completed')
        money = await query(9)
        if money != 14000 or studio.state[8] != 48: raise RuntimeError('Incorrect C26 stipend or cap')
        await talk('Repeat keeps optional finds available')
        if await query(9) != money: raise RuntimeError('Repeated research paid again')
        await walk(('Route111',19,59),'Back outside the tower; ready for Norman')
        if await query(1,'FLAG_BADGE05_GET') or await query(1,'FLAG_RECEIVED_HM_SURF'): raise RuntimeError('Traversal gained or required Surf')
        outcome.update(passed=True, final=packet_state(studio.packet,rec.decoder),money=money)
    except Exception as error:
        outcome['error'] = str(error)
        raise
    finally:
        await asyncio.to_thread(rec.finish,studio.packet,outcome)
        (out/'navigation.json').write_text(json.dumps(moves,indent=2)+'\n')
        await studio.core.close()

if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
    out = a.out.resolve();out.mkdir(parents=True,exist_ok=False)
    asyncio.run(run(out))
