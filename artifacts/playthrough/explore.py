"""Explore one map in the real game: photograph every reachable tile, talk to every NPC, check every
sign and hidden-item tile, and diff the Bag. Everything reported comes from the game's own readback.

usage (repo root): .venv-studio/bin/python artifacts/playthrough/explore.py MAP START_SAVE OUT_NAME
       [--caps cut,...] [--skip LOCALID,...]
Writes work/studio/scenes/<OUT_NAME>-NN/ chunks, then work/studio/explore/<OUT_NAME>/report.json,
report.md and mosaic.png. The last chunk's end.sav continues the playthrough.
"""
import argparse, json, os, re, subprocess, sys
from collections import deque
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
sys.path.insert(0, str(HERE))
import reach as R
import route_steps as RS

SHOP_LIKE = re.compile(r'pokemart|General_Mart_Script|BattleVendor|ChoosePartyMon|ShowScrollableMultichoice|'
                       r'multichoice|MoveTutor|Tutor|DoInGameTrade|CreateInGameTradePokemon|Lottery|NameRater|'
                       r'BufferEmeraldChampionsBattleItemStock|special ChooseMonForMoveRelearner|'
                       r'PlayerPC|BedroomPC|AccessPC|EventScript_PC\b|_PC::|WallClock|StartWallClock|'
                       r'checkmoney|removemoney')
SKIP_SCRIPTS = {'EventScript_CutTree', 'EventScript_RockSmash', 'EventScript_StrengthBoulder', '0x0', 'NULL', ''}
CHUNK = 6          # interactions/photos per scene (keeps each scene under the frame cap)


def script_graph_text(label, seen=None, depth=0):
    import glob
    if not hasattr(script_graph_text, 'labels'):
        labels = {}
        for f in glob.glob(str(ROOT / 'data/maps/*/scripts.inc')) + glob.glob(str(ROOT / 'data/scripts/*.inc')):
            cur = None
            for line in open(f, errors='ignore'):
                m = re.match(r'^(\w+)::', line)
                if m: cur = m.group(1); labels[cur] = []
                elif cur: labels[cur].append(line)
        script_graph_text.labels = labels
    seen = seen if seen is not None else set()
    if label in seen or depth > 5 or label not in script_graph_text.labels: return ''
    seen.add(label)
    body = ''.join(script_graph_text.labels[label])
    out = body
    for j in re.findall(r'\b(?:goto|call)\w*\s+(?:[^,\n]*,\s*)?(\w+)', body):
        out += script_graph_text(j, seen, depth + 1)
    return out


def mode_for(script):
    return 'B' if SHOP_LIKE.search(script + '\n' + script_graph_text(script)) else 'A'


def reachable(map_dir, start, caps, gates=frozenset()):
    mid = next(k for k, m in R.MAPS.items() if m['_dir'] == map_dir)
    g = R.grid(mid)
    wp = RS.warps(mid)   # a door, stair or mat leaves the map
    tp = RS.teleports(mid)
    # field obstacles are objects, not tiles: a Cut tree, smashable rock or Strength boulder is a
    # wall until the move that clears it
    need = {'OBJ_EVENT_GFX_CUTTABLE_TREE': 'cut', 'OBJ_EVENT_GFX_BREAKABLE_ROCK': 'smash',
            'OBJ_EVENT_GFX_PUSHABLE_BOULDER': 'strength'}
    walls = {(o['x'], o['y']) for o in R.MAPS[mid].get('object_events', [])
             if o.get('graphics_id') in need and need[o['graphics_id']] not in caps}
    seen = {start}; q = deque([start])
    while q:
        p = q.popleft()
        for d in RS.DIRS:
            r = R.can_enter(g, p, (p[0] + d[0], p[1] + d[1]), d, set(caps))
            if r and r in tp: r = tp[r]   # a teleport within the map lands on its pair
            if r and r not in seen and r not in wp and r not in gates and r not in walls:
                seen.add(r); q.append(r)
    return seen, g


