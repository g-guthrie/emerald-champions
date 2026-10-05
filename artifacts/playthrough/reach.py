# reach.py - world reachability from the map data, one level per field ability.
# Geometry only: collision, elevation, water, ledges, HM obstacles, bike tiles, warps,
# connections, dive/emerge. Story gates are checked by hand separately.
# Usage (from the repo root): python3 ../reach.py  -> writes ../reach_levels.json
import json, re, struct, glob, os
from collections import deque

LEVELS = ['none', 'cut', 'rocksmash', 'strength', 'surf', 'dive', 'waterfall']
# bikes come from Mauville (Rydel) before Rock Smash; treated as part of 'rocksmash' and above
LEVEL_CAPS = {
    'none': set(), 'cut': {'cut'}, 'rocksmash': {'cut', 'smash', 'bike'},
    'strength': {'cut', 'smash', 'bike', 'strength'},
    'surf': {'cut', 'smash', 'bike', 'strength', 'surf'},
    'dive': {'cut', 'smash', 'bike', 'strength', 'surf', 'dive'},
    'waterfall': {'cut', 'smash', 'bike', 'strength', 'surf', 'dive', 'waterfall'},
}

import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from mbparse import parse_mb
MB = parse_mb()
tb = open('src/metatile_behavior.c').read()
SURF = {MB[n] for n in re.findall(r'\[(MB_\w+)\]\s*=[^,\n]*TILE_FLAG_SURFABLE', tb)}
DIVEABLE = {MB['MB_INTERIOR_DEEP_WATER'], MB['MB_DEEP_WATER'], MB['MB_SOOTOPOLIS_DEEP_WATER']}
NO_EMERGE = {MB['MB_NO_SURFACING'], MB['MB_SEAWEED_NO_SURFACING']}
JUMP = {MB['MB_JUMP_EAST']: (1, 0), MB['MB_JUMP_WEST']: (-1, 0), MB['MB_JUMP_NORTH']: (0, -1), MB['MB_JUMP_SOUTH']: (0, 1)}
BIKE_TILES = {MB[n] for n in ('MB_MUDDY_SLOPE', 'MB_BUMPY_SLOPE', 'MB_ISOLATED_VERTICAL_RAIL', 'MB_ISOLATED_HORIZONTAL_RAIL',
                               'MB_VERTICAL_RAIL', 'MB_HORIZONTAL_RAIL')}
WATERFALL = MB['MB_WATERFALL']
DOOR_ON_WALL = {MB['MB_ANIMATED_DOOR'], MB['MB_TRICK_HOUSE_PUZZLE_DOOR']}

src = open('src/data/tilesets/metatiles.h').read()
_attr_cache = {}
hdr = open('src/data/tilesets/headers.h').read()
ATTR_SYM = {ts: a for ts, a in re.findall(r'const struct Tileset (gTileset_\w+) =\s*\{.*?\.metatileAttributes = (\w+)', hdr, re.S)}
def attrs(ts):
    if ts not in _attr_cache:
        sym = ATTR_SYM[ts]
        path = re.search(sym + r'\[\] = INCBIN_U16\("([^"]+)"', src).group(1)
        d = open(path, 'rb').read()
        _attr_cache[ts] = [struct.unpack_from('<H', d, i)[0] & 0xFF for i in range(0, len(d), 2)]
    return _attr_cache[ts]

layouts = {l['id']: l for l in json.load(open('data/layouts/layouts.json'))['layouts'] if 'id' in l}
MAPS = {}
for f in glob.glob('data/maps/*/map.json'):
    m = json.load(open(f))
    MAPS[m['id']] = m
    m['_dir'] = f.split('/')[2]

# Layouts a map script swaps in (setmaplayoutindex): a tile counts as open if any variant opens it.
ALT_LAYOUTS = {}
for f in glob.glob('data/maps/*/scripts.inc'):
    for lay in re.findall(r'setmaplayoutindex (LAYOUT_\w+)', open(f).read()):
        ALT_LAYOUTS.setdefault(f.split('/')[2], set()).add(lay)

MT_IDS = {n: int(v, 0) for n, v in re.findall(r'#define (METATILE_\w+)\s+(0x[0-9A-Fa-f]+|\d+)', open('include/constants/metatile_labels.h').read())}
# setmetatile in a map's scripts: (x, y) -> (metatile id, impassable) - openings applied as a union.
SET_MT = {}
for f in glob.glob('data/maps/*/scripts.inc'):
    for x, y, name, c in re.findall(r'setmetatile (\d+), (\d+), (METATILE_\w+), (\w+)', open(f).read()):
        if name in MT_IDS:
            SET_MT.setdefault(f.split('/')[2], {}).setdefault((int(x), int(y)), []).append((MT_IDS[name], c in ('1', 'TRUE')))

