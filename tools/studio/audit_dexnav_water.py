#!/usr/bin/env python3
"""A DexNav search on water, played to its encounter.

    .venv-studio/bin/python tools/studio/audit_dexnav_water.py work/studio/audits/dexnav-water

Warps onto Route 119's river already Surfing (the warp's "surf" option), opens
the DexNav, searches for its first Surfing Pokémon and sneaks to it. A water
search's Pokémon moves when the player draws near, so the hidden spot is read
back from the game after every step (sDexNavSearchDataPtr) and the route to it
is planned over the map's surfable tiles. Frames are saved to the output
directory; it passes when the search turns into a battle.
"""
import asyncio, struct, sys
from collections import deque
from pathlib import Path
from PIL import Image
sys.path.insert(0, str(Path(__file__).resolve().parent))
import server
from scenes import keys

MAP, NEAR = "Route119", (12, 88)
MAP_OFFSET = 7               # the game's map coordinates run 7 tiles in
SEARCH_TILE_OFFSET = 6       # struct DexNavSearch: species (u16), level, proximity, environment, pad, tileX, tileY
OUT = Path(sys.argv[1]).resolve(); OUT.mkdir(parents=True, exist_ok=True)
server.WORK = OUT; server.BUILD_STORE = OUT / "builds"


async def main():
    s = server.Studio(0); s.build = await s.stage(); s.build_id = s.build["id"]
    s.core, p = await s.boot(s.build, chapter=3); s.ingest(p)
    # Route 119's water Pokémon arrive with the sixth Badge.
    badges = {f"FLAG_BADGE0{i}_GET": True for i in range(1, 7)}
    await s.command(dict(op="setup", flags={"FLAG_SYS_POKEDEX_GET": True, "FLAG_RECEIVED_DEXNAV": True,
                                            "FLAG_RECEIVED_HM_SURF": True, **badges}))
    await s.command(dict(op="warp", map=MAP, x=NEAR[0], y=NEAR[1], facing=1, surf=True))

    async def tick(k=0, n=1):
        for _ in range(n): s.ingest(await s.core.tick(k, frames=1))

    def shot(name): Image.frombytes("RGBA", (240, 160), s.packet[16:153616]).convert("RGB").save(OUT / f"{name}.png")

    async def search_tile():
        ptr = struct.unpack("<I", await s.core.rpc(4, server.words(s.core.syms["sDexNavSearchDataPtr"], 4)))[0]
        if ptr == 0: return None
        x, y = struct.unpack_from("<hh", await s.core.rpc(4, server.words(ptr, 16)), SEARCH_TILE_OFFSET)
        return x - MAP_OFFSET, y - MAP_OFFSET

    water = s.cat.surf_tiles(MAP)

    def next_step(here, goal):
        prev, queue = {here: None}, deque([here])
        while queue:
            c = queue.popleft()
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                n = (c[0] + dx, c[1] + dy)
                if (n in water or n == goal) and n not in prev: prev[n] = c; queue.append(n)
        if goal not in prev: return None
        c = goal
        while prev[c] != here: c = prev[c]
        return c

    await tick(0, 120); shot("0-surfing")
    # Start menu: Pokédex, DexNav; then the Surfing row, three rows down on Route 119.
    for k, n in (("START", 60), ("DOWN", 20), ("A", 200), ("DOWN", 30), ("DOWN", 30), ("DOWN", 30)):
        await tick(keys(k), 1); await tick(0, n)
    shot("1-dexnav"); await tick(keys("A"), 1); await tick(0, 90); shot("2-searching")

    for i in range(40):
        goal = await search_tile()
        if goal is None or s.state[1]: break
        here = (s.state[4], s.state[5]); n = next_step(here, goal)
        if n is None: print("no water route from", here, "to", goal); break
        d = "RIGHT" if n[0] > here[0] else "LEFT" if n[0] < here[0] else "DOWN" if n[1] > here[1] else "UP"
        await tick(keys("A+" + d), 24); await tick(0, 40); shot(f"3-step{i:02d}")
        print("spot", goal, "player", (s.state[4], s.state[5]))
    await tick(0, 240); shot("4-end")
    battle = bool(s.state[1]); await s.core.close()
    print("PASS: the water search became a battle" if battle else "FAIL: no battle")
    return 0 if battle else 1

sys.exit(asyncio.run(main()))