def story_gates(map_dir, values):
    """Trigger tiles that are live now (their var holds the trigger's value) and whose script never
    changes that var: they fire every time and turn the player back (Petalburg's Gym escort), so the
    sweep walks around them. One-time scenes set their var and are walked into as usual."""
    m = next(mm for mm in R.MAPS.values() if mm['_dir'] == map_dir)
    gates = set()
    for c in m.get('coord_events', []) or []:
        var = c.get('var') or ''
        if c.get('type') != 'trigger' or var not in values: continue
        if values[var] != int(str(c.get('var_value', 0)), 0): continue
        if re.search(rf'\b(?:setvar|addvar|subvar|copyvar)\s+{var}\b', script_graph_text(c['script'])): continue
        gates.add((c['x'], c['y']))
    return gates


def trigger_vars(map_dir):
    m = next(mm for mm in R.MAPS.values() if mm['_dir'] == map_dir)
    return sorted({c['var'] for c in m.get('coord_events', []) or [] if (c.get('var') or '').startswith('VAR_')
                   and not c['var'].startswith('VAR_0x')})


def plan(map_dir, start, caps, skip, gates=frozenset(), live=None):
    """live: {local_id: (x, y)} where the game has actors right now; map scripts move some
    (Norman waits in the Petalburg Gym lobby), so these beat the map file's positions."""
    live = live or {}
    mid = next(k for k, m in R.MAPS.items() if m['_dir'] == map_dir)
    m = R.MAPS[mid]
    tiles, g = reachable(map_dir, start, caps, gates)
    # photo stops: greedy cover of every reachable tile with a 15x10 screen window
    uncovered = set(tiles); photos = []
    # a photo stop is a tile to stand on: never one an object occupies
    stands = (set(tiles) - {(o['x'], o['y']) for o in m.get('object_events', [])}
              - set(RS.teleports(mid))) or set(tiles)   # a teleport tile sends the player away
    while uncovered:
        best = max(stands, key=lambda t: sum(1 for u in uncovered if abs(u[0] - t[0]) <= 7 and abs(u[1] - t[1]) <= 4))
        photos.append(best)
        uncovered -= {u for u in uncovered if abs(u[0] - best[0]) <= 7 and abs(u[1] - best[1]) <= 4}
    jobs = [dict(kind='photo', at=p) for p in photos]
    for i, o in enumerate(m.get('object_events', [])):
        s = o.get('script') or ''
        if s in SKIP_SCRIPTS or (i + 1) in skip: continue
        jobs.append(dict(kind='talk', id=i + 1, at=live.get(i + 1, (o['x'], o['y'])), script=s, mode=mode_for(s)))
    for b in m.get('bg_events', []):
        if b.get('type') == 'hidden_item':
            jobs.append(dict(kind='inspect', at=(b['x'], b['y']), what='hidden ' + b['item'], mode='A'))
        elif b.get('script'):
            jobs.append(dict(kind='inspect', at=(b['x'], b['y']), what='sign ' + b['script'],
                             mode=mode_for(b['script'])))
    # targets with no reachable tile beside them (or across one counter) wait for a field move:
    # name the first level that brings them within reach
    def near(ts, at):
        # beside it, or two tiles straight across a counter (a clerk behind the desk)
        for dx, dy in ((0, 1), (0, -1), (1, 0), (-1, 0)):
            if (at[0] + dx, at[1] + dy) in ts: return True
            if (at[0] + 2 * dx, at[1] + 2 * dy) in ts and RS.is_counter(map_dir, (at[0] + dx, at[1] + dy)): return True
        return False
    later = []
    for j in [j for j in jobs if j['kind'] != 'photo' and not near(tiles, j['at'])]:
        jobs.remove(j)
        need = next((lv for lv in R.LEVELS[1:] if near(reachable(map_dir, start, R.LEVEL_CAPS[lv], gates)[0], j['at'])),
                    'story progress' if gates and near(reachable(map_dir, start, caps)[0], j['at']) else 'not from here')
        later.append(dict(label=to_step(j)['label'], source=j.get('script') or j.get('what'), needs=need))
    # nearest-neighbour order from the start, but never go somewhere the remaining jobs cannot be
    # reached from (a one-way ledge drops the player out of an area for good)
    def spot(j):
        if j['kind'] == 'photo': return tuple(j['at'])
        near_t = [t for t in tiles if abs(t[0] - j['at'][0]) + abs(t[1] - j['at'][1]) <= 2]
        return min(near_t, key=lambda t: abs(t[0] - j['at'][0]) + abs(t[1] - j['at'][1])) if near_t else tuple(j['at'])
    from_spot = {}
    def reach_from(p):
        if p not in from_spot: from_spot[p] = reachable(map_dir, p, caps, gates)[0]
        return from_spot[p]
    order = []; here = start; left = jobs[:]
    while left:
        ranked = sorted(left, key=lambda j: abs(j['at'][0] - here[0]) + abs(j['at'][1] - here[1]))
        j = next((c for c in ranked if all(spot(o) in reach_from(spot(c)) for o in left if o is not c)), ranked[0])
        order.append(j); left.remove(j); here = tuple(j['at'])
    return order, tiles, g, later


