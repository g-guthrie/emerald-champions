"""Source-driven control-flow checks for story doubles entry preflight.

This executes only the real script preambles and shared rejection branch. It
proves ordering and termination, not movement execution or emulator behavior.
"""
import copy
import json
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
ENTRIES = {
    'PetalburgWoods': ('DevonResearcherLeft', 'DevonResearcherRight'),
    'MeteorFalls_1F_1R': ('MagmaStealsMeteoriteScene',),
    'SlateportCity_OceanicMuseum_2F': ('CaptStern',),
    'SeafloorCavern_Room9': ('ArchieAwakenKyogre',),
    'MtChimney': ('Maxie',),
    'MagmaHideout_4F': ('Maxie',),
    'JaggedPass': ('MagmaHideoutGuard',),
}
CHECK = 'EventScript_CheckStoryDoubleBattleParty'
# Entries whose already-resolved paths branch away before the preflight. This
# models the first-encounter path those branches fall through to; every other
# command still fails closed.
RESOLVED_PATH_BRANCHES = ('goto_if_set', 'goto_if_not_defeated')


def scripts():
    blocks = {}
    paths = [ROOT / 'data/scripts/trainer_battle.inc']
    paths += [ROOT / 'data/maps' / name / 'scripts.inc' for name in ENTRIES]
    for path in paths:
        label = None
        for line in path.read_text().splitlines():
            line = line.split('@', 1)[0].strip()
            match = re.fullmatch(r'(\w+)::?', line)
            if match:
                label = match[1]
                blocks[label] = []
            elif label and line:
                blocks[label].append(line)
    return blocks


def execute_preamble(blocks, entry, party_result):
    """Stop at first scene operation; fail closed on unknown preflight logic."""
    label, index, stack = entry, 0, []
    trace = []
    checked = False
    result = None
    for _ in range(100):
        line = blocks[label][index]
        index += 1
        command, _, tail = line.partition(' ')
        args = [arg.strip() for arg in tail.split(',')]
        trace.append(line)
        if command in ('lock', 'lockall', 'releaseall'):
            continue
        if command in RESOLVED_PATH_BRANCHES and not checked:
            continue
        if command == 'special' and args == ['HasEnoughMonsForDoubleBattle']:
            checked, result = True, party_result
        elif command == 'goto_if_ne':
            if args[:2] != ['VAR_RESULT', 'PLAYER_HAS_TWO_USABLE_MONS']:
                raise AssertionError(f'unsupported condition: {line}')
            if result != 'PLAYER_HAS_TWO_USABLE_MONS':
                label, index = args[2], 0
        elif command == 'call' and args == [CHECK]:
            stack.append((label, index))
            label, index = CHECK, 0
        elif command == 'return':
            label, index = stack.pop()
        elif command == 'msgbox' and args == ['EmeraldChampions_Text_NeedTwoPokemon', 'MSGBOX_DEFAULT']:
            continue
        elif command == 'end':
            return 'rejected', checked, trace
        else:
            return 'scene', checked, trace
    raise AssertionError('entry preflight did not terminate')


class StoryDoubleEntryTests(unittest.TestCase):
    def test_rejection_precedes_scene_changes_and_reentry_is_stable(self):
        blocks = scripts()
        for name, suffixes in ENTRIES.items():
            for suffix in suffixes:
                entry = f'{name}_EventScript_{suffix}'
                for party in ('PLAYER_HAS_ONE_MON', 'PLAYER_HAS_ONE_USABLE_MON'):
                    with self.subTest(entry=entry, party=party):
                        outcome, checked, trace = execute_preamble(blocks, entry, party)
                        self.assertEqual(outcome, 'rejected')
                        self.assertTrue(checked)
                        self.assertEqual(trace[-2:], ['releaseall', 'end'])
                        self.assertIn('msgbox EmeraldChampions_Text_NeedTwoPokemon, MSGBOX_DEFAULT', trace)
                        self.assertEqual(execute_preamble(blocks, entry, party), (outcome, checked, trace))
                outcome, checked, _ = execute_preamble(blocks, entry, 'PLAYER_HAS_TWO_USABLE_MONS')
                self.assertEqual(outcome, 'scene')
                self.assertTrue(checked)

    def test_detects_missing_or_late_preflight(self):
        blocks = scripts()
        for name, suffixes in ENTRIES.items():
            for suffix in suffixes:
                entry = f'{name}_EventScript_{suffix}'
                for late in (False, True):
                    mutated = copy.deepcopy(blocks)
                    mutated[entry].remove(f'call {CHECK}')
                    if late:
                        # Place it after the first real scene operation, which is
                        # not a fixed index once an entry branches its already
                        # resolved path away first.
                        scene = next(
                            i for i, line in enumerate(mutated[entry])
                            if line.split(' ', 1)[0] not in
                            ('lock', 'lockall', 'releaseall', *RESOLVED_PATH_BRANCHES)
                        )
                        mutated[entry].insert(scene + 1, f'call {CHECK}')
                    outcome, checked, _ = execute_preamble(mutated, entry, 'PLAYER_HAS_ONE_MON')
                    self.assertEqual(outcome, 'scene')
                    self.assertFalse(checked)

    def test_entries_are_interaction_or_step_triggers_not_frame_callbacks(self):
        for name, suffixes in ENTRIES.items():
            payload = json.loads((ROOT / 'data/maps' / name / 'map.json').read_text())
            script = (ROOT / 'data/maps' / name / 'scripts.inc').read_text()
            self.assertNotIn('MAP_SCRIPT_ON_FRAME_TABLE', script)
            step_or_talk = {event.get('script') for kind in ('coord_events', 'object_events')
                            for event in payload.get(kind, [])}
            for suffix in suffixes:
                self.assertIn(f'{name}_EventScript_{suffix}', step_or_talk)

    def test_museum_attrition_battles_keep_allow_single(self):
        source = (ROOT / 'data/maps/SlateportCity_OceanicMuseum_2F/scripts.inc').read_text()
        battles = re.findall(r'^\s*(trainerbattle_\w+) (TRAINER_\w+)', source, re.M)
        self.assertEqual(battles, [
            ('trainerbattle_no_intro_double', 'TRAINER_GRUNT_MUSEUM_1'),
            ('trainerbattle_no_intro_double_allow_single', 'TRAINER_GRUNT_MUSEUM_2'),
            ('trainerbattle_no_intro_double_allow_single', 'TRAINER_ARCHIE_SLATEPORT'),
        ])


if __name__ == '__main__':
    unittest.main()
