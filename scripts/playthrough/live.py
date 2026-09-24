#!/usr/bin/env python3
"""Persistent headless harness for an earned campaign save.

One native mGBA core (the Studio core) stays resident behind a Unix socket, so
every command costs milliseconds instead of a process boot. The player advances
only through ordinary buttons; trainer-battle decisions are answered through the
headless battle bridge (moves and switches only, the same menu choices a player
makes). Nothing writes flags, vars, party or bag. The only memory writes arm the
bridge's own EWRAM statics, exactly as EmeraldChampionsAgentBattleBegin would,
without reseeding RNG or touching difficulty.

    live.py serve --dir RUN --save FILE.sav      boot the save via native Continue
    live.py serve --dir RUN --resume             reload RUN/auto.ss1 (same ROM)
    live.py <op> [key=value ...]                 talk to the running daemon

Ops: status, shot, press, walk, goto, exit, advance, wait, battle, act, arm,
disarm, query, snap, load, export, party, stop.
"""
import argparse
import asyncio
import collections
import hashlib
import json
import os
import shutil
import socket
import struct
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
# The core links ffmpeg, which wants x265 4.1; hardened Python drops DYLD_* from
# its own environment, so restore it for the child here.
os.environ.setdefault('DYLD_LIBRARY_PATH', '/opt/homebrew/Cellar/x265/4.1/lib')
os.environ['PATH'] = str(Path.home() / '.local/share/arm-gnu-toolchain-15.2-20260718/Payload/bin') + ':' + os.environ['PATH']
sys.path.insert(0, str(ROOT / 'tools' / 'studio'))
sys.path.insert(0, str(ROOT / 'scripts'))
sys.path.insert(0, str(ROOT / 'scripts' / 'playthrough'))

DIRS = {'UP': (0, -1), 'DOWN': (0, 1), 'LEFT': (-1, 0), 'RIGHT': (1, 0)}
FACING = {1: 'DOWN', 2: 'UP', 3: 'LEFT', 4: 'RIGHT'}
BUTTONS = {"A": 1, "B": 2, "SELECT": 4, "START": 8, "RIGHT": 16, "LEFT": 32, "UP": 64,
           "DOWN": 128, "R": 256, "L": 512}


def mask_of(value):
    out = 0
    for key in str(value).replace('+', ' ').upper().split():
        out |= BUTTONS[key]
    return out


# ------------------------------------------------------------------ the daemon

