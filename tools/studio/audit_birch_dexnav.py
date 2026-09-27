#!/usr/bin/env python3
"""Capture every page of Birch's native DexNav handoff and its repeat visit.

Run from the repository root with Studio Python and a fresh output directory.
The setup represents the first Route 103 rival victory; it is not a player save.
"""
import asyncio
import json
import sys
from pathlib import Path
from PIL import Image
sys.path.insert(0, str(Path('tools/studio').resolve()))
import server
from scenes import keys, packet_state
from native_tools import read_game_query

OUT = Path(sys.argv[1]).resolve()
OUT.mkdir(parents=True, exist_ok=True)
server.WORK = OUT
server.BUILD_STORE = OUT / 'builds'

async def main():
    studio = server.Studio(0)
    studio.build = await studio.stage()
    studio.build_id = studio.build['id']
    studio.core, packet = await studio.boot(studio.build)
    studio.ingest(packet)
    captures, inputs, queries = [], [], {}
    async def tick(frames, buttons=0):
        inputs.append({'frames':frames, 'keys':buttons})
        while frames:
            count = min(120, frames)
            studio.ingest(await studio.core.tick(keys=buttons, frames=count))
            frames -= count
    async def press(button, frames=90):
        await tick(1, keys(button))
        await tick(frames)
    def capture(name):
        path = OUT / (name + '.png')
        Image.frombytes('RGBA', (240,160), studio.packet[16:153616]).save(path)
        captures.append({'name':name, 'file':str(path), 'state':packet_state(studio.packet)})
    async def query(kind, name):
        return await read_game_query(studio.core, kind, studio.cat.constants[name], lambda: tick(1))
    setup = {
        'vars':{'VAR_BIRCH_LAB_STATE':4, 'VAR_DEX_UPGRADE_JOHTO_STARTER_STATE':0,
                'VAR_BIRCH_STATE':0, 'VAR_PETALBURG_GYM_STATE':0},
        'flags':{'FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_BIRCH':False,
                 'FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_RIVAL':False,
                 'FLAG_SYS_POKEDEX_GET':False, 'FLAG_RECEIVED_DEXNAV':False,
                 'FLAG_RECEIVED_POKEDEX_FROM_BIRCH':False, 'FLAG_SYS_POKENAV_GET':False}
    }
    passed = False
    try:
        await studio.command(dict(op='setup', **setup))
        # A portable checkpoint immediately outside the lab, for production ROM proof.
        await studio.command(dict(op='warp',map='LittlerootTown',x=7,y=17,facing=2))
        await tick(150)
        studio.ingest(await studio.core.field_command(1))
        await studio.core.rpc(6,str(OUT/'before-handoff.sav').encode())
        before = await query(4,'ITEM_CHERISH_BALL')
        # Enter through the native door; a direct warp skips its entry step.
        await press('UP',300)
        for page in range(100):
            capture(f'handoff-{page:02d}')
            await press('A')
            if studio.state[0]:break
        assert studio.state[0], 'Handoff did not return controls'
        capture('handoff-controls-returned')
        actors=packet_state(studio.packet)['actors']
        player=next(a for a in actors if a['local_id']==255)
        birch=next(a for a in actors if a['local_id']==2)
        assert (player['x'],player['y']) != (birch['x'],birch['y']), 'Player overlaps Birch'
        for kind,name in [(1,'FLAG_RECEIVED_DEXNAV'),(1,'FLAG_SYS_POKEDEX_GET'),
                          (1,'FLAG_SYS_POKENAV_GET'),(2,'VAR_BIRCH_LAB_STATE'),(4,'ITEM_CHERISH_BALL')]:
            queries[name] = await query(kind,name)
        assert queries['FLAG_RECEIVED_DEXNAV'] == 1
        assert queries['FLAG_SYS_POKEDEX_GET'] == 1
        assert queries['FLAG_SYS_POKENAV_GET'] == 0
        assert queries['VAR_BIRCH_LAB_STATE'] == 5
        assert queries['ITEM_CHERISH_BALL'] == before + 30
        await press('START',60);capture('handoff-start-menu')
        await press('B',90)
        await press('UP',30);await press('A',90);capture('birch-repeat-interaction')
        assert not studio.state[0], 'Repeat visit did not open Birch dialogue'
        for page in range(12):
            if studio.state[0]:break
            await press('B',90);capture(f'birch-repeat-exit-{page}')
        assert studio.state[0], 'Repeat visit did not return controls'
        assert await query(4,'ITEM_CHERISH_BALL') == queries['ITEM_CHERISH_BALL']
        assert await query(2,'VAR_BIRCH_LAB_STATE') == 5
        capture('repeat-controls-returned')
        passed = True
    finally:
        (OUT/'evidence.json').write_text(json.dumps(dict(build=studio.build_info(),synthetic=True,
            setup=setup,inputs=inputs,captures=captures,queries=queries,passed=passed),indent=2))
        await studio.core.close()
    print(OUT)

asyncio.run(main())
