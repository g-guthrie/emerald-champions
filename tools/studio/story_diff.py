#!/usr/bin/env python3
"""Differential check: two builds must agree wherever the player has control.

usage: story_diff.py OLD_TREE NEW_TREE SCENE [SCENE...] [--maps Map,Map,...]

For each scene, both trees' headless ROMs boot that scene's end save (the tree's own
replay of the same recipe). At that idle moment the player's map, position, facing,
party and Bag are compared. Then each listed map is entered in both games and, on the
last frame before any scene script starts, every character (sprite, position, facing,
visibility, for those on screen), every object the map would spawn (sprite, position,
movement; from the live templates and the game's own hide decision), every trigger tile (would it fire now) and every hidden item are compared.
Everything is read from the running games (gEcStudioState, gEcStudioActors,
gEcStudioCensus, the native Bag query); nothing is computed here. Exit 1 on any difference.
"""
import argparse, asyncio, json, re, struct, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE)); sys.path.insert(0, str(HERE.parent.parent / 'scripts'))
import server
from scenes import packet_state
from native_tools import read_game_query

PLAYER_IDS = {255, 254}   # the player and the follower Pokemon


class Game:
    def __init__(self, tree):
        self.tree = Path(tree)
        self.rom, self.elf = self.tree / 'pokeemerald-headless.gba', self.tree / 'pokeemerald-headless.elf'
        header = (self.tree / 'include/emerald_champions_headless.h').read_text()
        self.scenarios = re.findall(r'EC_HEADLESS_SCENARIO_\w+', header.split('enum EmeraldChampionsHeadlessScenario')[1].split('};')[0])
        self.maps = {}
        for f in (self.tree / 'data/maps').glob('*/map.json'):
            m = json.load(open(f))
            self.maps[m['name']] = m
        groups = json.load(open(self.tree / 'data/maps/map_groups.json'))
        self.where = {}
        for g, gname in enumerate(groups['group_order']):
            for n, name in enumerate(groups[gname]):
                self.where[name] = (g, n)
        self.items = json.loads((self.tree / 'artifacts/playthrough/item_ids.json').read_text())

    async def boot(self, save):
        core = await server.Core.open(self.rom, self.elf, save)
        await core.tick(frames=60)
        await core.write([(core.syms['gEcHeadlessFixtureParam'], 3),
                          (core.syms['gEcHeadlessFixtureScenario'], self.scenarios.index('EC_HEADLESS_SCENARIO_STUDIO_RESUME'))])
        for _ in range(60):
            packet = await core.tick(frames=30)
            if packet_state(packet)['ready']:
                return core, packet
        await core.close()
        raise RuntimeError(f'{self.tree.name}: save did not reach an idle field: {save}')

    async def census(self, core):
        raw = await core.rpc(4, struct.pack('<II', core.syms['gEcStudioCensus'], 1024))
        n = raw[0]; at = 1; triggers = []
        for _ in range(n):
            triggers.append(tuple(raw[at:at + 4])); at += 4
        m = raw[at]; at += 1; bgs = []
        for _ in range(m):
            bgs.append(tuple(raw[at:at + 4])); at += 4
        k = raw[at]; at += 1; objects = []
        for _ in range(k):
            b = raw[at:at + 6]; at += 6
            objects.append((b[0] | b[1] << 8, b[2], b[3], b[4], b[5]))   # graphics, x, y, movement, hidden
        return sorted(triggers), sorted(bgs), sorted(o for o in objects if not o[4])


def actors(state):
    return sorted((a['graphic'], a['x'], a['y'], a['facing'], a['invisible'])
                  for a in state['actors'] if a['local_id'] not in PLAYER_IDS)


