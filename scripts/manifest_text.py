#!/usr/bin/env python3
"""Readable battle-by-battle availability list for tuning teams.

Renders the progression that scripts/manifest_campaign.py writes
(progression.pickle) into one plain-text file. Every trainer battle gets a
number in the order it first becomes possible. Each step lists what newly
becomes available and which battles open with it; a team for battle N may use
everything up to and including N's step and nothing after it.

  python3 scripts/manifest_campaign.py --out work/progression-manifest --quick
  python3 scripts/manifest_text.py work/progression-manifest/progression.pickle \\
      --out artifacts/progression-manifest/availability-manifest.txt
"""
from __future__ import annotations

import argparse
import pickle
import re
import subprocess
from collections import defaultdict
from pathlib import Path

import emerald_champions_teams as teams
from export_trainer_catalogue import difficulty_level_rules
from manifest_campaign import battle_items
from reference_pool import ROOT, SpeciesData

WIDTH = 100

# Forms that exist only inside a battle; the owned Pokemon is the base form.
BATTLE_ONLY = ('_MEGA', '_PRIMAL', '_GMAX', '_ULTRA', '_ETERNAMAX', '_CROWNED')
# Non-campaign source kinds the progression keeps for completeness.
SKIP_ITEM_KINDS = {'wild_held', 'pickup', 'theft'}

KIND_LABEL = {
    'wild': 'Wild', 'storm_visitor': 'Storm visitor', 'static': 'Static encounter',
    'gift': 'Gift', 'gift_egg': 'Gift egg', 'egg': 'Gift egg', 'trade': 'In-game trade',
    'starter': 'Starter', 'fossil': 'Fossil', 'game_corner': 'Game Corner',
    'evolution': 'Evolve', 'breeding': 'Breed', 'form': 'Form change',
    'vendor_species': 'Vendor', 'cut_tree': 'Cut-tree habitat', 'feebas': 'Feebas tiles',
}


def place(token) -> str:
    if token is None:
        return ''
    if isinstance(token, (list, tuple)):
        token = token[0]
    token = str(token)
    token = re.sub(r'(?<=[a-z])(?=[A-Z0-9])|(?<=[0-9])(?=[A-Z][a-z])', ' ', token.replace('_', ' '))
    token = re.sub(r'\bRoute(\d)', r'Route \1', token)
    return re.sub(r'\s+', ' ', token).strip()


def pretty(token: str) -> str:
    return re.sub(r'^(SPECIES|ITEM|TRAINER)_', '', token).replace('_', ' ').title()


def wrap(prefix: str, words: list[str], indent: int = 4) -> list[str]:
    """Comma-separated list wrapped to WIDTH with a hanging indent."""
    lines, line = [], prefix
    for i, word in enumerate(words):
        piece = word + (', ' if i < len(words) - 1 else '')
        if len(line) + len(piece.rstrip()) > WIDTH and line.strip():
            lines.append(line.rstrip())
            line = ' ' * indent
        line += piece
    lines.append(line.rstrip())
    return lines


def trainer_names() -> dict[str, str]:
    text = (ROOT / 'src/data/trainers.party').read_text()
    names = {}
    for block in re.split(r'^=== ', text, flags=re.M)[1:]:
        key = block.split(' ===', 1)[0].strip()
        name = re.search(r'^Name:\s*(.*)$', block, re.M)
        cls = re.search(r'^Class:\s*(.*)$', block, re.M)
        if name:
            names[key] = ' '.join(x for x in ((cls[1] if cls else ''), name[1]) if x).strip()
    return names