class Grid:
    def __init__(self, m):
        lay = layouts[m['layout']]
        self.w, self.h = lay['width'], lay['height']
        self.b = open(lay['blockdata_filepath'], 'rb').read()
        for alt in sorted(ALT_LAYOUTS.get(m['_dir'], ())):
            al = layouts[alt]
            if (al['width'], al['height']) != (self.w, self.h) or al['primary_tileset'] != lay['primary_tileset'] or al['secondary_tileset'] != lay['secondary_tileset']:
                continue
            ab = open(al['blockdata_filepath'], 'rb').read()
            merged = bytearray(self.b)
            for i in range(0, len(merged), 2):
                v = struct.unpack_from('<H', merged, i)[0]
                a = struct.unpack_from('<H', ab, i)[0]
                # take the variant's tile where it opens a wall, or where it is an elevation
                # transition (0/15) that joins two levels the base layout keeps apart
                if not (a >> 10) & 3 and ((v >> 10) & 3 or (a >> 12) in (0, 15)):
                    struct.pack_into('<H', merged, i, a)
            self.b = bytes(merged)
        self.pa, self.sa = attrs(lay['primary_tileset']), attrs(lay['secondary_tileset'])
        self.mt_over = {}
        # only openings matter: a wall a script can open (passable) or turn into a door
        for p, opts in SET_MT.get(m['_dir'], {}).items():
            if not self.inb(*p) or not (self.blk(*p) >> 10) & 3: continue
            for mtid, imp in opts:
                if not imp:
                    self.mt_over[p] = mtid
                elif p not in self.mt_over and self._attr(mtid) in DOOR_ON_WALL:
                    self.mt_over[p] = ('door', mtid)
        self.under = m['_dir'].startswith('Underwater')
        self.obst = {}
        for o in m.get('object_events', []):
            g = str(o.get('graphics_id')); s = o.get('script')
            if s == 'EventScript_CutTree' or 'CUTTABLE_TREE' in g: self.obst[(o['x'], o['y'])] = 'cut'
            elif s == 'EventScript_RockSmash' or 'BREAKABLE_ROCK' in g: self.obst[(o['x'], o['y'])] = 'smash'
            elif s == 'EventScript_StrengthBoulder' or 'PUSHABLE_BOULDER' in g: self.obst[(o['x'], o['y'])] = 'strength'
    def _attr(self, t):
        a = self.pa if t < 512 else self.sa
        i = t if t < 512 else t - 512
        return a[i] if i < len(a) else 0
    def blk(self, x, y): return struct.unpack_from('<H', self.b, (y * self.w + x) * 2)[0]
    def coll(self, x, y):
        o = self.mt_over.get((x, y))
        if isinstance(o, int): return 0
        return (self.blk(x, y) >> 10) & 3
    def elev(self, x, y): return self.blk(x, y) >> 12
    def beh(self, x, y):
        o = self.mt_over.get((x, y))
        t = (o if isinstance(o, int) else o[1]) if o is not None else self.blk(x, y) & 0x3FF
        a = self.pa if t < 512 else self.sa
        i = t if t < 512 else t - 512
        return a[i] if i < len(a) else 0
    def inb(self, x, y): return 0 <= x < self.w and 0 <= y < self.h

GRIDS = {}
def grid(mid):
    if mid not in GRIDS: GRIDS[mid] = Grid(MAPS[mid])
    return GRIDS[mid]

def can_enter(g, a, n, d, caps):
    """Return the tile actually reached when stepping from a in direction d onto n, or None."""
    x, y = n
    if not g.inb(x, y): return None
    o = g.obst.get(n)
    if o and o not in caps: return None
    bn = g.beh(x, y)
    # the game checks ledge jumps before collision (CheckForObjectEventCollision), so a
    # ledge tile may carry collision and still be jumped; the player lands two tiles on
    if bn in JUMP and not g.under:
        if JUMP[bn] != d: return None
        l = (x + d[0], y + d[1])
        if not g.inb(*l) or (g.obst.get(l) and g.obst[l] not in caps): return None
        return l
    if g.coll(x, y): return None
    if g.under:
        return n
    if bn in BIKE_TILES and 'bike' not in caps: return None
    if bn == WATERFALL and 'waterfall' not in caps: return None
    ea, en = g.elev(*a), g.elev(x, y)
    wa, wn = g.beh(*a) in SURF, bn in SURF
    if wn and not wa:          # embark: needs Surf
        return n if 'surf' in caps else None
    if wa and not wn:          # disembark onto land
        return n if en in (0, 3, 15) else None
    if wa and wn: return n
    if ea == en or ea in (0, 15) or en in (0, 15): return n
    return None