def to_step(j):
    if j['kind'] == 'photo':
        return {'walk_to': list(j['at']), 'label': f"photo {j['at'][0]},{j['at'][1]}"}
    if j['kind'] == 'talk':
        return {'talk_id': j['id'], 'mode': j['mode'], 'near': list(j['at']), 'label': f"talk {j['id']}"}
    return {'inspect': list(j['at']), 'mode': j['mode'], 'label': f"inspect {j['at'][0]},{j['at'][1]}"}


def run_chunk(recipe, out):
    path = ROOT / 'work/studio/recipes' / (out + '.json')
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(recipe, indent=1))
    dest = ROOT / 'work/studio/scenes' / out
    if dest.exists():
        subprocess.run(['chmod', '-R', 'u+w', str(dest)]); subprocess.run(['rm', '-rf', str(dest)])
    subprocess.run([str(ROOT / '.venv-studio/bin/python'), str(ROOT / 'tools/studio/run_scene.py'), str(path),
                    '--out', str(dest)], cwd=ROOT, capture_output=True, text=True, timeout=3600)
    res = dest / 'result.json'
    if not res.exists():
        raise SystemExit(f'chunk {out} crashed; see {dest}')
    return dest, json.loads(res.read_text())


def texts_between(rec, a, b):
    """Lines the game printed between frames a and b, minus the line already on screen at a."""
    last = ''
    for e in rec['events']:
        if e['frame'] >= a: break
        last = e['state']['text']
    seen = []
    for e in rec['events']:
        if a <= e['frame'] < b:
            t = e['state']['text']
            if t and t != last and t not in seen: seen.append(t)
            last = t
    return seen


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('map'); ap.add_argument('save'); ap.add_argument('name')
    ap.add_argument('--caps', default=''); ap.add_argument('--skip', default='')
    ap.add_argument('--start', default='', help='x,y if the save does not start on this map')
    a = ap.parse_args()
    caps = [c for c in a.caps.split(',') if c]
    skip = {int(s) for s in a.skip.split(',') if s}
    # find the start position from the save
    probe = {'name': 'probe ' + a.name, 'start': {'save': a.save}, 'steps': [{'frames': 2}],
             'queries': {v: {'kind': 2, 'id': v} for v in trigger_vars(a.map)}}
    _, pr = run_chunk(probe, a.name + '-probe')
    f = pr['outcome']['final']
    if f['map'] != a.map: raise SystemExit(f"save is on {f['map']}, not {a.map}")
    start = (f['x'], f['y'])
    gates = story_gates(a.map, f.get('queries', {}))
    live = {x['local_id']: (x['x'], x['y']) for x in f.get('actors', []) if x['local_id'] not in (255, 254) and not x['invisible']}  # player, follower Pokemon
    order, tiles, g, later = plan(a.map, start, caps, skip, gates, live)
    pending = list(order); save = a.save; results = []; dests = []; n = 0
    while pending:
        chunk, pending = pending[:CHUNK], pending[CHUNK:]
        steps = ([{'bag': 'before'}] if n == 0 else []) + [dict(to_step(j), map=a.map, avoid=sorted(gates)) for j in chunk]
        if not pending: steps.append({'bag': 'after'})
        recipe = {'name': f'{a.name} {n + 1}', 'start': {'save': save}, 'battle_resolution': 'fixture_win',
                  'steps': steps, 'expect': {'ready': True}}
        dest, res = run_chunk(recipe, f'{a.name}-{n + 1:02d}')
        n += 1
        dests.append(dest); results.append(res)
        fin = res['outcome']['final']
        if not fin.get('end_save'): raise SystemExit(f'chunk {n} did not end idle: {res["outcome"]["failures"]}')
        os.chmod(fin['end_save'], 0o444)
        save = fin['end_save']
        fails = res['outcome']['failures']
        if fails == ['frame budget']:
            # the scene ran out of frames (battles, Cut trees): carry the unstarted jobs over
            done = {i['label'] for i in fin.get('interactions', [])}
            done |= {m['label'] for m in json.loads((dest / 'recording.json').read_text())['markers']}
            left = [j for j in chunk if to_step(j)['label'] not in done]
            if len(left) == len(chunk): raise SystemExit(f'chunk {n} made no progress within the frame budget')
            pending = left + pending
            res['outcome']['failures'] = []
            continue
        if fails: break   # report what was done; the sweep stops on failures
    # report
    report = dict(map=a.map, start=list(start), reachable_tiles=len(tiles), chunks=[str(d) for d in dests],
                  end_save=save, failures=[], interactions=[], photos=[], later=later)
    for dest, res in zip(dests, results):
        rec = json.loads((dest / 'recording.json').read_text())
        report['failures'] += res['outcome']['failures']
        for it in res['outcome']['final']['interactions']:
            job = next((j for j in order if to_step(j)['label'] == it['label']), None)
            if job: it['source'] = job.get('script') or job.get('what')
            lo, hi = it.get('start', -1), it.get('end', -1)
            it['texts'] = texts_between(rec, lo, hi + 1) if it['status'] == 'visited' else []
            it['battle'] = any(e['state']['battle'] for e in rec['events'] if lo <= e['frame'] <= hi)
            report['interactions'].append(it)
        for c in rec['captures']:
            if c.get('label', '').startswith('photo'):
                report['photos'].append(dict(path=str(dest / c['path']), x=c['state']['x'], y=c['state']['y'],
                                             map=a.map))
    before = json.loads((dests[0] / 'bag-before.json').read_text())
    last_bag = dests[-1] / 'bag-after.json'
    if not last_bag.exists():   # stopped early: read the Bag where it stopped
        _, br = run_chunk({'name': a.name + ' bag', 'start': {'save': save}, 'steps': [{'bag': 'after'}]}, a.name + '-bag')
        last_bag = ROOT / 'work/studio/scenes' / (a.name + '-bag') / 'bag-after.json'
    after = json.loads(last_bag.read_text())
    report['bag_gained'] = {k: after.get(k, 0) - before.get(k, 0) for k in set(before) | set(after)
                            if after.get(k, 0) != before.get(k, 0)}
    out = ROOT / 'work/studio/explore' / a.name
    out.mkdir(parents=True, exist_ok=True)
    # mosaic: paste each photo at the camera position its reported tile implies (16 px per tile)
    from PIL import Image
    W, H = g.w * 16, g.h * 16
    canvas = Image.new('RGB', (W, H), (20, 20, 20))
    for ph in report['photos']:
        im = Image.open(ph['path']).convert('RGB')
        canvas.paste(im, (ph['x'] * 16 - 112, ph['y'] * 16 - 64))
    canvas.save(out / 'mosaic.png')
    (out / 'report.json').write_text(json.dumps(report, indent=1))
    lines = [f"# {a.map} ({a.name})", f"start {start}, reachable tiles {len(tiles)}, photos {len(report['photos'])}",
             f"failures: {report['failures'] or 'none'}", f"Bag gained: {report['bag_gained'] or 'nothing'}", '']
    for it in report['interactions']:
        t = ' | '.join(x.replace('\n', ' ') for x in it['texts'])[:400]
        lines.append(f"- {it['label']} {it.get('source','')} [{it['status']}{', battle' if it.get('battle') else ''}]: {t}")
    for it in later:
        lines.append(f"- {it['label']} {it['source']} [later: needs {it['needs']}]")
    (out / 'report.md').write_text('\n'.join(lines) + '\n')
    # Frames and sheets were only working material: the report holds what they showed.
    for dest, res in zip(dests, results):
        if res['outcome']['failures']: continue   # a failed chunk keeps its frames for diagnosis
        for f in dest.iterdir():
            if f.name not in ('end.sav', 'result.json', 'bag-before.json', 'bag-after.json'):
                if f.is_dir(): subprocess.run(['rm', '-rf', str(f)])
                else: f.unlink()
    print(out / 'report.md')


if __name__ == '__main__':
    main()
