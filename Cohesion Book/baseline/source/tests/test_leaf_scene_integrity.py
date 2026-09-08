"""Execute Leaf's actual script branches with controlled battle/inventory APIs.

This is a bounded script-state check, not engine timing or rendered evidence.
Unknown reachable commands fail rather than silently weakening the simulation.
"""
import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAP = ROOT / 'data/maps/AlteringCave_B1F'
COMPLETE = 'FLAG_EC_DEFEATED_LEAF_ALTERING_CAVE'


class LeafScene:
    def __init__(self, *, completed=False, defeated=False, room=True, party=True, item=False):
        self.commands, self.labels = [], {}
        for path in (MAP / 'scripts.inc', ROOT / 'data/scripts/trainer_battle.inc'):
            for line in path.read_text().splitlines():
                line = line.split('@')[0].strip()
                label = re.fullmatch(r'(\w+)::?', line)
                if label:
                    self.labels[label[1]] = len(self.commands)
                elif line:
                    self.commands.append(line)
        self.flags = {COMPLETE} if completed else set()
        self.defeated, self.room, self.party, self.item = defeated, room, party, item
        self.battles = self.gifts = self.moves = 0
        self.vars = {}
        self.reenter()

    def reenter(self):
        self.vars.clear()
        data = json.loads((MAP / 'map.json').read_text())
        self.trigger = data['coord_events'][0]
        leaf = next(o for o in data['object_events'] if o.get('local_id') == 'LOCALID_EC_LEAF')
        self.positions = {'LOCALID_EC_LEAF': (leaf['x'], leaf['y']),
                          'LOCALID_PLAYER': (self.trigger['x'], self.trigger['y'])}
        self.hidden = COMPLETE in self.flags

    def run(self):
        if self.vars.get(self.trigger['var'], 0) != int(self.trigger['var_value']):
            return
        pc, stack, result = self.labels[self.trigger['script']], [], False
        for _ in range(500):
            line = self.commands[pc]
            pc += 1
            cmd, _, tail = line.partition(' ')
            args = [a.strip() for a in tail.split(',')]
            if cmd in ('lockall', 'releaseall', 'playse', 'waitmovement', 'delay', 'closemessage', 'msgbox'):
                continue
            if cmd == 'end':
                return
            if cmd == 'return':
                pc = stack.pop()
            elif cmd == 'call':
                stack.append(pc)
                pc = self.labels[args[0]]
            elif cmd == 'goto':
                pc = self.labels[args[0]]
            elif cmd == 'goto_if_set':
                if args[0] in self.flags:
                    pc = self.labels[args[1]]
            elif cmd == 'goto_if_defeated':
                if self.defeated:
                    pc = self.labels[args[1]]
            elif cmd in ('goto_if_eq', 'goto_if_ne'):
                constants = {'TRUE': True, 'FALSE': False, 'PLAYER_HAS_TWO_USABLE_MONS': True}
                matches = result == constants[args[1]]
                if matches == (cmd == 'goto_if_eq'):
                    pc = self.labels[args[2]]
            elif cmd == 'special' and args[0] == 'HasEnoughMonsForDoubleBattle':
                result = self.party
            elif cmd == 'applymovement':
                actor, movement = args
                if actor == 'LOCALID_EC_LEAF' and self.hidden:
                    raise AssertionError('choreography addresses hidden Leaf')
                self.moves += 1
                if movement in ('Common_Movement_ExclamationMark', 'Common_Movement_WalkInPlaceFasterDown'):
                    continue  # Known common reactions do not translate the actor.
                x, y = self.positions[actor]
                cursor = self.labels[movement]
                while self.commands[cursor] != 'step_end':
                    step = self.commands[cursor]
                    if step in ('walk_up', 'walk_down', 'walk_left', 'walk_right'):
                        dx, dy = {'walk_up': (0, -1), 'walk_down': (0, 1),
                                  'walk_left': (-1, 0), 'walk_right': (1, 0)}[step]
                        x, y = x + dx, y + dy
                    elif not step.startswith(('walk_in_place_', 'face_')):
                        raise AssertionError('unsupported movement: ' + step)
                    cursor += 1
                self.positions[actor] = x, y
            elif cmd == 'trainerbattle_no_intro_double':
                if not self.party:
                    return  # Native rejection ends the scene after its dialogue.
                self.battles += 1
                self.defeated = True  # Controlled winning battle.
            elif cmd == 'checkitem':
                result = self.item
            elif cmd == 'giveitem':
                result = self.room
                if result:
                    self.gifts += 1
                    self.item = True
            elif cmd == 'setflag':
                self.flags.add(args[0])
            elif cmd == 'removeobject':
                self.hidden = True
            elif cmd == 'setvar':
                self.vars[args[0]] = int(args[1])
            else:
                raise AssertionError('unsupported command: ' + line)
        raise AssertionError('script did not terminate')


class LeafSceneIntegrity(unittest.TestCase):
    def test_completed_save_precedes_trainer_flag(self):
        for defeated in (False, True):
            scene = LeafScene(completed=True, defeated=defeated)
            scene.run()
            self.assertEqual((scene.battles, scene.gifts, scene.moves), (0, 0, 0))

    def test_first_win_and_reentry(self):
        scene = LeafScene()
        scene.run()
        self.assertEqual((scene.battles, scene.gifts), (1, 1))
        self.assertEqual(scene.positions['LOCALID_PLAYER'], (20, 18))
        self.assertIn(COMPLETE, scene.flags)
        scene.reenter()
        scene.run()
        self.assertEqual((scene.battles, scene.gifts), (1, 1))

    def test_full_bag_retry_without_repeat_battle(self):
        for reenter in (False, True):
            scene = LeafScene(room=False)
            scene.run()
            self.assertNotIn(COMPLETE, scene.flags)
            self.assertEqual(scene.positions['LOCALID_EC_LEAF'], (21, 16))
            self.assertEqual(scene.positions['LOCALID_PLAYER'], (21, 19))
            scene.run()  # Repeated full-bag attempt stays stable.
            self.assertEqual(scene.battles, 1)
            if reenter:
                scene.reenter()
            scene.room = True
            scene.party = False  # Earned reward does not require battle readiness.
            scene.run()
            self.assertEqual((scene.battles, scene.gifts), (1, 1))
            self.assertIn(COMPLETE, scene.flags)

    def test_insufficient_party_does_not_move_actors(self):
        scene = LeafScene(party=False)
        scene.run()
        self.assertEqual((scene.moves, scene.battles, scene.gifts), (0, 0, 0))
        self.assertNotIn(COMPLETE, scene.flags)

    def test_existing_item_is_not_duplicated(self):
        scene = LeafScene(item=True)
        scene.run()
        self.assertEqual((scene.battles, scene.gifts), (1, 0))
        self.assertIn(COMPLETE, scene.flags)


if __name__ == '__main__':
    unittest.main()