# Scripted travel (boats, cable car, ferries, puzzle entrances): source map -> landing tile.
EDGES = [
    ('MAP_ROUTE104_MR_BRINEYS_HOUSE', 'MAP_DEWFORD_TOWN', (12, 9), 'briney'),
    ('MAP_DEWFORD_TOWN', 'MAP_ROUTE104_MR_BRINEYS_HOUSE', (5, 4), 'briney'),
    ('MAP_DEWFORD_TOWN', 'MAP_ROUTE109', (21, 24), 'briney109'),
    ('MAP_ROUTE109', 'MAP_DEWFORD_TOWN', (12, 9), 'briney109'),
    ('MAP_ROUTE112_CABLE_CAR_STATION', 'MAP_MT_CHIMNEY_CABLE_CAR_STATION', (6, 10), 'cablecar'),
    ('MAP_MT_CHIMNEY_CABLE_CAR_STATION', 'MAP_ROUTE112_CABLE_CAR_STATION', (6, 10), 'cablecar'),
    ('MAP_SLATEPORT_CITY_HARBOR', 'MAP_LILYCOVE_CITY_HARBOR', (11, 13), 'sstidal'),
    ('MAP_LILYCOVE_CITY_HARBOR', 'MAP_SLATEPORT_CITY_HARBOR', (11, 13), 'sstidal'),
    ('MAP_LILYCOVE_CITY_HARBOR', 'MAP_BATTLE_FRONTIER_OUTSIDE_WEST', None, 'frontier'),
    ('MAP_SLATEPORT_CITY_HARBOR', 'MAP_SS_TIDAL_CORRIDOR', None, 'sstidal'),
    ('MAP_LILYCOVE_CITY_HARBOR', 'MAP_SS_TIDAL_CORRIDOR', None, 'sstidal'),
    ('MAP_ROUTE121_SAFARI_ZONE_ENTRANCE', 'MAP_SAFARI_ZONE_SOUTH', (32, 32), 'safari'),
    ('MAP_SOOTOPOLIS_CITY', 'MAP_CAVE_OF_ORIGIN_ENTRANCE', (9, 19), 'origin'),
    ('MAP_SOOTOPOLIS_CITY_GYM_1F', 'MAP_SOOTOPOLIS_CITY_GYM_B1F', (11, 21), 'sootogym'),
] + [
    # setdivewarp map scripts (diving/surfacing without a map connection); need Dive
    ('MAP_ABANDONED_SHIP_CORRIDORS_B1F', 'MAP_ABANDONED_SHIP_UNDERWATER1', (5, 4), 'dive'),
    ('MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS', 'MAP_ABANDONED_SHIP_UNDERWATER1', (5, 4), 'dive'),
    ('MAP_ABANDONED_SHIP_ROOMS_B1F', 'MAP_ABANDONED_SHIP_UNDERWATER2', (17, 4), 'dive'),
    ('MAP_ABANDONED_SHIP_UNDERWATER1', 'MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS', (0, 10), 'dive'),
    ('MAP_ABANDONED_SHIP_UNDERWATER2', 'MAP_ABANDONED_SHIP_ROOMS_B1F', (13, 7), 'dive'),
    ('MAP_ROUTE134', 'MAP_UNDERWATER_ROUTE134', (8, 6), 'dive'),
    ('MAP_UNDERWATER_ROUTE134', 'MAP_ROUTE134', (60, 31), 'dive'),
    ('MAP_SEAFLOOR_CAVERN_ENTRANCE', 'MAP_UNDERWATER_SEAFLOOR_CAVERN', (6, 5), 'dive'),
    ('MAP_UNDERWATER_SEAFLOOR_CAVERN', 'MAP_SEAFLOOR_CAVERN_ENTRANCE', (10, 17), 'dive'),
    ('MAP_SEALED_CHAMBER_OUTER_ROOM', 'MAP_UNDERWATER_SEALED_CHAMBER', (12, 44), 'dive'),
    ('MAP_UNDERWATER_SEALED_CHAMBER', 'MAP_SEALED_CHAMBER_OUTER_ROOM', (10, 19), 'dive'),
    ('MAP_SOOTOPOLIS_CITY', 'MAP_UNDERWATER_SOOTOPOLIS_CITY', (9, 6), 'dive'),
    ('MAP_UNDERWATER_SOOTOPOLIS_CITY', 'MAP_SOOTOPOLIS_CITY', (29, 53), 'dive'),
] + [('MAP_ROUTE110_TRICK_HOUSE_ENTRANCE', f'MAP_ROUTE110_TRICK_HOUSE_PUZZLE{n}', None, f'trick{n}') for n in range(1, 9)]

