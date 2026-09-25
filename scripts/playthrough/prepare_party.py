"""Bounded, user-authorized party preparation through the native fixture API.

Availability is an authored source audit, not inferred from a species number.
This module never writes progress, trainer results, money or arbitrary memory.
"""
import json
import re
import shutil
import subprocess
import tempfile
from pathlib import Path

KEEP = 0xffffffff
# enum EmeraldChampionsAgentPrepResult (include/emerald_champions_agent_prep.h),
# as the reason a caller reports when the native preparation refuses a party.
PREP_RESULTS = {
    0: 'pending (the ROM never read the preparation command)',
    2: 'bad command',
    3: 'called during a battle',
    4: 'bad party size (prepare one to six Pokemon)',
    5: 'unknown species',
    6: 'bad level (the level must be the campaign cap for that species)',
    7: 'bad preset',
    8: 'illegal move for this species',
    9: 'bad nature',
    10: 'ability not available to this species',
    11: 'bad held item',
    12: 'bad EV spread',
    13: ('restricted party: the game allows one Legendary/Mythical, one Ultra Beast and one '
         'Paradox Pokemon per party, and this is a second one of its class'),
}


def protocol(path, root):
    spec = json.loads(path.read_text())
    if not spec.get('availability_audit') or not spec.get('encounter'):
        raise ValueError('Preparation requires an encounter and reachable-pool audit.')
    party = spec['party']
    if not 1 <= len(party) <= 6:
        raise ValueError('Prepare one to six Pokemon.')
    names = set()
    for mon in party:
        if not mon.get('availability') or not mon.get('role'):
            raise ValueError('Every member needs acquisition evidence and a role.')
        for key, prefix in [('species', 'SPECIES_'), ('nature', 'NATURE_'),
                            ('ability', 'ABILITY_'), ('item', 'ITEM_')]:
            value = mon[key]
            if not re.fullmatch(prefix + r'[A-Z0-9_]+', value):
                raise ValueError(f'Invalid {key} constant: {value}')
            names.add(value)
        if len(mon['moves']) != 4 or len(set(mon['moves'])) != 4:
            raise ValueError('Supply four distinct legal moves.')
        for move in mon['moves']:
            if not re.fullmatch(r'MOVE_[A-Z0-9_]+', move):
                raise ValueError('Invalid move constant.')
            names.add(move)
        evs = mon['evs']
        if len(evs) != 6 or any(type(n) is not int or not 0 <= n <= 252 for n in evs) or sum(evs) > 510:
            raise ValueError('Invalid EV spread (HP/Atk/Def/SpA/SpD/Spe).')
    source = '#include <stdio.h>\n'
    source += ''.join(f'#include "constants/{header}.h"\n'
                      for header in ['species', 'moves', 'abilities', 'items', 'pokemon'])
    source += 'int main(void) {\n'
    source += ''.join(f'printf("{name} %u\\n", (unsigned){name});\n' for name in sorted(names))
    source += 'return 0; }\n'
    with tempfile.TemporaryDirectory(prefix='ec-prep-') as tmp:
        exe = str(Path(tmp) / 'constants')
        subprocess.run([shutil.which('cc'), '-Iinclude', '-x', 'c', '-', '-o', exe],
                       input=source, cwd=root, text=True, check=True, capture_output=True)
        output = subprocess.run([exe], text=True, check=True, capture_output=True).stdout
    constants = {name: int(value) for name, value in (line.split() for line in output.splitlines())}
    words = [('gEcAgentPrepResult', 0, 0), ('gEcAgentPrepPartyCount', 0, len(party))]
    for slot, mon in enumerate(party):
        for field, value in [('Species', constants[mon['species']]), ('Preset', KEEP),
                             ('Format', 0), ('Level', KEEP), ('Nature', constants[mon['nature']]),
                             ('Ability', constants[mon['ability']]), ('Item', constants[mon['item']])]:
            words.append(('gEcAgentPrep' + field, 4 * slot, value))
        for i, move in enumerate(mon['moves']):
            words.append(('gEcAgentPrepMoves', 4 * (slot * 4 + i), constants[move]))
        for i, ev in enumerate(mon['evs']):
            words.append(('gEcAgentPrepEvs', 4 * (slot * 6 + i), ev))
    return spec, words