async def idle_record(game, save):
    core, packet = await game.boot(save)
    try:
        s = packet_state(packet)
        async def adv():
            await core.tick(frames=1)
        bag = {}
        for name in game.items:
            v = await read_game_query(core, 4, int(game_const(game, name)), adv)
            if v: bag[name] = v
        raw = await core.rpc(4, struct.pack('<II', core.syms['gEcStudioState'], 128))
        st = struct.unpack('<32I', raw)
        party = [tuple(st[12 + i * 3:15 + i * 3]) for i in range(6) if st[12 + i * 3]]
        # The story machine's own alarms (new builds only): refused story moves, walks
        # with no path. Any non-zero value fails, whatever the other game shows.
        alarms = {}
        for sym, size in (('gStoryOrderViolation', 4), ('gStoryPathFailures', 2)):
            if sym in core.syms:
                raw = await core.rpc(4, struct.pack('<II', core.syms[sym], 4))
                v = struct.unpack('<I', raw)[0] & (0xFFFFFFFF if size == 4 else 0xFFFF)
                if v: alarms[sym] = v
        return dict(map=(s['group'], s['num']), x=s['x'], y=s['y'], facing=s['facing'], party=party, bag=bag, alarms=alarms)
    finally:
        await core.close()


_CONSTANTS = None
def game_const(game, name):
    """Item ids from the Studio catalogue (both trees share include/constants/items.h)."""
    global _CONSTANTS
    if _CONSTANTS is None:
        _CONSTANTS = server.Catalogue().constants
    return _CONSTANTS[name]


async def map_record(game, save, mapname):
    core, packet = await game.boot(save)
    try:
        m = game.maps[mapname]
        g, n = game.where[mapname]
        w = (m.get('warp_events') or [{'x': 1, 'y': 1}])[0]
        await core.write([(core.syms['gEcStudioArgs'] + i * 4, v) for i, v in enumerate([g, n, w['x'], w['y'], 1])]
                         + [(core.syms['gEcStudioCommand'], 2)])
        last = None
        for frame in range(600):
            packet = await core.tick(frames=1)
            s = packet_state(packet)
            if (s['group'], s['num']) != (g, n):
                continue
            if s['script'] and last is not None:
                break          # a scene started: keep the frame before it
            last = (actors(s), await game.census(core))
            if s['ready']:
                break
        if last is None:
            raise RuntimeError(f'{game.tree.name}: never arrived on {mapname}')
        return dict(actors=last[0], triggers=last[1][0], hidden=last[1][1], objects=last[1][2])
    finally:
        await core.close()


def diff(label, a, b, out):
    if a != b:
        out.append(f'{label}:\n    old {a}\n    new {b}')


async def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('old'); ap.add_argument('new'); ap.add_argument('scenes', nargs='+')
    ap.add_argument('--maps', default='')
    ap.add_argument('--no-bag', action='store_true')
    a = ap.parse_args()
    old, new = Game(a.old), Game(a.new)
    maps = [x for x in a.maps.split(',') if x]
    report, failures = [], 0
    for scene in a.scenes:
        so = old.tree / 'work/studio/scenes' / scene / 'end.sav'
        sn = new.tree / 'work/studio/scenes' / scene / 'end.sav'
        out = []
        if not so.exists() or not sn.exists():
            out.append(f'missing end save (old {so.exists()}, new {sn.exists()})')
        else:
            if a.no_bag:
                old.items = new.items = []
            ro, rn = await idle_record(old, so), await idle_record(new, sn)
            for k in ('map', 'x', 'y', 'facing', 'party', 'bag'):
                diff(f'player {k}', ro[k], rn[k], out)
            for r, who in ((ro, 'old'), (rn, 'new')):
                if r['alarms']:
                    out.append(f"{who} game alarms: {r['alarms']}")
            for mapname in maps:
                mo, mn = await map_record(old, so, mapname), await map_record(new, sn, mapname)
                for k in ('objects', 'actors', 'triggers', 'hidden'):
                    diff(f'{mapname} {k}', mo[k], mn[k], out)
        failures += bool(out)
        report.append(f"== {scene}: {'SAME' if not out else str(len(out)) + ' difference(s)'}")
        report += ['  ' + line for line in out]
        print(report[-1 - len(out)] if not out else '\n'.join(report[-1 - len(out):]), flush=True)
    return 1 if failures else 0


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