class Harness:
    def __init__(self, run_dir):
        import server
        from scenes import TextDecoder
        import battle_driver as bd
        self.server, self.bd = server, bd
        self.dir = Path(run_dir).resolve()
        (self.dir / 'shots').mkdir(parents=True, exist_ok=True)
        (self.dir / 'snaps').mkdir(exist_ok=True)
        (self.dir / 'saves').mkdir(exist_ok=True)
        server.WORK = self.dir / 'runtime'
        server.WORK.mkdir(exist_ok=True)
        self.cat = server.Catalogue()
        self.decoder = TextDecoder(ROOT)
        self.core = None
        self.packet = b''
        self.st = {}
        self.blocked = collections.defaultdict(set)  # map -> tiles we bumped into
        self.shot_serial = 0
        self.log_path = self.dir / 'log.jsonl'
        cache = self.dir / 'constants.json'
        if cache.exists() and json.loads(cache.read_text()).get('schema') == bd.CONSTANTS_SCHEMA:
            self.constants = json.loads(cache.read_text())
        else:
            self.constants = bd.build_constants()
            cache.write_text(json.dumps(self.constants) + '\n')
        self.msg_since = 0

    # the decode_state() session protocol
    def constants_table(self):
        return self.constants

    # -------------------------------------------------------------- core I/O
    async def open(self, save=None, state=None):
        rom, elf = self.dir / 'scene.gba', self.dir / 'scene.elf'
        self.core = await self.server.Core.open(rom, elf, save)
        self.syms = self.core.syms
        if state is not None:
            await self.core.rpc(3, str(state).encode())
            self.ingest(await self.core.tick(frames=1))
            return
        header = (ROOT / 'include/emerald_champions_headless.h').read_text()
        import re
        enum = re.findall(r'EC_HEADLESS_SCENARIO_\w+',
                          header.split('enum EmeraldChampionsHeadlessScenario')[1].split('};')[0])
        await self.core.tick(frames=60)
        await self.core.write([(self.syms['gEcHeadlessFixtureParam'], 0),
                               (self.syms['gEcHeadlessFixtureScenario'],
                                enum.index('EC_HEADLESS_SCENARIO_STUDIO_RESUME'))])
        for _ in range(60):
            self.ingest(await self.core.tick(frames=30))
            if self.st['ready']:
                return
        raise RuntimeError('native Continue never reached an idle field')

    def ingest(self, packet):
        from scenes import packet_state
        self.packet = packet
        self.st = packet_state(packet, self.decoder)
        self.st['map'] = self.cat.map_ids.get((self.st['group'], self.st['num']), '?')
        count = struct.unpack_from('<I', packet, 12)[0]
        samples = struct.unpack_from('<I', packet, 8)[0]
        words = struct.unpack_from('<' + 'I' * count, packet, 153616 + samples * 4)
        self.st['party_words'] = words[12:30]

    async def tick(self, mask=0, frames=1):
        while frames > 0:
            n = min(frames, 120)
            self.ingest(await self.core.tick(mask, frames=n))
            frames -= n

    async def read(self, address, size):
        return await self.core.rpc(4, self.server.words(address, size))

    async def read_words(self, address, count):
        raw = await self.read(address, 4 * count)
        return list(struct.unpack('<' + 'I' * count, raw))

    async def write(self, pairs):
        await self.core.write(pairs)

    # --------------------------------------------------------------- bridge
    async def rmw_bytes(self, address, values):
        """Set individual bytes, preserving the rest of each aligned word."""
        by_word = collections.defaultdict(dict)
        for offset, value in values.items():
            by_word[(address + offset) & ~3][(address + offset) & 3] = value & 0xFF
        pairs = []
        for word_addr, changes in by_word.items():
            raw = bytearray(await self.read(word_addr, 4))
            for i, v in changes.items():
                raw[i] = v
            pairs.append((word_addr, struct.unpack('<I', bytes(raw))[0]))
        await self.write(pairs)

    async def arm(self):
        """Mirror EmeraldChampionsAgentBattleBegin's resets, minus RNG and difficulty."""
        if self.st.get('battle'):
            return False
        s = self.syms
        b = lambda name, size, value: {i: value for i in range(size)}
        await self.write([
            (s['gEcAgentBattlePhase'], 0), (s['gEcAgentBattleSerial'], 0),
            (s['gEcAgentBattleNeedMask'], 0), (s['gEcAgentBattleHalted'], 0),
            (s['gEcAgentBattleCommand'], 0),
            *[(s['gEcAgentBattleAction'] + 4 * i, 0) for i in range(4)],
            (s['sPendingSwitchSlot'], 0xFFFFFFFF), (s['sSwitchRefused'], 0),
            (s['sLastAction'], 0xFFFFFFFF), (s['sLastSelection'], 0),
            (s['sMessageSerial'], 0), (s['sAiDecisionFrames'], 0), (s['sAiSetupFrames'], 0),
            (s['sRevealed'], 0), (s['sRevealed'] + 4, 0),
        ])
        await self.rmw_bytes(s['sBridgeArmed'], {0: 1, 1: 0, 2: 0xFF, 3: 0xFF})  # armed, !seen, lastTurn
        await self.rmw_bytes(s['sAiDecisionLatched'], {0: 0})
        await self.rmw_bytes(s['sLevelCap'], {0: self.st.get('cap', 0), 1: 0, 2: 0, 3: 0})
        await self.rmw_bytes(s['sPlayerFaints'], {0: 0, 1: 0})
        await self.rmw_bytes(s['sFinalOutcome'], {0: 0})
        self.msg_since = 0
        return True

    async def disarm(self):
        await self.rmw_bytes(self.syms['sBridgeArmed'], {0: 0})

    async def armed(self):
        return (await self.read(self.syms['sBridgeArmed'], 1))[0] == 1

    async def view(self):
        return await self.read_words(self.syms['gEcAgentBattleView'], self.bd.VIEW_WORDS)

    async def halted(self):
        return (await self.read_words(self.syms['gEcAgentBattleHalted'], 1))[0]

    # --------------------------------------------------------------- status
    def party_brief(self):
        w = self.st['party_words']
        out = []
        for i in range(6):
            sp, lv, hp = w[3 * i:3 * i + 3]
            if sp:
                out.append(f"{self.cat.species.get(sp, sp)} L{lv} {hp}hp")
        return out

    def actors_brief(self):
        me = (self.st['x'], self.st['y'])
        out = []
        for a in self.st['actors']:
            if a['invisible'] or (a['x'], a['y']) == me:
                continue
            out.append(f"#{a['local_id']}@{a['x']},{a['y']}")
        return out

    def brief(self, extra=None):
        s = self.st
        out = {'map': s['map'], 'xy': [s['x'], s['y']], 'face': FACING.get(s['facing'], s['facing']),
               'ready': s['ready'], 'battle': s['battle'], 'script': s['script']}
        if s['text'] and not (s['ready'] and not s['script']):
            out['text'] = s['text'].replace('\n', ' ')[-240:]
        if extra:
            out.update(extra)
        return out

    async def shot(self, name=None, scale=2):
        from PIL import Image
        self.shot_serial += 1
        name = name or f'{int(time.time())}-{self.shot_serial:04d}'
        path = self.dir / 'shots' / f'{name}.png'
        img = Image.frombytes('RGBA', (240, 160), bytes(self.packet[16:153616])).convert('RGB')
        if scale != 1:
            img = img.resize((240 * scale, 160 * scale), Image.NEAREST)
        img.save(path)
        return str(path)

    # --------------------------------------------------------------- inputs
    async def press(self, keys, hold=1, wait=20, times=1):
        m = mask_of(keys)
        for _ in range(times):
            await self.tick(m, hold)
            await self.tick(0, wait)
        return self.brief()

    async def settle(self, limit=600):
        """Let a warp, jump or step finish; stop at ready, battle or a script."""
        for _ in range(limit // 4):
            if self.st['ready'] or self.st['battle']:
                return
            await self.tick(0, 4)

    async def step(self, direction, run=True):
        """One tile. Returns 'moved', 'blocked', or an interrupt reason."""
        start = (self.st['map'], self.st['x'], self.st['y'])
        m = mask_of(direction) | (BUTTONS['B'] if run else 0)
        moved = False
        for f in range(24):
            await self.tick(m, 1)
            if (self.st['map'], self.st['x'], self.st['y']) != start:
                moved = True
                break
            if self.st['battle'] or (self.st['script'] and f > 2):
                break
        await self.tick(0, 1)
        await self.settle()
        if self.st['battle']:
            return 'battle'
        if self.st['map'] != start[0]:
            return 'map'
        if self.st['script'] or not self.st['ready']:
            # A trainer's sight or a trigger took control; let it reach the text.
            for _ in range(60):
                if self.st['battle'] or self.st['text']:
                    break
                await self.tick(0, 4)
            return 'script'
        return 'moved' if moved else 'blocked'

    async def walk(self, direction, tiles=1, run=True):
        direction = direction.upper()
        done = 0
        reason = 'done'
        for _ in range(int(tiles)):
            r = await self.step(direction, run)
            if r == 'moved':
                done += 1
                continue
            reason = r
            if r in ('map',):
                done += 1
            break
        return self.brief({'walked': done, 'stop': reason})

    # --------------------------------------------------------------- paths
    def grid(self, name):
        m = self.cat.maps[name]
        layout = self.cat.layouts[m['layout']]
        data = (ROOT / layout['blockdata_filepath']).read_bytes()
        cells = struct.unpack('<' + 'H' * (len(data) // 2), data)
        return layout['width'], layout['height'], cells

    def trainer_of(self, label, depth=0):
        import re
        block = self.cat.blocks.get(label)
        if block is None or depth > 3:
            return None
        m = re.search(r'trainerbattle\w*\s+(TRAINER_\w+)', block[3])
        if m:
            return m[1]
        for token in re.findall(r'\b(?:goto|call)\w*\s+(\w+)', block[3]):
            found = self.trainer_of(token, depth + 1)
            if found:
                return found
        return None

    async def sight_tiles(self):
        """Tiles inside the sight line of every undefeated trainer on this map."""
        name = self.st['map']
        m = self.cat.maps.get(name)
        if m is None:
            return set()
        w, h, cells = self.grid(name)
        live = {a['local_id']: a for a in self.st['actors']}
        tiles = set()
        for index, o in enumerate(m['object_events']):
            if o.get('trainer_type', 'TRAINER_TYPE_NONE') == 'TRAINER_TYPE_NONE':
                continue
            trainer = self.trainer_of(o.get('script', ''))
            if trainer and trainer in self.cat.constants:
                flag = self.cat.constants['TRAINER_FLAGS_START'] + self.cat.constants[trainer]
                if await self.query(1, flag):
                    continue
            actor = live.get(index + 1)
            if actor is None:
                # Off-screen objects are not loaded; use the template, unless
                # its visibility flag hides it.
                hide = o.get('flag', '0')
                if hide not in ('0', 0, None, '') and hide in self.cat.constants \
                   and await self.query(1, self.cat.constants[hide]):
                    continue
                actor = {'x': o['x'], 'y': o['y'], 'facing': 0, 'invisible': False}
            if actor['invisible']:
                continue
            sight = int(o.get('trainer_sight_or_berry_tree_id') or 0)
            if sight <= 0:
                sight = 1
            movement = o.get('movement_type', '')
            facing = {1: (0, 1), 2: (0, -1), 3: (-1, 0), 4: (1, 0)}.get(actor['facing'])
            vec = {'UP': (0, -1), 'DOWN': (0, 1), 'LEFT': (-1, 0), 'RIGHT': (1, 0)}
            if movement.startswith('MOVEMENT_TYPE_FACE_') and '_AND_' in movement:
                names = movement[len('MOVEMENT_TYPE_FACE_'):].replace('_AND_', '_').split('_')
                dirs = [vec[n] for n in names if n in vec]
            elif movement.startswith('MOVEMENT_TYPE_FACE_') and facing:
                dirs = [facing]
            else:
                dirs = list(DIRS.values())
            for dx, dy in dirs:
                for k in range(1, sight + 1):
                    t = (actor['x'] + dx * k, actor['y'] + dy * k)
                    if not (0 <= t[0] < w and 0 <= t[1] < h) or cells[t[1] * w + t[0]] & 0xC00:
                        break
                    tiles.add(t)
        return tiles

    def plan(self, target, avoid=()):
        name = self.st['map']
        w, h, cells = self.grid(name)
        start = (self.st['x'], self.st['y'])
        blocked = set(self.blocked[name]) | set(avoid)
        for a in self.st['actors']:
            if not a['invisible'] and (a['x'], a['y']) != start:
                blocked.add((a['x'], a['y']))
        blocked.discard(target)
        q = collections.deque([start])
        seen = {start: None}
        while q:
            a = q.popleft()
            if a == target:
                break
            ae = cells[a[1] * w + a[0]] >> 12
            for d, (dx, dy) in DIRS.items():
                b = (a[0] + dx, a[1] + dy)
                if not (0 <= b[0] < w and 0 <= b[1] < h) or b in seen or b in blocked:
                    continue
                cell = cells[b[1] * w + b[0]]
                if cell & 0xC00 and b != target:
                    continue
                be = cell >> 12
                if ae and be and ae != be and ae != 15 and be != 15:
                    continue
                seen[b] = (a, d)
                q.append(b)
        if target not in seen:
            return None
        steps = []
        a = target
        while seen[a]:
            a, d = seen[a]
            steps.append(d)
        return steps[::-1]

    async def goto(self, x, y, run=True, replans=25, avoid_trainers=True):
        target = (int(x), int(y))
        origin_map = self.st['map']
        crossing = False
        for _ in range(replans):
            if (self.st['x'], self.st['y']) == target:
                warps = {(w['x'], w['y']) for w in self.cat.maps[origin_map]['warp_events']}
                if target in warps:
                    # Door mats and cave mouths warp on a push, not on arrival.
                    # Push toward a wall or the map edge first: walking there
                    # cannot move us off the warp unless the warp fires.
                    w, h, cells = self.grid(origin_map)
                    def walled(d):
                        t = (target[0] + DIRS[d][0], target[1] + DIRS[d][1])
                        return not (0 <= t[0] < w and 0 <= t[1] < h) or bool(cells[t[1] * w + t[0]] & 0xC00)
                    order = sorted(('DOWN', 'UP', 'LEFT', 'RIGHT'), key=lambda d: not walled(d))
                    for d in order:
                        r = await self.step(d, run)
                        if self.st['map'] != origin_map:
                            return self.brief({'stop': 'map'})
                        if (self.st['x'], self.st['y']) != target:
                            break
                return self.brief({'stop': 'arrived', 'crossed_sight': crossing})
            sight = await self.sight_tiles() if avoid_trainers else set()
            sight.discard(target)
            steps = self.plan(target, sight)
            if steps is None and sight:
                steps = self.plan(target)
                crossing = True
            if steps is None:
                return self.brief({'stop': 'no_path'})
            for d in steps:
                before = (self.st['x'], self.st['y'])
                r = await self.step(d, run)
                if r == 'blocked':
                    dx, dy = DIRS[d]
                    self.blocked[self.st['map']].add((before[0] + dx, before[1] + dy))
                    break
                if r != 'moved':
                    return self.brief({'stop': r})
                if (self.st['x'], self.st['y']) != (before[0] + DIRS[d][0], before[1] + DIRS[d][1]):
                    break  # ledge jump or slide; replan from here
            if self.st['map'] != origin_map:
                return self.brief({'stop': 'map'})
        return self.brief({'stop': 'gave_up', 'crossed_sight': crossing})

    async def exit(self, direction, run=True):
        """Leave the map by an edge: reach the nearest open edge tile, then keep walking."""
        direction = direction.upper()
        name = self.st['map']
        w, h, cells = self.grid(name)
        me = (self.st['x'], self.st['y'])
        if direction == 'LEFT':
            edge = [(0, y) for y in range(h)]
        elif direction == 'RIGHT':
            edge = [(w - 1, y) for y in range(h)]
        elif direction == 'UP':
            edge = [(x, 0) for x in range(w)]
        else:
            edge = [(x, h - 1) for x in range(w)]
        edge = [t for t in edge if not cells[t[1] * w + t[0]] & 0xC00]
        edge.sort(key=lambda t: abs(t[0] - me[0]) + abs(t[1] - me[1]))
        sight = await self.sight_tiles()
        edge = [t for t in edge if t not in sight]
        for t in edge[:12]:
            if self.plan(t, sight) is None:
                continue
            r = await self.goto(*t, run=run)
            if r.get('stop') != 'arrived':
                return r
            break
        for _ in range(4):
            r = await self.step(direction, run)
            if r != 'moved':
                break
        return self.brief({'stop': r})

    # --------------------------------------------------------------- dialogue
    async def advance(self, max_presses=80, wait=14, stop_on_question=True, key='A'):
        """Press A through text. Stop at idle, battle, a question, or a stall."""
        texts = []
        last_serial = self.st['text_serial']
        stall = 0
        if self.st['text']:
            texts.append(self.st['text'])
        for i in range(int(max_presses)):
            if self.st['battle']:
                return self.brief({'stop': 'battle', 'texts': texts[-12:]})
            if self.st['ready'] and not self.st['script']:
                # Twelve idle frames: a destination script may still start.
                await self.tick(0, 12)
                if self.st['ready'] and not self.st['script'] and not self.st['battle']:
                    return self.brief({'stop': 'idle', 'texts': texts[-12:]})
                continue
            cur = self.st['text'].rstrip()
            if stop_on_question and i > 0 and cur.endswith('?') and stall == 0 and self.st['script']:
                return self.brief({'stop': 'question', 'texts': texts[-12:]})
            await self.tick(mask_of(key), 2)
            await self.tick(0, wait)
            if self.st['text_serial'] != last_serial:
                last_serial = self.st['text_serial']
                stall = 0
                if self.st['text']:
                    texts.append(self.st['text'].replace('\n', ' '))
            else:
                # Heal machines, fades and walking cutscenes print nothing for a
                # while; only call it stuck after ~10 seconds of silence.
                stall += wait + 2
                if stall >= 600:
                    return self.brief({'stop': 'stalled', 'texts': texts[-12:],
                                       'shot': await self.shot()})
        return self.brief({'stop': 'max', 'texts': texts[-12:]})

    # --------------------------------------------------------------- battle
    def compact_battle(self, state):
        def mon(a):
            out = f"{a['species'].replace('SPECIES_', '')} L{a['level']} {a['hp']}/{a['max_hp']}"
            if a['status']:
                out += ' ' + ','.join(a['status'])
            stages = {k: v for k, v in a['stat_stages'].items() if v}
            if stages:
                out += ' ' + ' '.join(f'{k}{v:+d}' for k, v in stages.items())
            return out
        actives = {}
        for a in state['actives']:
            key = f"{a['battler']}{'P' if a['side'] == 'player' else 'O'}"
            if a['absent'] or not a['species'] or a['species'] == 'SPECIES_NONE':
                actives[key] = None
                continue
            entry = mon(a)
            if a['side'] == 'opponent':
                entry += f" [{a['ability'].replace('ABILITY_', '')}/{a['item'].replace('ITEM_', '')}]"
            actives[key] = entry
        pend = []
        for d in state['pending_decision']:
            moves = [f"move{m['index']}={m['move'].replace('MOVE_', '')}"
                     f"{'' if m['legal'] else '(X)'}@{m['targets']}" for m in d['moves']]
            pend.append({'battler': d['battler'], 'species': d.get('species'),
                         'replacing': d.get('replacing'), 'moves': moves,
                         'switch': d['switch_slots'] if (d['may_switch'] or d.get('replacing')) else []})
        reserves = [f"{p['slot']}:{p['species'].replace('SPECIES_', '')} {p['hp']}/{p['max_hp']}"
                    f"{' ' + ','.join(p['status']) if p['status'] else ''}"
                    for p in state['player_reserves']]
        foes = [f"{f['owner']}{f['slot']}:{f.get('species', '?').replace('SPECIES_', '')}"
                f"{' L' + str(f['level']) if 'level' in f else ''}{' X' if f['fainted'] else ''}"
                for f in state['opponent_party']]
        out = {'phase': state['phase'], 'turn': state['turn'], 'outcome': state['outcome'],
               'actives': actives, 'pending': pend, 'party': reserves, 'foes': foes}
        extras = {k: state[k] for k in ('weather', 'field') if state[k] != ['none']}
        if state['terrain'] != 'none':
            extras['terrain'] = state['terrain']
        sides = {k: v for k, v in state['sides'].items() if v}
        if sides:
            extras['sides'] = sides
        out.update(extras)
        return out

    async def battle_state(self, full=False):
        words = await self.view()
        state = self.bd.decode_state(self, words)
        if full:
            return state
        return self.compact_battle(state)

    async def run_battle(self, writes=(), limit=20000):
        """Advance with A pulses until the bridge wants a decision or the battle ends."""
        if writes:
            await self.write(list(writes) + [(self.syms['gEcAgentBattleHalted'], 0)])
        f = 0
        messages = []
        while f < limit:
            await self.tick(BUTTONS['A'] if (f // 10) % 2 == 0 else 0, 10)
            f += 10
            if not self.st['battle']:
                break
            if f >= 20 and await self.halted():
                words = await self.view()
                if words[8]:  # need mask
                    # The next battler's legality lands a few frames after
                    # the halt; let the publication finish before reading.
                    await self.tick(0, 4)
                    break
        words = await self.view()
        messages = self.bd.decode_messages(self, words, self.msg_since)
        self.msg_since = words[16]
        return messages, f

    async def act(self, commands):
        bd = self.bd
        for _ in range(4):
            words = await self.view()
            state = bd.decode_state(self, words)
            stale = any(not m['targets'] for e in state['pending_decision']
                        if e.get('awaiting_now') for m in e['moves'])
            if not stale:
                break
            await self.tick(0, 4)  # legality still publishing (or a fresh state load)
        if state['phase'] not in ('await_action', 'await_switch'):
            raise ValueError(f"not at a decision point (phase={state['phase']})")
        pending = {e['battler']: e for e in state['pending_decision']}
        writes, submitted = [], {}
        s = self.syms
        for text in commands:
            match = bd.COMMAND_RE.match(text.strip())
            if not match:
                raise ValueError(f'unparsable {text!r}; use 0:move1@3 or 2:switch3')
            battler = int(match[1])
            if battler not in pending:
                raise ValueError(f'battler {battler} not pending; pending={sorted(pending)}')
            entry = pending[battler]
            if match[5] is not None:
                slot = int(match[5])
                if slot not in entry['switch_slots']:
                    raise ValueError(f'battler {battler} cannot switch to {slot}; legal={entry["switch_slots"]}')
                if state['phase'] == 'await_action' and not entry['may_switch']:
                    raise ValueError(f'battler {battler} cannot switch now')
                if any(c.get('slot') == slot for c in submitted.values()):
                    raise ValueError('two battlers cannot switch to the same slot')
                writes += [(s['gEcAgentBattleSwitchSlot'] + 4 * battler, slot),
                           (s['gEcAgentBattleAction'] + 4 * battler, 2)]
                submitted[battler] = {'action': 'switch', 'slot': slot}
            else:
                index, target, mega = int(match[2]), int(match[3]), match[4] is not None
                if state['phase'] == 'await_switch':
                    raise ValueError(f'battler {battler} must switch')
                option = next((m for m in entry['moves'] if m['index'] == index), None)
                if option is None or not option['legal']:
                    raise ValueError(f'battler {battler} move{index} unavailable')
                # A battler the engine has not asked yet has no legality
                # published; its held command is checked by the engine itself.
                if option['targets'] and target not in option['targets']:
                    raise ValueError(f"move{index} ({option['move']}) targets {option['targets']}, not {target}")
                writes += [(s['gEcAgentBattleMoveIndex'] + 4 * battler, index),
                           (s['gEcAgentBattleTarget'] + 4 * battler, target),
                           (s['gEcAgentBattleMega'] + 4 * battler, 1 if mega else 0),
                           (s['gEcAgentBattleAction'] + 4 * battler, 1)]
                submitted[battler] = {'action': 'move', 'move': option['move'], 'target': target}
        required = {b for b, e in pending.items() if e.get('awaiting_now', True)}
        missing = sorted(required - set(submitted))
        if missing:
            raise ValueError(f'still need commands for {missing}')
        messages, frames = await self.run_battle(writes)
        out = {'messages': messages}
        if self.st['battle']:
            out.update(await self.battle_state())
        else:
            out['ended'] = True
            out.update(self.brief())
        return out

    # --------------------------------------------------------------- queries
    async def query(self, kind, ident):
        kinds = {'flag': 1, 'var': 2, 'item': 4, 'pcitem': 5, 'money': 9}
        k = kinds.get(kind, kind)
        if isinstance(ident, str) and not ident.lstrip('-').isdigit():
            ident = self.cat.constants[ident]
        await self.write([(self.syms['gEcHeadlessCampaignQueryKind'], int(k)),
                          (self.syms['gEcHeadlessCampaignQueryId'], int(ident))])
        await self.tick(0, 3)
        return (await self.read_words(self.syms['gEcHeadlessCampaignQueryValue'], 1))[0]

    async def party_detail(self):
        """The bridge publishes the full party in the field while armed."""
        if not await self.armed():
            await self.arm()
        await self.tick(0, 2)
        words = await self.view()
        state = self.bd.decode_state(self, words)
        out = []
        for p in state['player_reserves']:
            out.append(f"{p['slot']}:{p['species'].replace('SPECIES_', '')} L{p['level']} "
                       f"{p['hp']}/{p['max_hp']} {p['ability'].replace('ABILITY_', '')} "
                       f"@{p['item'].replace('ITEM_', '')} "
                       + '/'.join(m['move'].replace('MOVE_', '') for m in p['moves'])
                       + (' ' + ','.join(p['status']) if p['status'] else ''))
        return out

    # --------------------------------------------------------------- persistence
    async def snap(self, name='auto'):
        path = self.dir / 'snaps' / f'{name}.ss1'
        await self.core.rpc(2, str(path).encode())
        if name == 'auto':
            shutil.copy2(path, self.dir / 'auto.ss1')
        return str(path)

    async def load(self, name):
        path = self.dir / 'snaps' / f'{name}.ss1'
        await self.core.rpc(3, str(path).encode())
        await self.tick(0, 1)
        return self.brief()

    async def export(self, name=None):
        name = name or time.strftime('%Y%m%dT%H%M%S')
        path = self.dir / 'saves' / f'{name}.sav'
        await self.core.rpc(6, str(path).encode())
        return {'save': str(path), 'sha256': hashlib.sha256(path.read_bytes()).hexdigest()}

    # --------------------------------------------------------------- dispatch
    async def handle(self, req):
        op = req.pop('op')
        auto_arm = op in ('walk', 'goto', 'exit', 'advance', 'press', 'wait')
        if auto_arm and not self.st['battle'] and req.get('arm', '1') != '0':
            if not await self.armed() or self.st['ready']:
                await self.arm()
        req.pop('arm', None)
        if op == 'status':
            out = self.brief({'party': self.party_brief(), 'actors': self.actors_brief(),
                              'cap': self.st['cap'], 'difficulty': self.st['difficulty']})
            if req.get('shot'):
                out['shot'] = await self.shot()
        elif op == 'shot':
            out = {'shot': await self.shot(req.get('name'), int(req.get('scale', 2)))}
        elif op == 'press':
            out = await self.press(req.get('keys', 'A'), int(req.get('hold', 3)),
                                   int(req.get('wait', 20)), int(req.get('times', 1)))
        elif op == 'wait':
            await self.tick(0, int(req.get('frames', 60)))
            out = self.brief()
        elif op == 'walk':
            out = await self.walk(req['dir'], int(req.get('n', 1)), req.get('run', '1') != '0')
        elif op == 'goto':
            out = await self.goto(req['x'], req['y'], req.get('run', '1') != '0',
                                  avoid_trainers=req.get('avoid', '0') == '1')
        elif op == 'exit':
            out = await self.exit(req['dir'], req.get('run', '1') != '0')
        elif op == 'path':
            sight = await self.sight_tiles()
            out = {'path': self.plan((int(req['x']), int(req['y'])), sight), 'sight': sorted(sight)}
        elif op == 'advance':
            out = await self.advance(int(req.get('max', 80)), int(req.get('wait', 14)),
                                     req.get('q', '1') != '0', req.get('key', 'A'))
        elif op == 'battle':
            if req.get('run'):
                messages, _ = await self.run_battle()
                out = {'messages': messages}
                out.update(await self.battle_state() if self.st['battle'] else self.brief())
            else:
                out = await self.battle_state(full=bool(req.get('full')))
        elif op == 'act':
            out = await self.act(req['cmds'].split())
        elif op == 'arm':
            out = {'armed': await self.arm()}
        elif op == 'disarm':
            await self.disarm()
            out = {'armed': False}
        elif op == 'query':
            out = {'value': await self.query(req['kind'], req.get('id', 0))}
        elif op == 'party':
            out = {'party': await self.party_detail()}
        elif op == 'snap':
            out = {'snap': await self.snap(req.get('name', 'auto'))}
        elif op == 'load':
            out = await self.load(req['name'])
        elif op == 'export':
            out = await self.export(req.get('name'))
        elif op == 'warps':
            m = self.cat.maps[self.st['map']]
            out = {'warps': [f"{w['x']},{w['y']}->{w['dest_map'].replace('MAP_', '')}"
                             for w in m['warp_events']],
                   'connections': [f"{c['direction']}:{c['map'].replace('MAP_', '')}@{c['offset']}"
                                   for c in (m.get('connections') or [])]}
        elif op == 'forget':
            self.blocked[self.st['map']].clear()
            out = {'ok': True}
        else:
            raise ValueError(f'unknown op {op}')
        if op not in ('status', 'shot', 'battle', 'query', 'party', 'path', 'snap', 'load'):
            await self.snap('auto')
        if req.get('shot') and 'shot' not in out:
            out['shot'] = await self.shot()
        return out


async def serve(args):
    run_dir = Path(args.dir).resolve()
    run_dir.mkdir(parents=True, exist_ok=True)
    h = Harness(run_dir)
    for name in ('scene.gba', 'scene.elf'):
        target = run_dir / name
        source = Path(args.build) / ('pokeemerald-headless' + Path(name).suffix)
        if not target.exists():
            shutil.copy2(source, target)
    meta = run_dir / 'session.json'
    if not meta.exists():
        meta.write_text(json.dumps({
            'rom_sha256': hashlib.sha256((run_dir / 'scene.gba').read_bytes()).hexdigest(),
            'elf_sha256': hashlib.sha256((run_dir / 'scene.elf').read_bytes()).hexdigest(),
            'parent_save': str(Path(args.save).resolve()) if args.save else None,
            'parent_save_sha256': hashlib.sha256(Path(args.save).read_bytes()).hexdigest() if args.save else None,
            'scope': 'Earned campaign continued by native Continue on the headless build of the same '
                     'source; ordinary buttons; trainer decisions through the battle bridge (moves/'
                     'switches only). No flag, var, party or bag writes.',
        }, indent=2) + '\n')
    if args.resume:
        await h.open(save=None, state=run_dir / 'auto.ss1')
    else:
        await h.open(save=Path(args.save).resolve())
    sock = run_dir / 'live.sock'
    if sock.exists():
        sock.unlink()
    lock = asyncio.Lock()
    stop = asyncio.Event()

    async def client(reader, writer):
        try:
            req = json.loads(await reader.readline())
            async with lock:
                t0 = time.time()
                try:
                    if req.get('op') == 'stop':
                        await h.snap('auto')
                        out = {'stopped': True}
                        stop.set()
                    else:
                        out = await h.handle(dict(req))
                except Exception as e:  # report, keep serving
                    import traceback
                    out = {'error': str(e), 'trace': traceback.format_exc()[-800:]}
                with h.log_path.open('a') as f:
                    f.write(json.dumps({'t': round(t0, 2), 'req': req, 'secs': round(time.time() - t0, 2),
                                        'out': out}) + '\n')
            writer.write((json.dumps(out) + '\n').encode())
            await writer.drain()
        finally:
            writer.close()

    srv = await asyncio.start_unix_server(client, path=str(sock))
    print(json.dumps(h.brief({'socket': str(sock)})), flush=True)
    async with srv:
        await stop.wait()
    await h.core.close()


def call(run_dir, req):
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.connect(str(Path(run_dir) / 'live.sock'))
    s.sendall((json.dumps(req) + '\n').encode())
    data = b''
    while not data.endswith(b'\n'):
        chunk = s.recv(1 << 20)
        if not chunk:
            break
        data += chunk
    return json.loads(data)


def main():
    if len(sys.argv) > 1 and sys.argv[1] == 'serve':
        p = argparse.ArgumentParser()
        p.add_argument('cmd')
        p.add_argument('--dir', required=True)
        p.add_argument('--save')
        p.add_argument('--resume', action='store_true')
        p.add_argument('--build', default=str(ROOT))
        asyncio.run(serve(p.parse_args()))
        return
    run_dir = os.environ.get('LIVE_DIR')
    if not run_dir:
        raise SystemExit('set LIVE_DIR to the run directory')
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    req = {'op': sys.argv[1]}
    for arg in sys.argv[2:]:
        k, _, v = arg.partition('=')
        req[k] = v
    out = call(run_dir, req)
    print(json.dumps(out, separators=(',', ':')))


if __name__ == '__main__':
    main()