def run(level, npc_block=False, edges=None):
    caps = LEVEL_CAPS[level]
    edges = {e[3] for e in EDGES} if edges is None else edges
    seen = {mid: set() for mid in MAPS}
    q = deque()
    def add(mid, p):
        if mid in MAPS and p not in seen[mid]:
            seen[mid].add(p); q.append((mid, p))
    start = MAPS['MAP_LITTLEROOT_TOWN']
    # the player starts outside the house after the truck: seed at the house door's front
    for w in start['warp_events']:
        if 'HOUSE' in w['dest_map']:
            add('MAP_LITTLEROOT_TOWN', (w['x'], w['y'] + 1))
            break
    warps_at = {}
    for mid, m in MAPS.items():
        for i, w in enumerate(m.get('warp_events', [])):
            warps_at.setdefault(mid, {})[(w['x'], w['y'])] = w
    fired = set()
    while True:
      while q:
          mid, p = q.popleft()
          m = MAPS[mid]; g = grid(mid)
          x, y = p
          for d in ((1, 0), (-1, 0), (0, 1), (0, -1)):
              n = (x + d[0], y + d[1])
              if g.inb(*n):
                  r = can_enter(g, p, n, d, caps)
                  if r: add(mid, r)
                  # walking into a warp tile (doors sit on collision tiles)
                  w = warps_at.get(mid, {}).get(n)
                  if w and g.coll(*n) and g.beh(*n) in DOOR_ON_WALL: take_warp(w, add)
              else:
                  # map connection
                  for c in m.get('connections') or []:
                      dirn, off, dest = c['direction'], c['offset'], c['map']
                      if dest not in MAPS: continue
                      dg = grid(dest)
                      if dirn == 'up' and d == (0, -1): t = (x - off, dg.h - 1)
                      elif dirn == 'down' and d == (0, 1): t = (x - off, 0)
                      elif dirn == 'left' and d == (-1, 0): t = (dg.w - 1, y - off)
                      elif dirn == 'right' and d == (1, 0): t = (0, y - off)
                      else: continue
                      if dg.inb(*t) and not dg.coll(*t) and (not dg.obst.get(t) or dg.obst[t] in caps):
                          if dg.beh(*t) in SURF and 'surf' not in caps and not dg.under: continue
                          add(dest, t)
          w = warps_at.get(mid, {}).get(p)
          if w: take_warp(w, add)
          if 'dive' in caps:
              for c in m.get('connections') or []:
                  if c['direction'] == 'dive' and g.beh(*p) in DIVEABLE and c['map'] in MAPS:
                      if grid(c['map']).inb(*p): add(c['map'], p)
                  if c['direction'] == 'emerge' and g.beh(*p) not in NO_EMERGE and c['map'] in MAPS:
                      dg = grid(c['map'])
                      if dg.inb(*p) and dg.beh(*p) in SURF: add(c['map'], p)
      new = False
      for a, b, t, tag in EDGES:
          if tag == 'dive' and 'dive' not in caps: continue
          if (tag == 'dive' or tag in edges) and seen.get(a) and b in MAPS and (a, b) not in fired:
              fired.add((a, b)); new = True
              if t is None:
                  w = MAPS[b]['warp_events'][0]; t = (w['x'], w['y'])
              add(b, t)
      if not new: break
    return seen

def take_warp(w, add):
    dest = w['dest_map']
    if dest not in MAPS: return
    dw = MAPS[dest].get('warp_events', [])
    try: i = int(w['dest_warp_id'])
    except ValueError: return
    if 0 <= i < len(dw): add(dest, (dw[i]['x'], dw[i]['y']))

if __name__ == '__main__':
    out = {}
    for lv in LEVELS:
        s = run(lv)
        out[lv] = {mid: sorted(map(list, v)) for mid, v in s.items() if v}
        print(lv, sum(1 for v in s.values() if v), 'maps reached')
    json.dump(out, open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'reach_levels.json'), 'w'))
