"""Execute all four production guide branches with controlled item delivery."""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run_scene(entry, room):
    commands, labels = [], {}
    for line in (ROOT / 'data/maps/RustboroCity_Gym/scripts.inc').read_text().splitlines():
        line = line.split('@')[0].strip()
        label = re.fullmatch(r'(\w+)::?', line)
        if label:
            labels[label[1]] = len(commands)
        elif line:
            commands.append(line)
    pc = labels[f'RustboroCity_Gym_EventScript_GuideTrigger{entry}']
    variables = {'VAR_RUSTBORO_GYM_GUIDE_STATE': 0}
    position, locked, count = [3, 18], False, 0
    for _ in range(100):
        cmd, _, tail = commands[pc].partition(' ')
        pc += 1
        args = [arg.strip() for arg in tail.split(',')]
        if cmd == 'end':
            return position, variables['VAR_RUSTBORO_GYM_GUIDE_STATE'], locked, count
        if cmd in ('msgbox', 'playse', 'waitmovement', 'closemessage'):
            continue
        if cmd == 'lockall':
            locked = True
        elif cmd == 'releaseall':
            locked = False
        elif cmd == 'setvar':
            variables[args[0]] = int(args[1])
        elif cmd == 'giveitem':
            assert args == ['ITEM_FRESH_WATER', '5']
            variables['VAR_RESULT'] = int(room)
            count += 5 if room else 0
        elif cmd == 'goto':
            pc = labels[args[0]]
        elif cmd == 'goto_if_eq':
            if variables[args[0]] == (0 if args[1] == 'FALSE' else int(args[1])):
                pc = labels[args[2]]
        elif cmd == 'applymovement':
            if args[0] == 'LOCALID_PLAYER' or args[1] in ('Common_Movement_ExclamationMark', 'Common_Movement_Delay48'):
                continue
            assert args[0] == 'LOCALID_RUSTBORO_GYM_GUIDE'
            cursor = labels[args[1]]
            while commands[cursor] != 'step_end':
                step = commands[cursor]
                if step.startswith('walk_'):
                    dx, dy = {'walk_up': (0,-1), 'walk_down': (0,1), 'walk_left': (-1,0), 'walk_right': (1,0)}[step]
                    position[0] += dx
                    position[1] += dy
                else:
                    assert step.startswith('face_'), step
                cursor += 1
        else:
            raise AssertionError('unsupported reachable command: ' + cmd)
    raise AssertionError('guide script did not terminate')


class GuideIntegrity(unittest.TestCase):
    def test_full_bag_returns_all_four_paths_without_completing(self):
        for entry in range(1, 5):
            with self.subTest(entry=entry):
                self.assertEqual(run_scene(entry, False), ([3,18], 0, False, 0))

    def test_success_returns_all_four_paths_and_delivers_five(self):
        for entry in range(1, 5):
            with self.subTest(entry=entry):
                self.assertEqual(run_scene(entry, True), ([3,18], 1, False, 5))


if __name__ == '__main__':
    unittest.main()