class Renderer:
    def __init__(self, manifest: dict):
        self.m = manifest
        self.sd = SpeciesData()
        self.held = battle_items() | {"ITEM_MASTER_BALL"}
        self.names = trainer_names()
        self.nodes = {n['site_id']: n for n in manifest['battles']}
        self.branches = {b.trainer: b for b in teams.read_teams()}
        self.prows = manifest['pokemon_sources']
        self.irows = manifest['item_sources']

    # ---- naming -------------------------------------------------------
    def species(self, sp: str) -> str:
        name = self.sd.name(sp)
        return name + (' [R]' if self.sd.restricted_class(sp) else '')

    def battle_title(self, node: dict) -> str:
        if node.get('display_name'):
            return node['display_name']
        if node.get('trainers'):
            parts = []
            for t in node['trainers']:
                n = self.names.get(t) or pretty(t)
                if n not in parts:
                    parts.append(n)
            return ' + '.join(parts)
        if node.get('wild_species'):
            return 'Wild ' + ' + '.join(self.species(s) for s in node['wild_species'])
        return node.get('label') or node['site_id']

    def group_key(self, node: dict):
        enc = sorted({self.branches[t].encounter for t in node.get('trainers', []) if t in self.branches})
        return ('E', *enc) if enc else ('S', node['site_id'])

    # ---- sources ------------------------------------------------------
    def species_how(self, sp, new_rows, derivations, known):
        rows = [self.prows[i] for i in new_rows if self.prows[i]['species'] == sp]
        if rows:
            r = rows[0]
            kind = KIND_LABEL.get(r['kind'], r['kind'].replace('_', ' ').capitalize())
            return kind, place(r.get('where'))
        d = derivations.get(sp)
        if d:
            kind = KIND_LABEL.get(d.get('kind'), str(d.get('kind', 'other')).replace('_', ' ').capitalize())
            src = d.get('species')
            frm = f'from {self.sd.name(src)}' if src and src != sp else ''
            return kind, frm
        # The progression keeps only its last derivation pass; recover the
        # route from species data and what was already available.
        pre = sorted(x for x in self.sd.pre_evo.get(sp, ()) if x in known)
        if pre:
            return 'Evolve', f'from {self.sd.name(pre[0])}'
        kids = sorted(x for x in self.sd.family(sp) if x in known and x != sp)
        if kids and self.sd.egg_species(kids[0]) == sp:
            return 'Breed', f'egg from {self.sd.name(kids[0])}'
        if kids:
            return 'Form change', f'from {self.sd.name(kids[0])}'
        forms = sorted(x for x in known if x != sp and self.sd.base(x) == self.sd.base(sp))
        if forms:
            return 'Form change', f'from {self.sd.name(forms[0])}'
        return 'Other', ''

    def item_how(self, item, new_rows):
        rows = [self.irows[i] for i in new_rows if self.irows[i].get('key') == item
                and self.irows[i].get('kind') not in SKIP_ITEM_KINDS]
        if not rows:
            return ''
        r = rows[0]
        where = place(r.get('where'))
        kind = r.get('kind', '')
        if kind == 'mart' or kind.startswith('vendor'):
            return f'buy, {where}' if where else 'buy'
        if kind in ('berry_tree', 'berry_soil'):
            return f'berry tree, {where}' if where else 'berry tree'
        return where or kind.replace('_', ' ')

    # ---- steps --------------------------------------------------------
    def raw_steps(self):
        """Linear sequence: each frontier, then each victory inside it."""
        for i, period in enumerate(self.m['periods']):
            yield dict(after=None, new=period['new'], battles=period['battles'],
                       cap=period['state']['cap'], rows=period['source_rows'])
            for av in period['after_victory']:
                yield dict(after=av['battle'], new=av['new'], battles=av['next_battles'],
                           cap=av['state']['cap'], rows=av['source_rows'])

    def build(self):
        known_species, known_items, known_battles, known_groups = set(), set(), set(), {}
        seen_p, seen_i = set(), set()
        steps, pending = [], None
        numbers = {}
        self.ring_seen = False
        for raw in self.raw_steps():
            rows = raw['rows'] or {}
            new_p = set(rows.get('pokemon_sources', [])) - seen_p
            new_i = set(rows.get('item_sources', [])) - seen_i
            seen_p |= new_p
            seen_i |= new_i
            deriv = (rows.get('derivations') or {}).get('species', {}) or {}
            if pending is None:
                pending = dict(after=[], species={}, items={}, pickups=[], battles=[], cap=raw['cap'])
            contributed = False
            known_before = set(known_species) | set(raw['new']['pokemon'])
            for sp in raw['new']['pokemon']:
                if sp in known_species:
                    continue
                known_species.add(sp)
                contributed = True
                pending['species'][sp] = self.species_how(sp, new_p, deriv, known_before)
            for it in raw['new']['items']:
                # The requested Ball exception is its finite field pickups;
                # lottery offers remain conditional in the source appendix.
                if it == 'ITEM_MASTER_BALL':
                    continue
                if it == 'ITEM_MEGA_RING' and not pending.get('ring') and not self.ring_seen:
                    pending['ring'] = contributed = self.ring_seen = True
                if it in known_items or it not in self.held:
                    continue
                contributed = True
                known_items.add(it)
                pending['items'][it] = self.item_how(it, new_i)
            # Finite Master Ball sources matter even after the lottery has
            # made the item theoretically obtainable; retain each pickup.
            for index in sorted(new_i):
                row=self.irows[index]
                if row.get('key')=='ITEM_MASTER_BALL' and row.get('kind')=='item_ball':
                    pending['pickups'].append(place(row.get('where')))
                    if 'ITEM_MASTER_BALL' not in known_items:
                        known_items.add('ITEM_MASTER_BALL')
                        pending['items']['ITEM_MASTER_BALL']=place(row.get('where'))
                    contributed=True
            pending['cap'] = raw['cap']
            for site in raw['battles']:
                if site in known_battles or site not in self.nodes:
                    continue
                known_battles.add(site)
                node = self.nodes[site]
                key = self.group_key(node)
                if key in known_groups:
                    known_groups[key]['variants'].append(node)
                    continue
                entry = dict(node=node, variants=[node], cap=raw['cap'])
                known_groups[key] = entry
                pending['battles'].append(entry)
                contributed = True
            # Name only the victories that actually opened something.
            if raw['after'] and contributed:
                pending['after'].append(raw['after'])
            if pending['battles']:
                steps.append(pending)
                pending = None
        if pending and (pending['species'] or pending['items'] or pending['pickups']):
            steps.append(pending)
        # Number battles in step order, story order within a step.
        n = 0
        for step in steps:
            step['battles'].sort(key=lambda e: (min((self.branches[t].encounter for t in e['node'].get('trainers', [])
                                                     if t in self.branches), default=9999), e['node']['site_id']))
            for entry in step['battles']:
                n += 1
                entry['number'] = n
                numbers[entry['node']['site_id']] = n
                for v in entry['variants']:
                    numbers[v['site_id']] = n
        self.numbers = numbers
        return steps

    # ---- output -------------------------------------------------------
    def levels(self, entry, cap):
        rules = difficulty_level_rules()
        team = []
        for t in entry['node'].get('trainers', []):
            b = self.branches.get(t)
            if not b:
                continue
            for mon in b.mons:
                team.append((mon.offset, b.cls in ('regular', 'casual')))
        if not team:
            return ''
        out = []
        for label, mode in (('Easy', 'DIFFICULTY_EASY'), ('Med', 'DIFFICULTY_NORMAL'), ('Hard', 'DIFFICULTY_HARD')):
            lp, dp = rules[mode]
            vals = []
            for offset, minus in team:
                lead = offset + 2
                if lead > 0:
                    lead = (lead * lp + 50) // 100
                vals.append(max(1, min(100, cap + lead - (cap * dp + 50) // 100) - minus))
            lo, hi = min(vals), max(vals)
            out.append(f'{label} {lo}' if lo == hi else f'{label} {lo}-{hi}')
        return ' / '.join(out)

    def render(self) -> str:
        steps = self.build()
        head = subprocess.run(['git', 'rev-parse', '--short', 'HEAD'], cwd=ROOT, text=True,
                              capture_output=True).stdout.strip()
        finale = self.m.get('finale_reached')
        last = steps[-1]['battles'][-1]['number'] if steps and steps[-1]['battles'] else 0
        L = []
        L.append('INCLEMENT EMERALD 2 - WHAT A PLAYER CAN HAVE BEFORE EACH BATTLE')
        L.append(f'Generated from source at {head} by scripts/manifest_text.py. {last} battles, '
                 + ('through Buffel.' if finale else 'PARTIAL: the progression did not reach Buffel.'))
        L.append('')
        L.append('HOW TO USE')
        for line in (
            'Battles are numbered in the order they first become possible. A team for battle N may use',
            'everything listed in every step up to and including the step that opens battle N - nothing later.',
            'An optional fight taken later may also use what opened in between, but tune it at its first point.',
            'All your Pokemon are at the step\'s level cap (Leveler). Evolutions still need their own condition',
            '(level, stone, friendship, trade, time, move) and the item or move must already be available.',
            '[R] = Restricted: Legendary, Mythical, Ultra Beast, Paradox, Gholdengo or either Ursaluna; one total per party.',
            'Starters: choose one region, then take up to two of its three starters. This reference follows',
            'Treecko + Mudkip; other starter choices swap those two lines. The rival keeps the third starter.',
            'Items are held battle items only (Mega Stones are listed on their own line). Key items, medicine,',
            'Poke Balls other than the Master Ball, evolution stones and decorations are left out. Wild held items, stolen items and',
            'Pickup finds are excluded. Master Ball entries are finite field pickups; lottery outcomes remain',
            'in the conditional source appendix. Prices and one-off pickups still apply.',
            'Foe levels are for the battle\'s first possible point: Easy / Medium / Hard.',
        ):
            L.append('  ' + line)
        L.append('')
        ring = False
        for index, step in enumerate(steps, 1):
            L.append('=' * WIDTH)
            after = [self.battle_title(self.nodes[s]) + (f' (#{self.numbers[s]})' if s in self.numbers else '')
                     for s in step['after'] if s in self.nodes]
            title = f'STEP {index} - level cap {step["cap"]}'
            L.append(title)
            if after:
                L.extend(wrap('  After beating: ', after))
            elif index == 1:
                L.append('  Start of the game')
            species = step['species']
            megas = sorted(sp for sp in species if any(t in sp for t in BATTLE_ONLY))
            owned = {sp: how for sp, how in species.items() if sp not in megas}
            if owned:
                L.append(f'  New Pokemon ({len(owned)}):')
                groups = defaultdict(list)
                derived = ('Evolve', 'Form change', 'Breed')
                for sp, (kind, where) in owned.items():
                    groups[(kind, '' if kind in derived else where)].append((sp, where if kind in derived else ''))
                order = ['Starter', 'Wild', 'Storm visitor', 'Static encounter', 'Gift', 'Gift egg',
                         'In-game trade', 'Fossil', 'Game Corner', 'Vendor', 'Evolve', 'Form change', 'Breed']
                for (kind, where) in sorted(groups, key=lambda k: (order.index(k[0]) if k[0] in order else 99, k[1])):
                    entries = sorted(groups[(kind, where)], key=lambda x: self.sd.name(x[0]))
                    label = f'    {kind}' + (f' - {where}' if where else '') + ': '
                    if kind in ('Evolve', 'Form change', 'Breed'):
                        words = [self.species(sp) + (f' ({frm})' if frm else '') for sp, frm in entries]
                    else:
                        words = [self.species(sp) + (f' ({w})' if w else '') for sp, w in entries]
                    L.extend(wrap(label, words, 8))
            stones = sorted(it for it in step['items'] if self.is_mega_stone(it))
            held = {it: how for it, how in step['items'].items() if it not in stones and it != 'ITEM_MASTER_BALL'}
            for where in step['pickups']:
                L.append('  Master Ball pickup: ' + where)
            if held:
                L.extend(wrap(f'  New held items ({len(held)}): ',
                              [pretty(it) + (f' ({how})' if how else '') for it, how in sorted(held.items())], 6))
            if step.get('ring'):
                L.append('  Mega Ring: Mega Evolution is now usable with any Mega Stone listed so far.')
                ring = True
            if stones:
                note = '' if ring else ' (no Mega Ring yet)'
                L.extend(wrap(f'  New Mega Stones{note}: ', [pretty(it) for it in stones], 6))
            L.append(f'  Battles that open here:')
            for entry in step['battles']:
                node = entry['node']
                where = sorted({place(l['map']) for l in node.get('locations', []) if l.get('map')})
                line = f'    #{entry["number"]} {self.battle_title(node)}'
                if where:
                    line += ' - ' + ', '.join(where)
                L.append(line)
                lv = self.levels(entry, step['cap'])
                if lv:
                    L.append(f'        foe levels {lv}')
            L.append('')
        return '\n'.join(L) + '\n'

    def is_mega_stone(self, item: str) -> bool:
        if not hasattr(self, '_stones'):
            forms = (ROOT / 'src/data/pokemon/form_change_tables.h').read_text()
            self._stones = set(re.findall(
                r'FORM_CHANGE_BATTLE_(?:MEGA_EVOLUTION_ITEM|PRIMAL_REVERSION)\s*,\s*SPECIES_\w+\s*,\s*(ITEM_\w+)', forms))
        return item in self._stones


def resources_before(manifest: dict, trainer: str) -> dict:
    """What a team for this trainer's battle may use: everything up to and
    including the step that first opens the battle."""
    r = Renderer(manifest)
    species, items = set(), set()
    for step in r.build():
        species.update(step['species'])
        items.update(step['items'])
        for entry in step['battles']:
            if any(trainer in v.get('trainers', []) for v in entry['variants']):
                return dict(number=entry['number'], cap=step['cap'], species=species, items=items)
    raise ValueError(f'{trainer} is not placed in the progression')


def main():
    p = argparse.ArgumentParser()
    p.add_argument('progression', type=Path)
    p.add_argument('--out', type=Path, required=True)
    args = p.parse_args()
    manifest = pickle.loads(args.progression.read_bytes())
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(Renderer(manifest).render())
    print('wrote', args.out)


if __name__ == '__main__':
    main()
