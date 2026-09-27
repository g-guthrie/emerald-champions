#!/usr/bin/env python3
"""Capture native hunt menus and Pokérus cards from a synthetic party.

Run from the repository root with the Studio Python environment, passing a
fresh output directory. Uses the matching headless ROM/ELF; never a player save.
Inspect the emitted PNGs as well as evidence.json before accepting a UI change.
"""
import sys, asyncio, json, hashlib, struct
from pathlib import Path
from PIL import Image
sys.path.insert(0, str(Path('tools/studio').resolve()))
import server
from scenes import keys, packet_state
OUT=Path(sys.argv[1]).resolve();OUT.mkdir(parents=True,exist_ok=True)
server.WORK=OUT; server.BUILD_STORE=OUT/'builds'
async def main():
    s=server.Studio(0); build=await s.stage(); s.build=build;s.build_id=build['id']
    s.core,p=await s.boot(build);s.ingest(p)
    captures=[]
    async def tick(n=45,key=0):
        while n:
            f=min(n,120);s.ingest(await s.core.tick(keys=key,frames=f));n-=f
    async def press(k,n=45):
        await tick(1,keys(k));await tick(n)
    def snap(name):
        file=OUT/(name+'.png');Image.frombytes('RGBA',(240,160),s.packet[16:153616]).save(file)
        captures.append(dict(name=name,file=str(file),state=packet_state(s.packet)))
    async def warp(map='Route103',x=10,y=13):
        await s.command(dict(op='warp',map=map,x=x,y=y,facing=1));await tick(150)
    async def fixture(chain,slot=0,status=0xFE,nature=3,ailment=0):
        s.ingest(await s.core.field_command(9,[chain,slot,status,nature,ailment]));await tick(2)
    async def start_select(index):
        await press('START')
        cursor=(await s.core.rpc(4,server.words(s.core.syms['sStartMenuCursorPos'],1)))[0]
        for _ in range(cursor): await press('UP',15)
        for _ in range(index): await press('DOWN',15)
        await press('A',150)
    async def dex():
        await start_select(1)
    try:
        await s.command(dict(op='setup',flags={'FLAG_SYS_POKEDEX_GET':True,'FLAG_RECEIVED_DEXNAV':True}))
        await s.command(dict(op='party',party=['SPECIES_SNUBBULL','SPECIES_TOXEL','SPECIES_TREECKO','SPECIES_SHELLOS','SPECIES_WINGULL','SPECIES_MUDKIP']))
        await warp()
        for slot,status in enumerate([0xFE,0xFD,0xFC,0,0xFC,0]):
            await fixture(0,slot,status,0 if slot==4 else 3)
        # Save native progress for clean-boot release confirmation later.
        s.ingest(await s.core.field_command(1))
        await s.core.rpc(6,str(OUT/'summary-preview.sav').encode())
        for chain in (0,1,50,100):
            await warp();await fixture(chain)
            await dex();snap(f'chain-{chain:03d}-roster')
            await press('RIGHT');snap(f'chain-{chain:03d}-cursor-right')
            await press('LEFT');await press('A',150)
            for attempt in range(6):
                ptr=struct.unpack('<I',await s.core.rpc(4,server.words(s.core.syms['sDexNavSearchDataPtr'],4)))[0]
                if ptr: break
                snap(f'chain-{chain:03d}-placement-failure-{attempt}')
                await press('A',90)
                await dex();await press('A',150)
            assert ptr, 'Search did not find a tile after six attempts'
            snap(f'chain-{chain:03d}-search')
            if chain==0:
                await tick(1500);snap('chain-000-search-after-25-seconds')
        await warp()
        await start_select(2);snap('party-pokerus-states')
        await press('A',60);snap('party-actions');await press('A',150);snap('summary-source-two')
        await press('RIGHT',90);snap('summary-source-stats')
        await press('A',90);snap('summary-source-ivs')
        await press('A',90);snap('summary-source-evs')
        await press('A',90);snap('summary-source-stats-return')
        await press('LEFT',90)
        for label in ('source-one','recipient','uninfected','neutral-recipient','uninfected-last'):
            await press('DOWN',90);snap('summary-'+label)
            await press('RIGHT',90);snap('summary-'+label+'-stats');await press('LEFT',90)
        await press('B',90);snap('summary-back-to-party')
        await press('B',90);snap('party-action-back-to-list')
        await press('B',90);snap('party-back-to-field')
        if not s.state[0]: await press('B',60)
        assert s.state[0], 'B did not return to field'
        await fixture(0,0,0xFE,3,8)
        await start_select(2);snap('party-pokerus-and-poison')
        await press('A');await press('A',150);snap('summary-pokerus-and-poison')
    finally:
        (OUT/'evidence.json').write_text(json.dumps(dict(build=s.build_info(),synthetic=True,captures=captures),indent=2))
        await s.core.close()
    print(OUT)
asyncio.run(main())
